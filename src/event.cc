#include <SimpleGL/event.h>

using namespace sgl;


void MouseDragger::operator() (const MouseEvent& event) {
    if (!event.isOverWindow) return;
    if (event.usesButton(_dragButton)) {
        if (event.eventType == MOUSE_PRESS) {
            _isDragging = true;
            onDragStart();
        } else if (event.eventType == MOUSE_MOVE && _isDragging) {
            if (_mx == -1 || _my == -1) {
                _ox = _mx = event.x;
                _oy = _my = event.y;
                return;
            }
            double dx = event.x - _mx;
            double dy = event.y - _my;
            _mx = event.x;
            _my = event.y;
            onDrag(_ox, _oy, _mx, _my, dx, dy);
        } else {
            onDragEnd();
            _mx = _my = _ox = _oy = -1;
            _isDragging = false;
        }
    }
}
