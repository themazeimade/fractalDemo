
#include "config.h"
#include <array>
#include <cstdint>

struct color {
  unsigned char R;
  unsigned char G;
  unsigned char B;

  color(unsigned R_ = 0, unsigned G_ = 0, unsigned B_ = 0) {
    R = R_;
    G = G_;
    B = B_;
  }
};

struct position {
  uint32_t x;
  uint32_t y;

  position(uint32_t x_, uint32_t y_) : x(x_), y(y_) {}
};

struct vertexGrid {
  color *grid;
  position size;

  vertexGrid(unsigned width, unsigned height) : size{width, height} {
    grid = new color[width * height];
  };
  void setColor(uint32_t x, uint32_t y, color c_) {
    const uint32_t index = x * size.y + y;
    grid[index] = c_;
  }
  const unsigned char* getData() {
    int nElements = size.x * size.y; 
    int totalBytes = nElements * 3;
    unsigned char* data = new unsigned char[totalBytes] ;
    for (int i=0;i<nElements;i++) {
      data[i*3] = grid[i].R;
      data[i*3+1] = grid[i].G;
      data[i*3+2] = grid[i].B;
    }
    return data;
  }
};

namespace fractal {

template <typename Tfloat> float julia(position pos) {
  unsigned i{0};
  Tfloat zr{pos.x};
  Tfloat zi{pos.y};
  auto mod = zi * zi + zi * zi;
  while (mod < Tfloat{4.0} && i < config::maxIteration) {
    const Tfloat temp = zr;
    zr = zr * zr - zi * zi + config::constant_r;
    zi = static_cast<Tfloat>(2.0) * zi * temp + config::constant_i;
    mod = zr * zr + zi * zi;
    i++;
  }
  return i;
}

}; // namespace fractal
