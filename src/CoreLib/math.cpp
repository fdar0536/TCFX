#include <iostream>
#include <algorithm>
#include <limits>

#include <cmath>

#include "boost/math/constants/constants.hpp"

#include "math.hpp"

using namespace std;

CoreMath::CoreMath()
{
    CoreCommon();
}

// public member function
vector<pair<double, double>> CoreMath::arc_curve(double x, double y,
                                    double cx, double cy, double angle)
{
    if (angle < -360.f || angle > 360.f)
    {
        throw invalid_argument("start & center point and valid angle (-360<=x<=360) expected");
    }

    if (angle == 0.f)
    {
        throw invalid_argument("angle CANNOT be zero");
    }

    // Factor for bezier control points distance to node points
    double kappa(4.f * (sqrt(2.f) - 1.f) / 3.f);
    double rx0, ry0, rx1, ry1, rx2, ry2, rx3(0.f), ry3, rx03, ry03;

    rx0 = x - cx;
    ry0 = y - cy;

    double cw = (angle > 0.f ? 1.f : -1.f);

    // 把do while loop轉換成while loop
    size_t curves_n(4);
    vector<pair<double, double>> curves;
    curves.reserve(curves_n);
    double angle_sum(0.f);
    double cur_angle_pct;
    pair<double, double> tmpPair;
    tuple<double, double, double> tmpTuple;
    double tmpDouble;

    while (angle_sum < angle)
    {
        cur_angle_pct = min((angle - angle_sum), static_cast<double>(90.f)) / 90.f;
        tmpPair = rotate2d(rx0, ry0, cw * 90.f * cur_angle_pct);

        // 下面會有用
        rx3 = tmpPair.first;
        ry3 = tmpPair.second;

        // Get arc start to end vector
        rx03 = tmpPair.first - rx0;
        ry03 = tmpPair.second - ry0;

        // Scale arc vector to curve node <-> control point distance
        tmpDouble = distance(rx03, ry03);
        tmpDouble = pow(tmpDouble, 2) / 2.f;
        tmpDouble = sqrt(tmpDouble);
        tmpTuple = stretch(rx03, ry03, 0, tmpDouble * kappa);
        rx03 = get<0>(tmpTuple);
        ry03 = get<1>(tmpTuple);

        // Get curve control points
        tmpPair = rotate2d(rx03, ry03, cw * (-45.f) * cur_angle_pct);
        rx1 = rx0 + tmpPair.first;
        ry1 = ry0 + tmpPair.second;

        tmpPair = rotate2d(rx03 * -1.f,
                                        ry03 * -1.f,
                                        cw * 45.f * cur_angle_pct);
        rx2 = rx3 + tmpPair.first;
        ry2 = ry3 + tmpPair.second;

        // Insert curve to output
        curves.push_back(make_pair(cx + rx0, cy + ry0));
        curves.push_back(make_pair(cx + rx1, cy + ry1));
        curves.push_back(make_pair(cx + rx2, cy + ry2));
        curves.push_back(make_pair(cx + rx3, cy + ry3));
        curves_n += 4;
        curves.reserve(curves_n);

        // Prepare next curve
        rx0 = rx3;
        ry0 = ry3;
        angle_sum += 90.f;
    }

    return curves;
}

tuple<double, double, double> CoreMath::bezier(double pct,
                        vector<tuple<double, double, double>> &pts,
                        bool is3D)
{
    if (pct < 0.f || pct > 1.f)
    {
        throw invalid_argument("pct must between 0 and 1");
    }

    size_t ptsSize(pts.size());
    if (ptsSize < 2)
    {
        throw invalid_argument("at least 2 points expected");
    }

    switch (ptsSize)
    {
    case 2:
        return bezier2(pct, pts, is3D);
    case 3:
        return bezier3(pct, pts, is3D);
    case 4:
        return bezier4(pct, pts, is3D);
    }

    return bezierN(pct, pts, is3D);
}

double CoreMath::degree(double x1, double y1, double z1,
                        double x2, double y2, double z2)
{
    using namespace boost::math::double_constants;
    double degree = distance(x1, y1, z1) * distance(x2, y2, z2);
    degree = acos((x1 * x2 + y1 * y2 + z1 * z2) / degree);
    degree = deg(degree);

    // Return with sign by clockwise direction
    if ((x1 * y2 - y1 * x2) < 0)
    {
        return (degree * -1.f);
    }

    return degree;
}

