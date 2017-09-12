#ifdef SGL_USE_GLM

#include <SimpleGL/camera.h>
#include "glm/ext.hpp"

using namespace sgl;

void CameraBase::updateViewMatrix () {
    viewMatrix = glm::lookAt(position, looking, up);
}

void CameraBase::update () {
    updateViewMatrix();
}

float * CameraBase::getView () {
    return glm::value_ptr(viewMatrix);
}

void PerspectiveCamera::updateProjection () {
    aspect = width / height;
    float _fov = glm::degrees(2.f * (float)glm::atan((glm::tan(glm::radians(fov * 0.5f)) / zoom)));
    perspectiveMatrix = glm::perspective(_fov, aspect, near, far);
}

void PerspectiveCamera::update () {
    CameraBase::update();
    updateProjection();
}

float * PerspectiveCamera::getProjection () {
    return glm::value_ptr(perspectiveMatrix);
}

void OrthoCamera::updateProjection () {
    perspectiveMatrix = glm::ortho(left,right,bottom,top,near,far);
}

void OrthoCamera::update () {
    CameraBase::update();
    updateProjection();
}

void OrthoCamera::setFrustrum (float left, float right, float top, float bottom, float near, float far) {
    this->left   = left;
    this->right  = right;
    this->top    = top;
    this->bottom = bottom;
    this->near   = near;
    this->far    = far;
}

float * OrthoCamera::getProjection () {
    return glm::value_ptr(perspectiveMatrix);
}

#endif
