#ifndef TRAJECTORYSIMULATION_BALLMATH_H
#define TRAJECTORYSIMULATION_BALLMATH_H
#include <vector>

namespace ballMath
{
    /**
     * Calculates the x coordinate of the point on the ball by given fi and theta in radians
     * @param r ball radius
     * @param fi longitude
     * @param theta latitude
     * @return the x coordinate
     */
    double calculate_x(double r, double fi, double theta);
    /**
     * Calculates the y coordinate of the point on the ball by given fi and theta in radians
     * @param r ball radius
     * @param fi longitude
     * @param theta latitude
     * @return the y coordinate
     */
    double calculate_y(double r, double fi, double theta);
    /**
     * Calculates the z coordinate of the point on the ball by given fi and theta in radians
     * @param r ball radius
     * @param theta latitude
     * @return the z coordinate
     */
    double calculate_z(double r, double theta);

    /**
     * Calculates radians based on given degrees
     * @param degrees
     * @return
     */
    double degrees_to_radians(double degrees);

    std::vector<double> get_coords(double r, double latitude, double longitude);
}




#endif //TRAJECTORYSIMULATION_BALLMATH_H
