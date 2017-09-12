#ifndef WINDOWPROVIDER_H
#define WINDOWPROVIDER_H

#include <stdint.h>
#include <GLFW/glfw3.h>

#include "sglconfig.h"


namespace sgl {


namespace detail {


    WindowConfig getDefaultWindowConfig (WindowAPI api, WindowConfig& dest);





} // namespace
} // namespace
#endif // WINDOWPROVIDER_H
