#pragma once
#include "stealpool.h"
#include <array>
#include <cstdint>
#include <cstdio>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <vector>

struct color {
  uint8_t r, g, b, a;
  color(uint8_t r = 255, uint8_t g = 255, uint8_t b = 255, uint8_t a = 255)
      : r(r), g(g), b(b), a(a) {}
};

struct position {
  uint32_t x;
  uint32_t y;

  position() {}
  position(uint32_t x_, uint32_t y_) : x(x_), y(y_) {}
};

struct vertexGrid {
  std::vector<color> grid;
  std::vector<unsigned char> data;
  std::array<uint32_t, 2> size;

  /*vertexGrid() : grid(nullptr), data(nullptr), size{1000, 1000} {*/
  /*}*/
  vertexGrid(unsigned width, unsigned height) : size{width, height} {
    grid = std::vector<color>(width * height, color());
    data = std::vector<unsigned char>(width * height * 4, 0);
  };

  ~vertexGrid() {}

  void setColor(uint32_t x, uint32_t y, color c_) {
    uint32_t index = y * size[0] + x;
    grid[index] = c_;
  }

  unsigned char *getData() {
    int nElements = size[0] * size[1];
    const unsigned nThreads = Tpool.getThreadCount(); 
    uint32_t slice_elements = (nElements / nThreads);
    for (unsigned i = 0; i < nThreads; i++) {
      Tpool.submit([=] {
        for (unsigned int k = (i * slice_elements);
             k < (i + 1) * slice_elements; k++) {
          data[k * 4] = static_cast<unsigned char>(grid[k].r);
          data[k * 4 + 1] = static_cast<unsigned char>(grid[k].g);
          data[k * 4 + 2] = static_cast<unsigned char>(grid[k].b);
          data[k * 4 + 3] = static_cast<unsigned char>(grid[k].a);
        }
      });
    }
    while (not Tpool.isDone()) {
      Tpool.run_pending_task();
    }
    /*for (int i = 0; i < nElements; i++) {*/
    /*  data[i * 4] = static_cast<unsigned char>(grid[i].r);*/
    /*  data[i * 4 + 1] = static_cast<unsigned char>(grid[i].g);*/
    /*  data[i * 4 + 2] = static_cast<unsigned char>(grid[i].b);*/
    /*  data[i * 4 + 3] = static_cast<unsigned char>(grid[i].a);*/
    /*}*/
    return data.data();
  }
};

struct palette {

  struct pcolor {
    glm::vec4 rgba;
    float value;
  };
  std::vector<pcolor> colors;
  void addColor(float value_, color color_) {
    pcolor a = {{static_cast<float>(color_.r), static_cast<float>(color_.g),
                 static_cast<float>(color_.b), static_cast<float>(color_.a)},
                value_};
    colors.push_back(a);
  }
  glm::vec4 getVecColorByValue(float value) {
    if (value >= 1.0f) {
      return colors.back().rgba;
    }
    if (value <= 0.0f) {
      return colors.front().rgba;
    }
    int i = 0;
    for (const pcolor &b : colors) {
      if (b.value > value) {
        const float range = b.value - colors[i - 1].value;
        const float pos = value - colors[i - 1].value;
        const float ratio = pos / range;
        return (1.0f - ratio) * colors[i - 1].rgba + ratio * b.rgba;
      }
      i++;
    }
    return {255.f, 255.f, 255.f, 255.f};
  };

  color getColor(float value) {
    auto a = getVecColorByValue(value);
    color temp = color();
    temp.r = std::min(std::max(0.0f, a.r), 255.f);
    temp.g = std::min(std::max(0.0f, a.g), 255.f);
    temp.b = std::min(std::max(0.0f, a.b), 255.f);
    temp.a = std::min(std::max(0.0f, a.a), 255.f);
    return temp;
  }

  static color vec3toColor(glm::vec3 color_) {
    color temp = color();
    temp.r = static_cast<uint8_t>(color_.r);
    temp.g = static_cast<uint8_t>(color_.g);
    temp.b = static_cast<uint8_t>(color_.b);
    temp.a = 255;
    return temp;
  }
};
