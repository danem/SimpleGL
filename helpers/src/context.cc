#include <SimpleGL/helpers/context.h>
#include <SimpleGL/utils.h>
#include <stdexcept>
#include <iostream>


using namespace sgl;

#define GLBOOL(v) ((v) ? GL_TRUE : GL_FALSE)
#define BITSET_(v,x) ((v & x) == x)

static int getGLProfile (GLProfile profile) {
    if (profile == GLProfile::CORE) return GLFW_OPENGL_CORE_PROFILE;
    else if (profile == GLProfile::COMPAT) return GLFW_OPENGL_COMPAT_PROFILE;
    else return GLFW_OPENGL_ANY_PROFILE;
}

static void __handleMouseMove (GLFWwindow* window, double xp, double yp){
    detail::UserState * state = (detail::UserState*) glfwGetWindowUserPointer(window);
    state->xpos = xp;
    state->ypos = yp;
    for (auto& handler : state->mouseHandlers) {
        handler({MOVE, xp, yp, MOUSE_BTN_NONE, state->isMouseOverWindow});
    }
}

static void __handleMouseInput (GLFWwindow* window, int btn, int action, int mods){
    detail::UserState * state = (detail::UserState*) glfwGetWindowUserPointer(window);
    int button;
    for (auto& handler : state->mouseHandlers) {
        handler({action, state->xpos, state->ypos, btn, state->isMouseOverWindow});
    }
}

static void __handleMouseEnter (GLFWwindow* window, int entered){
    detail::UserState * state = (detail::UserState*) glfwGetWindowUserPointer(window);
    state->isMouseOverWindow = entered == 1;
    for (auto& handler : state->mouseHandlers) {
        handler({MOUSE_OVER_CHANGE, 0, 0, MOUSE_BTN_NONE, entered == 1});
    }
}

static void __handleMouseScroll (GLFWwindow* window, double xoff, double yoff) {
    detail::UserState * state = (detail::UserState*) glfwGetWindowUserPointer(window);
    for (auto& handler : state->mouseHandlers) {
        handler({SCROLL, xoff, yoff, MOUSE_BTN_NONE, state->isMouseOverWindow});
    }
}

static void __handleKeyEvent (GLFWwindow* window, int key, int scancode, int action, int mods){ 
    detail::UserState * state = (detail::UserState*) glfwGetWindowUserPointer(window);
    for (auto& handler : state->keyHandlers) {
        handler({action, key, BITSET_(mods,GLFW_MOD_SHIFT), false, false});
    }
}


void detail::getDefaultWindowConfig(detail::ContextConfig& config, int width, int height, const std::string& title, int major, int minor) {
    config.glVersionMajor = major;
    config.glVersionMinor = minor;
    config.glProfile = GLProfile::CORE;
    config.resizable = false;
    config.decorated = true;
    config.onTop = false;
    config.doubleBuffer = true;
    config.redBits = 8;
    config.greenBits = 8;
    config.blueBits = 8;
    config.alphaBits = 8;
    config.depthBits = 24;
    config.stencilBits = 8;
    config.forwardCompat = true;
    config.windowVisible = true;
    config.width = width;
    config.height = height;
    config.title = title;
    config.debug = false;
}

void Context::initialize () {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, attrs.glVersionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, attrs.glVersionMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, getGLProfile(attrs.glProfile));
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLBOOL(attrs.forwardCompat));
    glfwWindowHint(GLFW_RED_BITS, attrs.redBits);
    glfwWindowHint(GLFW_BLUE_BITS, attrs.blueBits);
    glfwWindowHint(GLFW_GREEN_BITS, attrs.greenBits);
    glfwWindowHint(GLFW_ALPHA_BITS, attrs.alphaBits);
    glfwWindowHint(GLFW_DEPTH_BITS, attrs.depthBits);
    glfwWindowHint(GLFW_STENCIL_BITS, attrs.stencilBits);
    glfwWindowHint(GLFW_DECORATED, GLBOOL(attrs.decorated));
    glfwWindowHint(GLFW_RESIZABLE, GLBOOL(attrs.resizable));
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLBOOL(attrs.doubleBuffer));
    glfwWindowHint(GLFW_FLOATING, GLBOOL(attrs.onTop));
    glfwWindowHint(GLFW_VISIBLE, GLBOOL(attrs.windowVisible));
    //glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLBOOL(attrs.debug));

    _windowState = glfwCreateWindow(attrs.width, attrs.height, attrs.title.c_str(), nullptr, nullptr);    
    if (_windowState == nullptr) throw std::runtime_error("Failed to create GLFW window");

    glfwMakeContextCurrent(_windowState);


    //glewExperimental = GL_TRUE;
    //if (glewInit() != GLEW_OK) throw std::runtime_error("Failed to initialize GLEW");

    glfwSetWindowUserPointer(_windowState, (void*)&_userState);

    glfwSetCursorPosCallback(_windowState, __handleMouseMove);
    glfwSetMouseButtonCallback(_windowState,__handleMouseInput);
    glfwSetCursorEnterCallback(_windowState,__handleMouseEnter);
    glfwSetScrollCallback(_windowState, __handleMouseScroll);
    glfwSetKeyCallback(_windowState, __handleKeyEvent);

    sgl::sglInitialize(attrs.glVersionMajor, attrs.glVersionMinor);
    sglClearGLError();

}

void Context::destroy () {
    glfwDestroyWindow(_windowState);
    glfwTerminate();
}

void Context::pollEvents () {
    glfwPollEvents();
}

void Context::setTitle (const std::string& title) {
    attrs.title = title;
    glfwSetWindowTitle(_windowState, title.c_str());
}

void Context::setWindowVisible (bool visible) {
    attrs.windowVisible = visible;
    if (visible) glfwShowWindow(_windowState);
    else glfwHideWindow(_windowState);
}

void Context::addKeyHandler (sgl::KeyHandler handler) {
    _userState.keyHandlers.emplace_back(handler);
}

/*
void Context::addKeyHandler (sgl::KeyHandler&& handler) {
    _userState.keyHandlers.emplace_back(handler);
}
*/

void Context::addMouseHandler (sgl::MouseHandler handler) {
    _userState.mouseHandlers.emplace_back(handler);
}

/*
void Context::addMouseHandler (sgl::MouseHandler&& handler) {
    _userState.mouseHandlers.emplace_back(handler);
}
*/

void Context::swapBuffers () {
    glfwSwapBuffers(_windowState);
}

bool Context::isAlive () {
    return !glfwWindowShouldClose(_windowState);
}

void Context::close () {
    glfwSetWindowShouldClose(_windowState, true);
}


