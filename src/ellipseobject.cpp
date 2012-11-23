#include "ellipseobject.h"

EllipseObject::EllipseObject() {
    _center = Point(0,0);
    _rX = 0;
    _rY = 0;
}

Point EllipseObject::getCenter() {
    return _center;
}

float EllipseObject::getDistanceFromPoint(Point pnt) {
    float res = 0.0;
    if ((_rX * _rY) != 0)
        res = sqrt((_center.x - pnt.x)*(_center.x - pnt.x) + (_center.y - pnt.y)*(_center.y - pnt.y));
    return res;
}

float EllipseObject::getEccentricity() {
    float res = 0.0;
    if ((_rX * _rY) != 0)
        res = _rX / _rY;
    return res;
}

double EllipseObject::getHRadius() {
    return _rX;
}

double EllipseObject::getVRadius() {
    return _rY;
}

bool EllipseObject::isInternal(Point pnt) {
    return (abs(pnt.x - _center.x) < _rX) &&
            (abs(pnt.y - _center.y) < _rY) &&
            ((getDistanceFromPoint(pnt)*getDistanceFromPoint(pnt)) < _rX * _rY);
}

void EllipseObject::setCenter(Point pnt) {
    _center = pnt;
}

void EllipseObject::setHRadius(double radius) {
    _rX = radius;
}

void EllipseObject::setVRadius(double radius) {
    _rY = radius;
}
