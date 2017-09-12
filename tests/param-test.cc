#include <SimpleGL/SimpleGL.h>
#include <iostream>

#include <glm/vec3.hpp> 


void printVec3 (const char * name, const glm::vec3& vec) {
    std::cout << name << ": " << vec.x << ", " << vec.y << ", " << vec.z << std::endl;
}

void addX (bool sign, glm::vec3& vec, float v) {
    if (sign) vec.x += v;
    else vec.x -= v;
}

int main () {
    // No name. Never prints
    sgl::Param<float> p;
    p.increment();
    p.decrement();

    // Prints with each change
    sgl::Param<float> q("q");
    q.increment();
    q.decrement();

    // Modify references
    float z = 1;
    sgl::Param<float&> zp("zp",z,1);
    zp.increment();
    zp.increment();
    std::cout << "z: " << z << std::endl;

    // Different value and step types. Custom printer
    sgl::Param<glm::vec3,float> gp("vec", 1, printVec3);
    gp.increment();
    gp.decrement();

    // Custom incrementer
    sgl::Param<glm::vec3,float> gp2("vecx", 1, printVec3, addX);
    gp2.increment();
    gp2.increment();
}
