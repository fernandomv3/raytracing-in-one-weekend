#include <vec3.h>

inline vec3& vec3::operator+=(const vec3& v2){
  this->e[0] += v2.e[0];
  this->e[1] += v2.e[1];
  this->e[2] += v2.e[2];
  return *this;
}

inline vec3& vec3::operator-=(const vec3& v2){
  this->e[0] -= v2.e[0];
  this->e[1] -= v2.e[1];
  this->e[2] -= v2.e[2];
  return *this;
}

inline vec3& vec3::operator*=(const vec3& v2){
  this->e[0] *= v2.e[0];
  this->e[1] *= v2.e[1];
  this->e[2] *= v2.e[2];
  return *this;
}

inline vec3& vec3::operator/=(const vec3& v2){
  this->e[0] /= v2.e[0];
  this->e[1] /= v2.e[1];
  this->e[2] /= v2.e[2];
  return *this;
}

inline vec3& vec3::operator*=(const float t){
  this->e[0] *= t;
  this->e[1] *= t;
  this->e[2] *= t;
  return *this;
}

inline vec3& vec3::operator/=(const float t){
  this->e[0] /= t;
  this->e[1] /= t;
  this->e[2] /= t;
  return *this;
}

inline vec3 operator+(const vec3& v1, const vec3& v2){
  return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

inline vec3 operator-(const vec3& v1, const vec3& v2){
  return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

inline vec3 operator*(const vec3& v1, const vec3& v2){
  return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

inline vec3 operator/(const vec3& v1, const vec3& v2){
  return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

inline float dot(const vec3& v1, const vec3& v2){
  return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

inline vec3 cross(const vec3& v1, const vec3& v2){
  return vec3((v1.e[1]*v2.e[2] - v1.e[2]*v2.e[1]),
             -(v1.e[0]*v2.e[2] - v1.e[2]*v2.e[0]),
              (v1.e[0]*v2.e[1] - v1.e[1]*v2.e[0]) 
            );
}

inline vec3 unit_vector(vec3 v){
  return v / v.length();
}
