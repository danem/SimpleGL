#include <SimpleGL/helpers/SimpleGLHelpers.h>
#include <string>
#include <iostream>

const char* badVs =
        "#version 330 core\n"
        "void main() {\n"
        "    gl_Position = foo;\n"
        "}\n";

const char * badFs =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main () {\n"
        "    FragColor = vec4(1,0,0,1);\n"
        "}\n";



int main () {
    sgl::Context ctx{500,500,"shader test"};
    sgl::Shader shader;
    try {
        sgl::compileShader(shader, badVs, badFs);
    } catch (const std::runtime_error& err) {
        std::cout << err.what() << std::endl;
    }

    std::cout << "caught error\n";
}
