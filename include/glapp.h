
#include <memory>
#include "imguiManager.h"
#include "stealpool.h"
/*#include <glad/glad.h>*/
#include <shader.h>

struct DestroyglfwWin {

  void operator()(GLFWwindow *ptr) { glfwDestroyWindow(ptr); }
};

class glapp {
private:
  glapp() = default;
  ~glapp() {
    imgui->cleanup();
    glfwTerminate();
    deleteShader();
  }
  glapp(const glapp &) = delete;
  glapp &operator=(const glapp &) = delete;

  std::unique_ptr<GLFWwindow, DestroyglfwWin> windowApp;
  std::unique_ptr<imguiManager> imgui;

  static void framebuffer_size_callback(GLFWwindow *window, int width,
                                        int height);

  unsigned int VertexBuffer;
  unsigned int ElementBuffer;
  unsigned int VertexArrayObject;

  std::unique_ptr<Shader> shaders;
  unsigned int vertexShader;
  unsigned int fragmentShader;
  unsigned int shaderProgram;

  void initVertexBuffer();
  void initElementBuffer();
  void initShaders();
  void initFragmentShader();
  void initVertexShader();
  void initShaderProgram();
  void deleteShader();

public:
  static std::unique_ptr<stealpool> tpool;

  static glapp &getInst() {
    static glapp instance;
    return instance;
  }
  void processInput();
  void initWindow();
  void initContext();
  void glAppStart();
  void initOpenGL();
  void initBuffers();
  imguiManager *getImgui();
  GLFWwindow *getpWindow();
};
