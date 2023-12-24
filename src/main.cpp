#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

constexpr auto winHeight{ 480 };
constexpr auto winWidth{ 640 };
constexpr auto winTitle{ "OpenGL Window" };

void ErrorCallback(int error, const char* msg)
{
    std::cerr << "GLUT error " << error << ": " << msg << std::endl;
}

ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

    std::string line{};
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            } else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        } else {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[(int)ShaderType::VERTEX].str(),
             ss[(int)ShaderType::FRAGMENT].str() };
}

unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); // iv: interger vector
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(*message));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile: "
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << "shader." << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

unsigned int CreateShader(const std::string& vertexShader,
                          const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    assert(vs != 0);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    assert(vs != 0);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

} // namespace

int main(void)
{
    GLFWwindow* window = nullptr;

    if (!glfwInit())
        return GLFW_NOT_INITIALIZED;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwSetErrorCallback(ErrorCallback);

    window = glfwCreateWindow(winWidth, winHeight, winTitle, nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error initializing GLEW\n";
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    // clang-format off
    float positions[] = {
        -0.5f, -0.5f, // 0
         0.5f, -0.5f, // 1
         0.5f,  0.5f, // 2
        -0.5f,  0.5f, // 3
    };

    // Index of position of vertices
    unsigned int indicies[] = {
        0, 1, 2,
        2, 3, 0,
    };
    // clang-format on

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions,
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    // Using index buffers to avoid duplicate vertices
    // (using vertices more than once)
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indicies,
                 GL_STATIC_DRAW);

    ShaderProgramSource source = ParseShader("res/shaders/Basic.glsl");
    unsigned int shader
        = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // This will use vertices more than once (high memory usage)
        // glDrawElements(GL_TRIANGLES, 0, 6);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        // - 6: number of indicies
        // - nullptr: the buffer that is bound to GL_ELEMENT_ARRAY_BUFFER

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &buffer);
    glDeleteProgram(shader);

    glfwTerminate();

    return GLFW_NO_ERROR;
}
