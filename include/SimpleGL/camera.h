#pragma once
#ifdef SGL_USE_GLM

#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp> 

namespace sgl {

class CameraBase {
public:

    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 looking;
    glm::mat4 viewMatrix;

    CameraBase () :
        up(0.f, 1.f, 0.f)
    {
        update();
    }

    void updateViewMatrix ();
    virtual void update ();
    float * getView ();
};

class PerspectiveCamera : public CameraBase {
protected:
public:


    glm::mat4 perspectiveMatrix;

    float width;
    float height;
    float aspect;
    float near  = 0.1f;
    float far   = 1000;
    float zoom  = 1;
    float fov   = 45;
    float focus = 10;

    PerspectiveCamera (uint32_t w, uint32_t h) :
        CameraBase(),
        width(w), height(h), aspect((float)w/h)
    {
        update();
    }

    void updateProjection ();
    void update () override;
    float * getProjection ();

};

class OrthoCamera : public CameraBase {
protected:
public:


    glm::mat4 perspectiveMatrix;

    float near  = 1.0f;
    float far   = 7.5f;

    float left   = -10.0f;
    float right  = 10.0f;
    float top    = 10.0f;
    float bottom = -10.0f;

    OrthoCamera (float near, float far) :
        CameraBase(),
        near(near), far(far)
    {
        update();
    }

    void updateProjection ();
    void update () override;
    float * getProjection ();

    void setFrustrum (float left, float right, float top, float bottom, float near, float far);
    
};

} // end namespace

#endif
