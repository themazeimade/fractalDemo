struct config {
  using Tfloat = double;
  static constexpr unsigned maxIteration = 1000;
  static constexpr unsigned samples = 8;
  static constexpr Tfloat constant_r = static_cast<Tfloat>(0.654) ;
  static constexpr Tfloat constant_i = static_cast<Tfloat>(0.256);
};
