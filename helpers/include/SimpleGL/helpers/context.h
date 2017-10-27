#pragma once

#include <SimpleGL/sglconfig.h>
#include "event.h"

#include <vector>
#include <string>

#define GLFW_STATIC
#include <GLFW/glfw3.h>


namespace sgl {

enum class GLProfile {
    CORE, COMPAT, ANY
};

namespace detail {

    struct UserState {
        bool isMouseOverWindow;
        double xpos, ypos;
        std::vector<sgl::KeyHandler> keyHandlers;
        std::vector<sgl::MouseHandler> mouseHandlers;
    };

    struct ContextConfig {
        std::string title;
        uint32_t width, height;
        uint32_t glVersionMajor, glVersionMinor;
        GLProfile glProfile;
        bool resizable;
        bool onTop;
        bool decorated;
        bool doubleBuffer;
        char redBits, blueBits, greenBits, alphaBits, depthBits, stencilBits;
        bool forwardCompat;
        bool windowVisible;
        bool debug;
        bool fullscreen;
        int monitor;
    };

    void getDefaultWindowConfig (ContextConfig& dest, int width = 0, int height = 0, const std::string& title = "", int major = SGL_OPENGL_MAX_MAJOR, int minor = SGL_OPENGL_MAX_MINOR);
} // namespace

class Context {
private:
    detail::UserState _userState;

    // TODO: This should support other windowing APIs
    GLFWwindow * _windowState;

    void initialize ();

public:
    detail::ContextConfig attrs;

    Context (int width, int height, const std::string& title = "", bool debug = false) {
        detail::getDefaultWindowConfig(attrs, width, height, title);
        attrs.debug = debug;
        initialize();
    }

    Context (detail::ContextConfig& config) :
        attrs(config)
    {
        initialize();
    }

    ~Context () {
        destroy();
    }

    void destroy ();

    void pollEvents ();
    void swapBuffers ();

    void setTitle (const std::string& title);
    void setWindowVisible (bool visible);

    void addKeyHandler (sgl::KeyHandler handler);
    //void addKeyHandler (sgl::KeyHandler&& handler);
    void removeKeyHandler (sgl::KeyHandler handler);

    void addMouseHandler (sgl::MouseHandler handler);
    //void addMouseHandler (sgl::MouseHandler&& handler);
    void removeMouseHandler (sgl::MouseHandler handler);

    bool isAlive ();
    void close ();

};



class ContextBuilder {
private:
    detail::ContextConfig _config;

public:

    ContextBuilder () {
        detail::getDefaultWindowConfig(_config);
    }

    ContextBuilder& setSize (uint32_t width, uint32_t height) {
        _config.width = width;
        _config.height = height;
        return *this;
    }

    ContextBuilder& setGLVersion (GLuint major, GLuint minor) {
        _config.glVersionMajor = major;
        _config.glVersionMinor = minor;
        return *this;
    }

    ContextBuilder& setResizable (bool resizable) {
        _config.resizable = resizable;
        return *this;
    }

    ContextBuilder& setForwardCompatible (bool enabled){
        _config.forwardCompat = enabled;
        return *this;
    }

    ContextBuilder& setTitle (const std::string& title) {
        _config.title = title;
        return *this;
    }

    ContextBuilder& setDecorated (bool enabled) {
        _config.decorated = enabled;
        return *this;
    }

    ContextBuilder& setOnTop (bool enabled){
        _config.onTop = enabled;
        return *this;
    }

    ContextBuilder& setDoubleBuffered (bool enabled) {
        _config.doubleBuffer = enabled;
        return *this;
    }

    ContextBuilder& setOpenGLProfile (GLProfile profile) {
        _config.glProfile = profile;
        return *this;
    }

    ContextBuilder& setFBOFormat (char rb, char gb, char bb, char ab, char db, char sb) {
        _config.redBits = rb;
        _config.greenBits = gb;
        _config.blueBits = bb;
        _config.alphaBits = ab;
        _config.depthBits = db;
        _config.stencilBits = sb;
        return *this;
    }

    ContextBuilder& setVisible (bool visible) {
        _config.windowVisible = visible;
        return *this;
    }

    ContextBuilder& setDebug (bool enabled){
        _config.debug = enabled;
        return *this;
    }

    ContextBuilder& setFullscreen (bool enabled) {
        _config.fullscreen = enabled;
        return *this;
    }

    Context build () {
        return {_config};
    }

};


} // namespace
