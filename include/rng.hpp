#include <random>

class rng {
protected:
  std::random_device rd;
  std::mt19937 generator;

  rng() : generator(rd()){};
};

template <typename T> class randomNumber : public rng {
private:
  std::uniform_real_distribution<T> dist;
public:
  randomNumber() : rng(), dist(0.0f, 1.0f) {}

  T get() { return dist(generator); }

  T getUnder(T max) { return get() * max; }

  T getRange(T min, T max) { return (max - min) * get() + min; }
  T getRange(T width) { return getRange(0, width); }
};

template <typename T> 
class RNG {
private:
  static randomNumber<T> generator;
public:
  static T get() {
    return generator.get();
  }
  static T getUnder(T max) {
    return generator.getUnder(max);
  }
  static T getRange(T min, T max) {
    return generator.getRange(min,max);
  }

};

using RNGd = RNG<double>;

template<typename T>
randomNumber<T> RNG<T>::generator = randomNumber<T>();
