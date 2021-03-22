#include "box.h"

#include "aa_rect.h"
#include "flip_normals.h"

Box::Box(const Vec3& extents, std::shared_ptr<IMaterial> material)
{
    Vec3 mins = extents * -0.5;
    Vec3 maxs = extents * 0.5;

    // +X
    sides_.add(std::make_shared<RectangleYZ>(mins.y, maxs.y, mins.z, maxs.z, maxs.x, material));

    // -X
    sides_.add(std::make_shared<FlipNormals>(std::make_shared<RectangleYZ>(mins.y, maxs.y, mins.z, maxs.z, mins.x, material)));

    // +Y
    sides_.add(std::make_shared<RectangleXZ>(mins.x, maxs.x, mins.z, maxs.z, maxs.y, material));

    // -Y
    sides_.add(std::make_shared<FlipNormals>(std::make_shared<RectangleXZ>(mins.x, maxs.x, mins.z, maxs.z, mins.y, material)));

    // +Z
    sides_.add(std::make_shared<RectangleXY>(mins.x, maxs.x, mins.y, maxs.y, maxs.z, material));

    // -Z
    sides_.add(std::make_shared<FlipNormals>(std::make_shared<RectangleXY>(mins.x, maxs.x, mins.y, maxs.y, mins.z, material)));
}

bool Box::hit(const Ray& r, double tMin, double tMax, HitRecord& hit) const
{
    return sides_.hit(r, tMin, tMax, hit);
}
