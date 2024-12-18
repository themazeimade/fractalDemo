
struct rectangle {
  static constexpr float vertices[12] = {
      0.5f,  0.5f,  0.0f, // top right
      0.5f,  -0.5f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f  // top left
  };
  static constexpr unsigned int indices[6] = {
      // note that we start from 0!
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };
};

struct rxctangle {
  static constexpr float vertices[24] = {
      // positions         // colors
      0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, // bottom right
      0.5f,  -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom left
      -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, // top
      -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 1.0f  // top
  };
  static constexpr unsigned int indices[6] = {
      // note that we start from 0!
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };
};
struct Fill_Display {
  static constexpr float vertices[20] = {
      // positions         // colors
      -1.f, -1.f, 0.0f, 0.0f, 0.0f, // bottom left
      1.f,  -1.f, 0.0f, 1.0f, 0.0f, // bottom right
      1.f,  1.f,  0.0f, 1.0f, 1.0f, // top right
      -1.f, 1.f,  0.0f, 0.0f, 1.0f, // top left
  };
  static constexpr unsigned int indices[6] = {
      // note that we start from 0!
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };
};

struct triangle {
  static constexpr float vertices[9] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f,
                                        0.0f,  0.0f,  0.5f, 0.0f};
  static constexpr unsigned int indices[6] = {
      // note that we start from 0!
      0, 1, 3, // first triangle
  };
};
