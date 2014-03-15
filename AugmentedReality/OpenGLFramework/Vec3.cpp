#include "Vec3.h"

 
Vec3::Vec3(float x, float y, float z){
    this->elements[0] = x;
    this->elements[1] = y;
    this->elements[2] = z;
    //std::cout << "Created Vec3 {" << this->elements[0] << ", " << this->elements[1] << ", " << this->elements[2] << "}\n";
}
 
Vec3::~Vec3(){
    //std::cout << "Destroyed Vec3 {" << this->elements[0] << ", " << this->elements[1] << ", " << this->elements[2] << "}\n";
}
 
Vec3 Vec3::copy(){
        Vec3 copy(
                this->elements[0],
                this->elements[1],     
                this->elements[2]
                );
        return copy;           
}
 
bool Vec3::equals(const Vec3& v2, float epsilon){
        return ((std::abs(this->elements[0] - v2[0]) < epsilon) &&
                    (std::abs(this->elements[1] - v2[1]) < epsilon) &&
                        (std::abs(this->elements[2] - v2[2]) < epsilon));      
}
 
 
float Vec3::length(){
        return std::sqrt(this->lengthSquared());
}
 
float Vec3::lengthSquared(){
        return (
                this->elements[0]*this->elements[0] +
                this->elements[1]*this->elements[1] +
                this->elements[2]*this->elements[2]
                );
}
 
Vec3& Vec3::normalize(){
        float mag = this->length();
        if(mag){
                float multiplier = 1.0f/mag;
                this->elements[0] *= multiplier;
                this->elements[1] *= multiplier;
                this->elements[2] *= multiplier;
        }
        return *this;
}
 
Vec3 Vec3::cross(const Vec3& v2){
        Vec3 cross(
                (this->elements[1] * v2[2] - this->elements[2] * v2[1]),
                (this->elements[2] * v2[0] - this->elements[0] * v2[2]),
                (this->elements[0] * v2[1] - this->elements[1] * v2[0])
        );
        return cross;
}
 
Vec3 Vec3::substract(const Vec3& v1, int scale){
        Vec3 sub(
                (this->elements[0] - v1[0]*scale),
                (this->elements[1] - v1[1]*scale),
                (this->elements[2] - v1[2]*scale)
                );
        return sub;
}
 
Vec3& Vec3::set(float x, float y, float z){
        this->elements[0] = x;
        this->elements[1] = y;
        this->elements[2] = z;
        return *this;
}
 
Vec3& Vec3::setX(float x){
        this->elements[0] = x;
        return *this;
}
 
Vec3& Vec3::setY(float y){
        this->elements[1] = y;
        return *this;
}
 
Vec3& Vec3::setZ(float z){
        this->elements[2] = z;
        return *this;
}

float Vec3::getX(){
       return this->elements[0];
}
 
float Vec3::getY(){
       return this->elements[1];
}
 
float Vec3::getZ(){
      return this->elements[2];
}
 
float Vec3::dot(const Vec3& v2){
        return (this->elements[0]*v2[0] +
                this->elements[1]*v2[1] +
                this->elements[2]*v2[2]
                );
}
 
Vec3 Vec3::scale(int scale){
        Vec3 scaled(
                (this->elements[0]*scale),
                (this->elements[1]*scale),
                (this->elements[2]*scale)
                );
        return scaled;
}
 
Vec3 Vec3::add(const Vec3& v1, int scale){
        Vec3 sum(
                (this->elements[0] + v1[0]*scale),
                (this->elements[1] + v1[1]*scale),
                (this->elements[2] + v1[2]*scale)
                );
        return sum;
}
 
 
float Vec3::operator[](int pos) const{
    return elements[pos];
}
 
Vec3 Vec3::operator+(const Vec3& v2) {
    return this->add(v2);
}
 
Vec3 Vec3::operator-(const Vec3& v2) {
    return this->substract(v2);
}
 
Vec3& Vec3::operator+=(const Vec3& v2) {
                this->elements[0] += v2[0];
                this->elements[1] += v2[1];
                this->elements[2] += v2[2];
        return *this;
}
 
Vec3& Vec3::operator-=(const Vec3& v2) {
                this->elements[0] -= v2[0];
                this->elements[1] -= v2[1];
                this->elements[2] -= v2[2];
        return *this;
}