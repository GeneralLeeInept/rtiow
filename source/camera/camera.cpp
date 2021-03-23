#include "camera.h"

#include "core/rng.h"
#include "core/rtiow.h"

Camera::Camera(const CreateInfo& createInfo, double aspectRatio)
{
    w_ = normalize(createInfo.target - createInfo.position);
    u_ = normalize(cross(w_, createInfo.vup));
    v_ = cross(u_, w_);

    double h = std::tan(createInfo.fovy / 2.0);
    double viewportHeight = 2.0 * h;
    double viewportWidth = viewportHeight * aspectRatio;

    position_ = createInfo.position;
    horizontal_ = createInfo.focalDistance * viewportWidth * u_;
    vertical_ = createInfo.focalDistance * viewportHeight * v_;
    lowerLeftCorner_ = position_ - horizontal_ / 2.0 - vertical_ / 2.0 + createInfo.focalDistance * w_;
    lensRadius_ = createInfo.aperature * 0.5;
    timeBegin_ = createInfo.timeBegin;
    timeEnd_ = createInfo.timeEnd;
}

Ray Camera::createRay(Rng& rng, double s, double t) const
{
    Vec3 rd = lensRadius_ * rng.inUnitDisk();
    Vec3 offset = u_ * rd.x + v_ * rd.y;
    double time = rng(timeBegin_, timeEnd_);
    return Ray(position_ + offset, normalize(lowerLeftCorner_ + s * horizontal_ + t * vertical_ - position_ - offset), time, true);
}
