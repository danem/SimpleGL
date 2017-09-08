#pragma once

#include <functional>

namespace sgl {

const int MOUSE_MOVE        = 1 << 0;
const int MOUSE_PRESS       = 1 << 1;
const int MOUSE_RELEASE     = 1 << 2;
const int MOUSE_SCROLL      = 1 << 3;
const int MOUSE_OVER_CHANGE = 1 << 4;
const int KEY_PRESS         = 1 << 5;
const int KEY_RELEASE       = 1 << 6;
const int KEY_REPEAT        = 1 << 7;

const int MOUSE_BTN_NONE   = 1 << 0;
const int MOUSE_BTN_LEFT   = 1 << 1;
const int MOUSE_BTN_RIGHT  = 1 << 2;
const int MOUSE_BTN_MIDDLE = 1 << 3;

struct KeyEvent {
    int eventType;
    int key;
    bool shiftPressed;
    bool ctrlPressed;
    bool metaPressed;
};

struct MouseEvent {
    int eventType;
    double x;
    double y;
    int button;
    bool isOverWindow;

    bool usesButton (int btn) const;
};

using KeyHandler = std::function<void (const KeyEvent& event)>;
using MouseHandler = std::function<void (const MouseEvent& event)>;

class MouseController {
public:
    // MouseHandler
    virtual void operator()(const MouseEvent& event) = 0;
};
           

struct MouseDragState {
    double ox, oy;
    double mx, my;
    double dx, dy;
};

class MouseDragger : MouseController {
private:
    bool _isDragging;
    double _ox, _oy;
    double _mx, _my;
    int _dragButton;

public:
    MouseDragger (int dragButton = MOUSE_BTN_LEFT) :
        _dragButton(dragButton)
    {}

    // MouseHandler
    void operator() (const MouseEvent& event) override;

    virtual void onDragStart () = 0;
    virtual void onDrag (double ox, double oy, double mx, double my, double dx, double dy) = 0;
    virtual void onDragEnd () = 0;
};


}
