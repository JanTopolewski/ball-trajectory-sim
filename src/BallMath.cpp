#define _USE_MATH_DEFINES
#include <cmath>
#include "../include/BallMath.h"

namespace ballMath
{
    double calculate_x(const double r, const double fi, const double theta)
    {
        return r * sin(theta) * cos(fi);
    }

    double calculate_y(const double r, const double fi, const double theta)
    {
        return r * sin(theta) * sin(fi);
    }

    double calculate_z(const double r, const double theta)
    {
        return r * cos(theta);
    }

    double degrees_to_radians(const double degrees)
    {
        return degrees * M_PI / 180;
    }

    std::vector<double> get_coords(const double r, const double latitude, const double longitude)
    {
        std::vector<double> outputs;
        const double fi = degrees_to_radians(longitude);
        const double theta = degrees_to_radians(latitude);
        outputs.push_back(calculate_x(r, fi, theta));
        outputs.push_back(calculate_y(r, fi, theta));
        outputs.push_back(calculate_z(r, theta));
        return outputs;
    }
}
