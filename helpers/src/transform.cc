#include <SimpleGL/helpers/transform.h>


using namespace sgl;


Transform::Transform (glm::mat4 mat){ 
    fromMatrix(mat);
}

void Transform::update () {
    glm::mat4 r   = glm::mat4_cast(glm::toQuat(glm::orientate3(rotation)));
    glm::mat4 s   = glm::scale(this->scale);
    glm::mat4 t   = glm::translate(this->position);
    transform     = t * r * s;
}

float * Transform::getTransform () {
    update();
    return glm::value_ptr(transform);
}

void Transform::fromMatrix (glm::mat4 mat) {
    glm::vec3 skew;
    glm::vec4 persp;
    glm::quat rot;
    glm::decompose(mat, scale, rot, position, skew, persp);
    rotation = glm::eulerAngles(rot);
}

glm::mat4 Transform::operator* (glm::mat4 other) {
    return transform * other;
}
