#define SGL_DEBUG 1
#include <SimpleGL/SimpleGL.h>

int main () {
    sgl::Context ctx{500, 500, "Managed resource test"};
    {
        sgl::MArrayBuffer<int> foo;
    }
}


