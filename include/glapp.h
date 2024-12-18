#pragma once
#include "renderer.h"
#include "imguiManager.h"
#include <memory>
/*#include "stealpool.h"*/
/*#include <glad/glad.h>*/
/*#include <shader.h>*/

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
  std::unique_ptr<renderer<config::Tfloat>> FractalRenderer;
  /*renderer<config::Tfloat> Renderer;*/

  static void framebuffer_size_callback(GLFWwindow *window, int width,
                                        int height);
  static void keyboard_callback(GLFWwindow *window, int key, int scancode,
                                int action, int mods);

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
  /*static std::unique_ptr<stealpool> tpool;*/

  // input things
  config::Tfloat zoom_factor = static_cast<config::Tfloat>(1.005);
  const config::Tfloat speed = static_cast<config::Tfloat>(1.0);
  config::Tfloat zoom = static_cast<config::Tfloat>(HEIGHT/2.0);
  std::array<config::Tfloat, 2> center = {0.0, 0.0};

  bool zoom_in = false;
  bool zoom_out = false;
  bool left = false;
  bool right = false;
  bool up = false;
  bool down = false;

  // constructor abstraction

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
