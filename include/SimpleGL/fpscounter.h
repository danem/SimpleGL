#pragma once

namespace sgl {


class FPSCounter {
public:
    using fps_updater = std::function<void (float)>;

private:
    double _lastTime;
    int _frameCount;
    fps_updater _updateFn;

public:
    FPSCounter (fps_updater updater) :
        _updateFn(updater)
    {}

    void start ();
    void tick ();
};

} // end namespace
