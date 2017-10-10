#include <SimpleGL/utils.h>
#include <SimpleGL/helpers/camera.h>
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

void CameraController::onDragStart() {
    _originalPos = _camera.position;
    _originalLook = _camera.looking;
}

void CameraController::onDragEnd() {}

void CameraController::onDrag (const sgl::DragEvent& event) {
    double _dx = event.mx - event.ox;
    double _dy = event.my - event.oy;

    float ax = (float)(_dx / _width);
    float ay = (float)(_dy / _height);

    //printf("cam: %p\n", &_camera);
    glm::quat qa = glm::angleAxis(ax*2.0f, glm::vec3(0.0f,1.0f,0.0f));
    glm::quat qb = glm::angleAxis(ay*2.0f, glm::vec3(1.0f,0.0f,0.0f));
    glm::quat rotation = qa * qb;
    glm::mat4 rot    = glm::mat4_cast(rotation);
    glm::vec4 temp   = glm::vec4(_originalPos,1.0f);
    glm::vec4 newPos = rot * temp;
    glm::vec3 res(newPos);
    _camera.position = res;
    _camera.looking = _originalLook;
    _camera.update();
}

void CameraController::onScroll(double sx, double sy) {
    glm::vec3 look = _camera.looking - _camera.position;
    _camera.position += look * sy;
    _camera.update();
}