double CoreMath::distance(double x, double y, double z)
{
    return sqrt(x * x + y * y + z * z);
}

pair<double, double> CoreMath::line_intersect(double x0, double y0,
                                              double x1, double y1,
                                              double x2, double y2,
                                              double x3, double y3,
                                              bool strict)
{
    // Get line vectors & check valid lengths
    double x10(x0 - x1);
    double y10(y0 - y1);
    double x32(x2 - x3);
    double y32(y2 - y3);

    if ((x10 == 0.f && y10 == 0.f) ||
        (x32 == 0.f && y32 == 0.f))
    {
        throw invalid_argument("lines mustn't have zero length");
    }

    // Calculate determinant and check for parallel lines
    double det = x10 * y32 - y10 * x32;
    if (det == 0.f)
    {
        return pair<double, double>();
    }

    // Calculate line intersection (endless line lengths)
    double pre(x0 * y1 - y0 * x1);
    double post(x2 * y3 - y2 * x3);

    double ix = (pre * x32 - x10 * post) / det;
    double iy = (pre * y32 - y10 * post) / det;

    if (strict)
    {
        double s = (x10 != 0.f ? (ix - x1) / x10 : (iy - y1) / y10);
        double t = (x32 != 0.f ? (ix - x3) / x32 : (iy - y3) / y32);

        if (s < 0.f || s > 1.f || t < 0.f || t > 1.f)
        {
            return make_pair(numeric_limits<double>::infinity(), numeric_limits<double>::infinity());
        }
    }

    return make_pair(ix, iy);
}

tuple<double, double, double> CoreMath::ortho(double x1, double y1, double z1,
                            double x2, double y2, double z2)
{
    return make_tuple(y1 * z2 - z1 * y2,
				z1 * x2 - x1 * z2,
				x1 * y2 - y1 * x2);
}

double CoreMath::randomsteps(double min, double max, double step)
{
    if (max < min || step <= 0)
    {
        throw invalid_argument("Invalid input!");
    }

    return std::min(min + random(0, ceil((max - min) / step)) * step, max);
}

double CoreMath::round(double x, double dec)
{
    // Return number rounded to wished decimal size
    if (dec != 0.f && dec >= 1.f)
    {
        dec = pow(10, floor(dec));
        return floor(x * dec + 0.5) / dec;
    }

    return floor(x + 0.5);
}

tuple<double, double, double> CoreMath::stretch(double x, double y,
                                                double z, double length)
{
    double cur_length(distance(x, y, z));
    if (cur_length == 0.f)
    {
        return make_tuple(0.f, 0.f, 0.f);
    }

    double factor(length / cur_length);
    return make_tuple(x * factor, y * factor, z * factor);
}

double CoreMath::trim(double x, double min, double max)
{
    if (max < min)
    {
        throw invalid_argument("Invalid input!");
    }

    return (x < min ? min : (x > max ? max : x));
}

pair<double, double> CoreMath::ellipse(double x, double y,
                                       double w, double h,
                                       double a)
{
    double ra(rad(a));
    return make_pair(x + w / 2.f * sin(ra),
                     y + h / 2.f * cos(ra));
}

double CoreMath::randomway()
{
    double ret;
    while(1)
    {
        ret = random(0, 1) * 2.f - 1.f;
        if (ret != 0.f)
        {
            break;
        }
    }

    return (ret < 0.f ? -1.f : 1.f);
}

tuple<double, double, double> CoreMath::rotate(tuple<double, double, double> p,
                                               string axis,
                                               double angle)
{
    if (axis != "x" && axis != "y" && axis != "z")
    {
        throw invalid_argument("invalid axis");
    }

    double ra(rad(angle));

    // Is here has any better way to solve this problem?
    if (axis == "x")
    {
        return make_tuple(get<0>(p),
                          cos(ra) * get<1>(p) - sin(ra) * get<2>(p),
                          sin(ra) * get<1>(p) + cos(ra) * get<2>(p));
    }

    if (axis == "y")
    {
        return make_tuple(cos(ra) * get<0>(p) + sin(ra) * get<2>(p),
                          get<1>(p),
                          cos(ra) * get<2>(p) - sin(ra) * get<0>(p));
    }

    return make_tuple(cos(ra) * get<0>(p) - sin(ra) * get<1>(p),
                      sin(ra) * get<0>(p) + cos(ra) * get<1>(p),
                      get<2>(p));
}

