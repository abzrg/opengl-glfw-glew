#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include <iostream>
#include <string>

namespace {
constexpr auto winHeight{ 480 };
constexpr auto winWidth{ 640 };
constexpr auto winTitle{ "OpenGL Window" };

void ErrorCallback(int error, const char* msg)
{
    std::cerr << "GLUT error " << error << ": " << msg << std::endl;
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
        std::cout << "Failed to compile"
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                  << std::endl;
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

    float positions[6] = { -0.5f, -0.5f, 0.0f, 0.5f, 0.5f, -0.5f };

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    std::string vertexShader
        = "#version 330 core\n"
          "\n"
          "layout(location = 0) in vec4 position;\n"
          "\n"
          "void main()\n"
          "{\n"
          "   gl_Position = position;\n"
          "}\n";

    std::string fragmentShader
        = "#version 330 core\n"
          "\n"
          "layout(location = 0) out vec4 color;\n"
          "\n"
          "void main()\n"
          "{\n"
          "    color = vec4(1.0, 0.0, 0.0, 1.0);\n"
          "}\n";

    unsigned int shader = CreateShader(vertexShader, fragmentShader);
    glUseProgram(shader);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &buffer);
    glfwTerminate();

    return GLFW_NO_ERROR;
}
