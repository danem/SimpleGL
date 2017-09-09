#pragma once
#ifdef SGL_USE_GLM

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace sgl {

struct Transform {
    glm::mat4 transform;
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;

    Transform () :
        position(),
        scale(1,1,1),
        rotation(0,0,0),
        transform(1)
    {}

    Transform (glm::mat4 mat);

    float * getTransform();
    void update ();
    void fromMatrix (glm::mat4 mat);
    glm::mat4 operator* (glm::mat4 other);
};

} // end namespace

#endif
