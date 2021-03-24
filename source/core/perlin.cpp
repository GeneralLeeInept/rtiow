#include "perlin.h"

#include "core/rng.h"

static int* generatePerm(int pointCount, Rng& rng);
static void permute(int* p, int n, Rng& rng);
static double perlinInterp(Vec3 c[2][2][2], double u, double v, double w);

Perlin::Perlin()
{
    Rng rng;
    ranvec_ = new Vec3[pointCount];

    for (int i = 0; i < pointCount; ++i)
    {
        ranvec_[i] = normalize(Vec3(rng(-1, 1), rng(-1, 1), rng(-1, 1)));
    }

    xperm_ = generatePerm(pointCount, rng);
    yperm_ = generatePerm(pointCount, rng);
    zperm_ = generatePerm(pointCount, rng);
}

Perlin::~Perlin()
{
    delete[] ranvec_;
    delete[] xperm_;
    delete[] yperm_;
    delete[] zperm_;
}

double Perlin::noise(const Vec3& p) const
{
    auto u = p.x - std::floor(p.x);
    auto v = p.y - std::floor(p.y);
    auto w = p.z - std::floor(p.z);

    auto i = static_cast<int>(std::floor(p.x));
    auto j = static_cast<int>(std::floor(p.y));
    auto k = static_cast<int>(std::floor(p.z));
    Vec3 c[2][2][2];

    for (int di = 0; di < 2; di++)
    {
        for (int dj = 0; dj < 2; dj++)
        {
            for (int dk = 0; dk < 2; dk++)
            {
                c[di][dj][dk] = ranvec_[xperm_[(i + di) & 255] ^ yperm_[(j + dj) & 255] ^ zperm_[(k + dk) & 255]];
            }
        }
    }

    return perlinInterp(c, u, v, w);
}

double Perlin::turb(const Vec3& p, int depth ) const
{
    double accum = 0;
    Vec3 temp_p = p;
    double weight = 1;

    for (int i = 0; i < depth; i++)
    {
        accum += weight * noise(temp_p);
        weight *= 0.5;
        temp_p *= 2;
    }

    return std::abs(accum);
}

int* generatePerm(int pointCount, Rng& rng)
{
    auto p = new int[pointCount];

    for (int i = 0; i < pointCount; i++)
    {
        p[i] = i;
    }

    permute(p, pointCount, rng);

    return p;
}

void permute(int* p, int n, Rng& rng)
{
    for (int i = n - 1; i > 0; i--)
    {
        int target = rng.randomInt(0, i);
        int tmp = p[i];
        p[i] = p[target];
        p[target] = tmp;
    }
}

double perlinInterp(Vec3 c[2][2][2], double u, double v, double w)
{
    auto uu = u * u * (3 - 2 * u);
    auto vv = v * v * (3 - 2 * v);
    auto ww = w * w * (3 - 2 * w);
    auto accum = 0.0;

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            for (int k = 0; k < 2; k++)
            {
                Vec3 weight_v(u-i, v-j, w-k);
                accum += (i * uu + (1 - i) * (1 - uu)) * (j * vv + (1 - j) * (1 - vv)) * (k * ww + (1 - k) * (1 - ww)) * dot(c[i][j][k], weight_v);
            }
        }
    }

    return accum;
}