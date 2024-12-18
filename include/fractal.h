#pragma once
#include "config.h"
#include <array>
#include <cstdint>


namespace fractal {

template <typename Tfloat> float julia(std::array<Tfloat,2> pos) {
  uint32_t i{0};
  Tfloat zr{pos[0]};
  Tfloat zi{pos[1]};
  Tfloat mod = zr * zr + zi * zi;
  while (mod < Tfloat{4.0} && i < config::maxIteration) {
    const Tfloat temp = zr;
    zr = zr * zr - zi * zi + config::constant_r;
    zi = static_cast<Tfloat>(2.0) * zi * temp + config::constant_i;
    mod = zr * zr + zi * zi;
    ++i;
  }
  return static_cast<float>(i);
  /*return static_cast<float>(i) -*/
  /*       static_cast<float>(log2(std::max(Tfloat(1.0), log2(mod))));*/
}

}; // namespace fractal