// private member function
tuple<double, double, double> CoreMath::bezier2(double pct,
                         vector<tuple<double, double, double>> &pts, bool is3D)
{
    double pts1(0.f), pts2(0.f), pts3(0.f), pct_inv(1 - pct);
    
    pts1 = pct_inv * get<0>(pts.at(0)) + pct * get<0>(pts.at(1));
    pts2 = pct_inv * get<1>(pts.at(0)) + pct * get<1>(pts.at(1));
    if (is3D)
    {
        pts3 = pct_inv * get<2>(pts.at(0)) + pct * get<2>(pts.at(1));
    }

    return make_tuple(pts1, pts2, pts3);
}

tuple<double, double, double> CoreMath::bezier3(double pct,
                         vector<tuple<double, double, double>> &pts, bool is3D)
{
    double pts1(0.f), pts2(0.f), pts3(0.f), pct_inv(1 - pct);

    pts1 = pct_inv * pct_inv * get<0>(pts.at(0));
    pts1 += (2.f * pct_inv * pct * get<0>(pts.at(1)));
    pts1 += (pct * pct * get<0>(pts.at(2)));

    pts2 = pct_inv * pct_inv * get<1>(pts.at(0));
    pts2 += (2.f * pct_inv * pct * get<1>(pts.at(1)));
    pts2 += (pct * pct * get<1>(pts.at(2)));

    if (is3D)
    {
        pts3 = pct_inv * pct_inv * get<2>(pts.at(0));
        pts3 += (2.f * pct_inv * pct * get<2>(pts.at(1)));
        pts3 += (pct * pct * get<2>(pts.at(2)));
    }

    return make_tuple(pts1, pts2, pts3);
}

tuple<double, double, double> CoreMath::bezier4(double pct,
                         vector<tuple<double, double, double>> &pts, bool is3D)
{
    double pts1(0.f), pts2(0.f), pts3(0.f), pct_inv(1 - pct);

    pts1 = pct_inv * pct_inv * pct_inv * get<0>(pts.at(0));
    pts1 += (3.f * pct_inv * pct_inv * pct * get<0>(pts.at(1)));
    pts1 += (3.f * pct_inv * pct * pct * get<0>(pts.at(2)));
    pts1 += (pct * pct * pct * get<0>(pts.at(3)));

    pts2 = pct_inv * pct_inv * pct_inv * get<1>(pts.at(0));
    pts2 += (3.f * pct_inv * pct_inv * pct * get<1>(pts.at(1)));
    pts2 += (3.f * pct_inv * pct * pct * get<1>(pts.at(2)));
    pts2 += (pct * pct * pct * get<1>(pts.at(3)));

    if (is3D)
    {
        pts3 = pct_inv * pct_inv * pct_inv * get<2>(pts.at(0));
        pts3 += (3.f * pct_inv * pct_inv * pct * get<2>(pts.at(1)));
        pts3 += (3.f * pct_inv * pct * pct * get<2>(pts.at(2)));
        pts3 += (pct * pct * pct * get<2>(pts.at(3)));
    }

    return make_tuple(pts1, pts2, pts3);
}

tuple<double, double, double> CoreMath::bezierN(double pct,
                         vector<tuple<double, double, double>> &pts, bool is3D)
{
    double ret_x(0.f), ret_y(0.f), ret_z(0.f), pct_inv(1 - pct);
    size_t ni(pts.size() - 1);
    double nd = static_cast<double>(ni);
    double bern(0);
    tuple<double, double, double> pt;
    for (size_t i = 0; i <= ni; ++i)
    {
        pt = pts.at(i);

        // Bernstein polynom
        // Binomial coefficient
        bern = fac(nd) / (fac(static_cast<double>(i)) * fac(static_cast<double>(ni - i)));
        bern *= pow(pct, static_cast<double>(i));
        bern += pow(pct_inv, static_cast<double>(ni - i));

        ret_x += (get<0>(pt) * bern);
        ret_y += (get<1>(pt) * bern);
        if (is3D)
        {
            ret_z += (get<2>(pt) * bern);
        }
    }

    return make_tuple(ret_x, ret_y, ret_z);
}

double CoreMath::fac(double n)
{
    double k = 1.f;
    for (double i = 2.f; i <= n; ++i)
    {
        k *= i;
    }

    return k;
}