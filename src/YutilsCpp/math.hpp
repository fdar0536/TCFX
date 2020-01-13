#ifndef MATH_HPP
#define MATH_HPP

#include <vector>
#include <tuple>
#include <string>

#include "common.hpp"
#include "../common/basecommon.hpp"
#include "config.h"

namespace Yutils
{

class SYMBOL_SHOW Math : protected Common
{
public:
    Math();

    /**
     * @brief arc_curve
     * Converts arc data to bezier curves.
     * x & y is the arc starting point,
     * cx & cy the arc center (= orientation point to keep the same distance to all arc points)
     * and angle the angle in degree of the arc.
     * For each 90° one curve is generated, so a maximum of 4 curves can span a circle.
     * Curves are 3rd order bezier curves.
     * It returns a vector conatins pairs.
     * Each pair is one of the control points of a bezier curve.
     * Every four pairs describe a bezier curve.
     *
     * @throw std::invalid_argument When angle is less than -360 or
     *                              is greater than 360 or
     *                              is equal to 0, this function will throw
     *                              std::invalid_argument.
     */
    std::vector<std::pair<double, double>> arc_curve(double x, double y,
                                                     double cx, double cy,
                                                     double angle);

    /**
     * @brief bezier
     * Calculates a point on a bezier curve of any order.
     *
     * @param pct The position on the curve in range 0<=x<=1.
     * @param pts A vector contains tuples,
     *            each tuple containing 3 numbers as curve point.
     *            the size of this vector MUST greater than 1.
     * @param is3D It indicates pts is 3D or not.
     *             If is3D is set to false,
     *             the third number of tuples of pts will be ignored,
     *             and the third number of returned tuple always is zero.
     * @throw std::invalid_argument
     */
    std::tuple<double, double, double> bezier(double pct,
                std::vector<std::tuple<double, double, double>> &pts,
                bool is3D = false);

    /**
     * @brief degree
     * Calculates the degree between vectors x1|y1|z1 and x2|y2|z3
     */
    double degree(double x1, double y1, double z1,
                  double x2, double y2, double z2);

    /**
     * @brief distance
     * Calculates length of given vector.
     */
    double distance(double x, double y, double z = 0.f);

    /**
     * @brief line_intersect
     * Calculates intersection point of two lines.
     * x0, y0, x1, y1 are both points of line 1,
     * x2, y2, x3, y3 are points of line 2.
     * strict is a flag, determining the intersection has to be located on the lines.
     * x, y can be the intersection point.
     * If both lines are parallel, it will return empty pair.
     * If strict is true and there's no intersection on the strict length lines,
     * it will return inf (std::make_pair(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity())).
     *
     * @throw std::invalid_argument If one of lines or both have zero length,
     *                              this function will throw std::invalid_argument.
     */
    std::pair<double, double> line_intersect(double x0, double y0,
                                             double x1, double y1,
                                             double x2, double y2,
                                             double x3, double y3,
                                             bool strict);

    //Get orthogonal vector of 2 given vectors
    std::tuple<double, double, double> ortho(double x1,
                                             double y1,
                                             double z1,
                                             double x2,
                                             double y2,
                                             double z2);

    // Generates a random number in given range with specific item distance
    // it may throw invalid_argument
    double randomsteps(double min, double max, double step);

    // Rounds number
    double round(double x, double dec = 0.f);

    // Scales vector to given length
    std::tuple<double, double, double> stretch(double x, double y,
                                               double z, double length);

    // Trim number in range
    // it may throw invalid_argument
    double trim(double x, double min, double max);

    std::pair<double, double> ellipse(double x, double y,
                                      double w, double h,
                                      double a);

    double randomway();

    // rotate may throw invalid_argument
    std::tuple<double, double, double> rotate(std::tuple<double, double, double> p,
                                              std::string axis,
                                              double angle);

private:
    std::tuple<double, double, double> bezier2(double pct,
                     std::vector<std::tuple<double, double, double>> &pts,
                                               bool is3D);

    std::tuple<double, double, double> bezier3(double pct,
                     std::vector<std::tuple<double, double, double>> &pts,
                                               bool is3D);

    std::tuple<double, double, double> bezier4(double pct,
                     std::vector<std::tuple<double, double, double>> &pts,
                                               bool is3D);

    std::tuple<double, double, double> bezierN(double pct,
                     std::vector<std::tuple<double, double, double>> &pts,
                                               bool is3D);

    double fac(double n);
};

}

#endif // MATH_HPP
