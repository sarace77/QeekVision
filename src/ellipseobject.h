#ifndef ELLIPSEOBJECT_H
#define ELLIPSEOBJECT_H

#include <opencv2/core/core.hpp>

using namespace cv;

class EllipseObject
{

private:
    Point _center;
    double _rX, _rY;
public:
    EllipseObject();

    Point getCenter();
    float getDistanceFromPoint(Point pnt);
    float getEccentricity();
    double getHRadius();
    double getVRadius();
    bool isInternal(Point pnt);

    void setCenter(Point pnt);
    void setHRadius(double radius);
    void setVRadius(double radius);
};

#endif // ELLIPSEOBJECT_H
