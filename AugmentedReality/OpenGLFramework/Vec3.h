#ifndef _VEC3_H_
#define _VEC3_H_

#include <cmath>
#include <iostream>
 
class Vec3{
private:
    float elements[3];
public:
    Vec3(float x = 0, float y = 0, float z = 0);
    ~Vec3();
    Vec3 copy();
 
 
    Vec3& set(float x, float y, float z);
    Vec3& setX(float x);
    Vec3& setY(float y);
    Vec3& setZ(float z);

	float getX();
	float getY();
	float getZ();
 
    Vec3 add(const Vec3& v1, int scale=1);
    Vec3 substract(const Vec3& v1, int scale=1);
    Vec3 scale(int scale);
 
    float dot(const Vec3& v2);
    Vec3 cross(const Vec3& v2);
 
    Vec3& normalize();
    float length();
    float lengthSquared();
 
    bool equals(const Vec3& v2, float epsilon);
    float operator[](int pos) const;
 
    Vec3 operator+(const Vec3& v2);
    Vec3 operator-(const Vec3& v2);
 
    Vec3& operator+=(const Vec3& v2);
    Vec3& operator-=(const Vec3& v2);
};

#endif