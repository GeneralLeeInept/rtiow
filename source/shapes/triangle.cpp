#include "triangle.h"

#include "core/hit_record.h"

#include "tiny_obj_loader.h"

#include <iostream>

static bool intersect(const Ray& r, double tMin, double tMax, const Vec3& a, const Vec3& b, const Vec3& c, Vec3& n, double& t, double& u, double &v)
{
    // Calculate supporting plane normal direction
    Vec3 ab = b - a; 
    Vec3 ac = c - a; 
    n = cross(ab, ac);
 
    // Calculate t of intersection between ray and supporting plane
    t = -(dot(n, r.origin) - dot(n, a)) / dot(n, r.direction);

    if (t < tMin || t >= tMax)
    {
        return false;
    }

    // Check if intersection is inside triangle (and get barycentric coordinates)
    Vec3 p = r.at(t);
 
    Vec3 ap = p - a; 
    Vec3 area  = cross(ab, ap);

    if (dot(n, area) < 0)
    {
        return false;
    }
 
    Vec3 bc = c - b;
    Vec3 bp = p - b;
    area = cross(bc, bp);
    u = dot(n, area);

    if (u < 0)
    {
        return false;
    }
 
    Vec3 ca = a - c;
    Vec3 cp = p - c;
    area = cross(ca, cp);
    v = dot(n, area);

    if (v < 0)
    {
        return false;
    }
 
    double oodenom = 1.0 / length2(n);
    n = normalize(n);
    u *= oodenom;
    v *= oodenom;

    return true;
}

static Aabb getBounds(const Vec3& v0, const Vec3& v1, const Vec3& v2)
{
    Aabb bounds{ v0, v0 };

    for (int a = 0; a < 3; ++a)
    {
        bounds.mins[a] = std::min(bounds.mins[a], std::min(v1[a], v2[a]));
        bounds.maxs[a] = std::max(bounds.maxs[a], std::max(v1[a], v2[a]));

        if (bounds.mins[a] == bounds.maxs[a])
        {
            bounds.mins[a] -= 0.0005;
            bounds.maxs[a] += 0.0005;
        }
    }

    return bounds;
}

TriangleMesh::TriangleMesh(std::shared_ptr<IMaterial> material)
    : material_(material)
    , positions_(nullptr)
    , normals_(nullptr)
    , uvs_(nullptr)
    , numtris_(0)
{
}

TriangleMesh::~TriangleMesh()
{
    delete[] uvs_;
    delete[] normals_;
    delete[] positions_;
}

bool TriangleMesh::load(const std::string& objFile)
{
    tinyobj::ObjReaderConfig config{};
    config.triangulate = true;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(objFile, config))
    {
        return false;
    }

    const tinyobj::attrib_t& attrib = reader.GetAttrib();
    const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();

    for (const auto& shape : shapes)
    {
        numtris_ += shape.mesh.num_face_vertices.size();
    }

    positions_ = new Vec3[numtris_ * 3];
    normals_ = new Vec3[numtris_ * 3];
    uvs_ = new float[numtris_ * 3 * 2];

    Vec3* positionWrite = positions_;
    Vec3* normalWrite = normals_;
    float* uvWrite = uvs_;

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            positionWrite->x = attrib.vertices[index.vertex_index * 3 + 0];
            positionWrite->y = attrib.vertices[index.vertex_index * 3 + 1];
            positionWrite->z = attrib.vertices[index.vertex_index * 3 + 2];

            if (index.normal_index >= 0)
            {
                normalWrite->x = attrib.vertices[index.normal_index * 3 + 0];
                normalWrite->y = attrib.vertices[index.normal_index * 3 + 1];
                normalWrite->z = attrib.vertices[index.normal_index * 3 + 2];
            }
            else
            {
                // TODO: generate normals
                std::cerr << "OBJ file does not contain normals.\n";
                return false;
            }

            if (index.texcoord_index >= 0)
            {
                uvWrite[0] = attrib.texcoords[index.texcoord_index * 2 + 0];
                uvWrite[1] = attrib.texcoords[index.texcoord_index * 2 + 1];
            }
            else
            {
                uvWrite[0] = 0.0;
                uvWrite[1] = 0.0;
            }

            positionWrite++;
            normalWrite++;
            uvWrite += 2;
        }
    }

    bool first = true;

    for (size_t i = 0; i < numtris_; ++i)
    {
        Aabb tribounds = getBounds(positions_[i * 3], positions_[i * 3 + 1], positions_[i * 3 + 2]);
        bounds_ = first ? tribounds : bounds_.makeUnion(tribounds);
        first = false;
    }

    return true;
}

bool TriangleMesh::hit(const Ray& r, double tMin, double tMax, HitRecord& hitRecord) const
{
    if (!bounds_.hit(r, tMin, tMax))
    {
        return false;
    }

    bool hitAny = false;

    for (size_t i = 0; i < numtris_; ++i)
    {
        Vec3 n;
        double t, u, v;

        if (intersect(r, tMin, tMax, positions_[i * 3], positions_[i * 3 + 1], positions_[i * 3 + 2], n, t, u, v))
        {
            tMax = t;
            hitRecord.t = t;
            hitRecord.setFaceNormal(r, n);
            hitRecord.u = u;
            hitRecord.v = v;
            hitAny = true;
        }
    }

    if (!hitAny)
    {
        return false;
    }

    hitRecord.p = r.at(hitRecord.t);
    hitRecord.material = material_.get();

    return true;
}

bool TriangleMesh::boundingBox(double startTime, double endTime, Aabb& bbox) const
{
    bbox = bounds_;
    return true;
}

Triangle::Triangle(const Vec3& a, const Vec3& b, const Vec3& c, std::shared_ptr<IMaterial> material)
    : a_(a)
    , b_(b)
    , c_(c)
    , material_(material)
{
}

bool Triangle::hit(const Ray& r, double tMin, double tMax, HitRecord& hitRecord) const
{
    Vec3 n;
    double t, u, v;

    if (!intersect(r, tMin, tMax, a_, b_, c_, n, t, u, v))
    {
        return false;
    }

    hitRecord.t = t;
    hitRecord.p = r.at(t);
    hitRecord.setFaceNormal(r, n);
    hitRecord.u = u;
    hitRecord.v = v;
    hitRecord.material = material_.get();
    return true;
} 

bool Triangle::boundingBox(double startTime, double endTime, Aabb& bbox) const
{
    bbox = getBounds(a_, b_, c_);
    return true;
}
