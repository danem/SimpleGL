#include <SimpleGL/texture.h>

#define STB_IMAGE_IMPLEMENTATION
#include <SimpleGL/contrib/stb_image.h>


unsigned char * sgl::detail::loadTexture2D (const char * path, int * width, int * height, int * channels) {
    unsigned char* data = stbi_load(path, width, height, channels, 0);
    return data;
}

void sgl::detail::freeTexture2D (unsigned char * data) {
    stbi_image_free(data);
}
