#pragma once
#include <cstdint>
struct config {
  using Tfloat = double;
  static constexpr uint32_t maxIteration = 1000;
  static constexpr uint32_t samples = 16;
  static constexpr Tfloat constant_r = static_cast<Tfloat>(-0.8) ;
  static constexpr Tfloat constant_i = static_cast<Tfloat>(0.156);
  static constexpr float fadetime = 2.0f;
};
