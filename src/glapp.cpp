#include <cmath>
#include <geometry.h>
#include <iostream>
#include <memory>
#include <glapp.h>

std::unique_ptr<stealpool> glapp::tpool = std::make_unique<stealpool>();

void glapp::initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  windowApp.reset(glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL));
  if (windowApp == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    return;
  }

  glfwMakeContextCurrent(windowApp.get());

  glfwSwapInterval(1);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
  }

  glViewport(0, 0, 800, 600);

  glfwSetFramebufferSizeCallback(windowApp.get(), framebuffer_size_callback);
}

void glapp::framebuffer_size_callback(GLFWwindow *window, int width,
                                      int height) {
  glViewport(0, 0, width, height);
}

void glapp::processInput() {
  if (glfwGetKey(windowApp.get(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(windowApp.get(), true);
}

void glapp::glAppStart() {
  while (!glfwWindowShouldClose(windowApp.get())) {

    processInput();
    imgui->set();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /*glUseProgram(shaderProgram);*/
    shaders->use();

    float timeValue = glfwGetTime();
    float cValue = std::sin(timeValue) / 2.0f + 0.5f;
    shaders->setFloat("ourValue", cValue);
    /*int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");*/
    /*glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);*/

    glBindVertexArray(VertexArrayObject);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    /*glDrawArrays(GL_TRIANGLES, 0, 3);*/

    imgui->draw();

    glfwSwapBuffers(windowApp.get());
    glfwPollEvents();
  }
}

void glapp::initContext() {
  initWindow();
  initOpenGL();
  imgui = std::make_unique<imguiManager>();
  imgui->init(windowApp.get());
}

void glapp::initBuffers() {
  initVertexBuffer();
  initElementBuffer();
}

void glapp::initVertexBuffer() {
  // VB and VAO
  glGenBuffers(1, &VertexBuffer);
  glGenVertexArrays(1, &VertexArrayObject);

  glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
  glBindVertexArray(VertexArrayObject);

  // Vertex Attributes

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // this test vertex (triangle)
  /*float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f,
   * 0.0f};*/

  glBufferData(GL_ARRAY_BUFFER, sizeof(rxctangle::vertices),
               rxctangle::vertices, GL_STATIC_DRAW);
}

void glapp::initElementBuffer() {
  // VB and VAO
  glGenBuffers(1, &ElementBuffer);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBuffer);

  // Vertex Attributes

  // this test vertex (triangle)

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rxctangle::indices),
               rxctangle::indices, GL_STATIC_DRAW);
}

void glapp::initVertexShader() {
  // vertexShader compilation

  const char *vertexShaderSource =
      "#version 330 core\n"
      "layout (location = 0) in vec3 aPos;\n"
      "void main()\n"
      "{\n"
      "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
      "}\0";

  vertexShader = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  // check shader success

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }
}

void glapp::initFragmentShader() {
  // fragment shader compilation

  const char *fragmentShaderSource =
      "#version 330 core\n"
      "out vec4 FragColor;\n"
      "uniform vec4 ourColor;\n"
      "void main()\n"
      "{\n"
      /*"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"*/
      "   FragColor = ourColor;\n"
      "}\0";

  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  // check fragment shader success
  int success;
  char infoLog[512];
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }
}

void glapp::initShaderProgram() {
  // shader Program instatiation

  shaderProgram = glCreateProgram();

  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  int success;
  char infoLog[512];
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  /*glUseProgram(shaderProgram);*/
}

void glapp::deleteShader() {
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

void glapp::initOpenGL() {
  initBuffers();
  shaders = std::make_unique<Shader>("../shaders/vert.glsl","../shaders/frag.glsl");
  /*initVertexShader();*/
  /*initFragmentShader();*/
  /*initShaderProgram();*/
}

imguiManager* glapp::getImgui() {
  return imgui.get();
}
