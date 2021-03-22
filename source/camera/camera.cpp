#include "camera.h"

#include "core/rng.h"
#include "core/rtiow.h"

Camera::Camera(const Vec3& position, const Vec3& target, const Vec3& vup, double fovy, double aspectRatio, double aperature, double focalDistance)
{
    
    w_ = normalize(target - position);
    u_ = normalize(glm::cross(w_, vup));
    v_ = cross(u_, w_);

    double h = std::tan(fovy / 2.0);
    double viewportHeight = 2.0 * h;
    double viewportWidth = viewportHeight * aspectRatio;

    position_ = position;
    horizontal_ = focalDistance * viewportWidth * u_;
    vertical_ = focalDistance * viewportHeight * v_;
    lowerLeftCorner_ = position_ - horizontal_ / 2.0 - vertical_ / 2.0 + focalDistance * w_;

    lensRadius_ = aperature * 0.5;
}

Ray Camera::createRay(Rng& rng, double s, double t) const
{
    Vec3 rd = lensRadius_ * rng.inUnitDisk();
    Vec3 offset = u_ * rd.x + v_ * rd.y;
    return Ray(position_ + offset, normalize(lowerLeftCorner_ + s * horizontal_ + t * vertical_ - position_ - offset), true);
}
