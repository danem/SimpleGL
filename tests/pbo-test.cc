#include <SimpleGL/SimpleGL.h>

int main () {
    sgl::Context ctx{500,500,"pbo test"};
    sglClearGLError();

    sgl::Framebuffer fbo;

    sgl::Texture2D tex = sgl::Texture2DBuilder()
        .build(ctx.attrs.width, ctx.attrs.height);
     
     fbo.attachTexture(tex);
}
