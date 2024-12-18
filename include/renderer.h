#pragma once
#include "config.h"
#include "fractal.h"
#include "palette.hpp"
#include "rng.hpp"
#include "shader.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "stealpool.h"
#include "texture.hpp"
#include "utilities.h"
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <future>
#include <iostream>
#include <ostream>
#include <vector>

template <typename R> bool is_ready(std::future<R> const &f) {
  return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

template <typename Tfloat> struct renderState {
  std::unique_ptr<vertexGrid> grid;
  Tfloat zoom;
  std::array<Tfloat, 2> offset;

  /*~renderState(){}*/

  renderState(uint32_t width, uint32_t height)
      : zoom(1.0), grid(std::make_unique<vertexGrid>(width, height)) {
    /*grid = vertexGrid(width, height);*/
    offset = std::array<Tfloat, 2>{0.0, 0.0};
    /*printf("hi im here first\n");*/
  };
};

template <typename Tfloat> class renderer {
  std::vector<renderState<Tfloat>> states;

  Tfloat requestedzoom = 1.0;
  std::array<Tfloat, 2> requestedCenter = {0.0, 0.0};

  Tfloat renderedZoom = 1.0;
  std::array<Tfloat, 2> renderedCenter = {0.0, 0.0};

  uint32_t stateIndex = 0;

  palette Palette;

  Shader *pShaders = nullptr;

  // antialiasin
  std::array<std::array<Tfloat, 2>, 32> AAoffsets;
  std::array<Mytexture, 2> Textures;
  // palette
  uint32_t textureIndex = 0;

  float fadetime;

public:
  renderer(uint32_t width, uint32_t height, Shader *_pShaders)
      : pShaders(_pShaders), fadetime(config::fadetime) {
    /*requested_zoom = zoom_;*/
    Palette.addColor(0.0f, color{25, 24, 23, 255});
    Palette.addColor(0.03f, color{120, 90, 70, 255});
    Palette.addColor(0.05f, color{130, 24, 23, 255});
    Palette.addColor(0.25f, color{250, 179, 100, 255});
    Palette.addColor(0.5f, color{43, 65, 98, 255});
    Palette.addColor(0.85f, color{11, 110, 79, 255});
    Palette.addColor(0.95f, color{150, 110, 79, 255});
    Palette.addColor(1.0f, color{255, 255, 255, 255});

    states.push_back(renderState<Tfloat>(width, height));
    /*printf("hello im in renderer\n");*/
    states.push_back(renderState<Tfloat>(width, height));
    /*states[0] = renderState<Tfloat>(width, height);*/
    /*states[1] = renderState<Tfloat>(width, height);*/

    for (auto &a : AAoffsets) {
      a[0] = RNGd::getUnder(1.0);
      a[1] = RNGd::getUnder(1.0);
    }

    for (auto &a : Textures) {
      a = Mytexture(WIDTH, HEIGHT);
    }

    AAoffsets[0] = {0.0, 0.0};

    /*singleThreaded_work(HEIGHT / 2.0, {0.0, 0.0});*/
    /*const unsigned char *data = states[stateIndex].grid->getData();*/

    /*int result = stbi_write_png("../texture/output.png", width, height, 4,
     * data,*/
    /*                            width * 4);*/
    /*if (result) {*/
    /*  printf("Image saved successfully!\n");*/
    /*} else {*/
    /*  printf("Failed to save image\n");*/
    /*}*/
    /*int width_, height_, channels;*/
    /*unsigned char *imageData =*/
    /*    stbi_load("../texture/wiu.png", &width_, &height_, &channels, 4);*/
    /*if (!imageData) {*/
    /*  printf("Failed to load image\n");*/
    /*  return;*/
    /*}*/
    /*printf("channels: %i", channels);*/
    /*Textures[textureIndex].updateData(data);*/
  }

  ~renderer() { pShaders = nullptr; }

  bool setupRequestedWork() {
    renderState<Tfloat> &backState = states[!stateIndex];
    vertexGrid &currGrid = *backState.grid;

    renderedZoom = requestedzoom;
    renderedCenter = requestedCenter;

    constexpr std::array<Tfloat, 2> winCenter = {
        static_cast<Tfloat>(0.5) * static_cast<Tfloat>(WIDTH),
        static_cast<Tfloat>(0.5) * static_cast<Tfloat>(HEIGHT)};

    const uint32_t nThreads = Tpool.getThreadCount();
    const int sliceSize = currGrid.size[1] / nThreads;
    constexpr float sampleCoef = 1.0f / static_cast<float>(config::samples);

    /*auto start = std::chrono::steady_clock::now();*/
    for (unsigned int i{0}; i < nThreads; i++) {
      Tpool.submit([=] {
        auto &grid = *(states[!stateIndex].grid);
        for (unsigned int x{0}; x < grid.size[0]; x++) {
          for (unsigned int y{i * sliceSize}; y < (i + 1) * sliceSize; y++) {
            const Tfloat xf =
                (static_cast<Tfloat>(x) - winCenter[0]) / renderedZoom +
                renderedCenter[0];
            const Tfloat yf =
                (static_cast<Tfloat>(y) - winCenter[1]) / renderedZoom +
                renderedCenter[1];

            glm::vec3 vColor(0.0f);
            // monte carlo AAoffsets
            for (uint32_t k{config::samples}; k--;) {
              const std::array<Tfloat, 2> off = {AAoffsets[k][0] / renderedZoom,
                                                 AAoffsets[k][1] /
                                                     renderedZoom};
              const float iter =
                  fractal::julia<Tfloat>({xf + off[0], yf + off[1]});
              const float iterRatio =
                  iter / static_cast<float>(config::maxIteration);
              vColor += glm::vec3(Palette.getVecColorByValue(iterRatio));
            }
            grid.setColor(x, y, palette::vec3toColor(vColor * sampleCoef));
          }
        }
      });
    }
    /*auto end = std::chrono::steady_clock::now();*/
    /*auto duration =*/
    /*    std::chrono::duration_cast<std::chrono::milliseconds>(end - start);*/
    /*// Output the result*/
    /*std::cout << "Function execution time: " << duration.count()*/
    /*          << " milliseconds" << std::endl;*/
    return true;
  }
  void printVec3(const glm::vec3 &vec) {
    std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")"
              << std::endl;
  }

  void singleThreaded_work(Tfloat zoom, std::array<Tfloat, 2> center) {
    renderState<Tfloat> &backState = states[stateIndex];
    vertexGrid &currGrid = *backState.grid;

    renderedZoom = zoom;
    printf("zoom = %f\n", zoom);
    renderedCenter = center;
    printf("center.x = %f, center.y = %f\n", center[0], center[1]);

    const std::array<Tfloat, 2> winCenter = {
        static_cast<Tfloat>(0.5) * static_cast<Tfloat>(currGrid.size[0]),
        static_cast<Tfloat>(0.5) * static_cast<Tfloat>(currGrid.size[1])};
    printf("winCenter.x = %f and winCenter.y = %f", winCenter[0], winCenter[1]);

    constexpr float sampleCoef = 1.0f / static_cast<float>(config::samples);

    for (int x{0}; x < currGrid.size[0]; ++x) {
      for (int y{0}; y < currGrid.size[1]; ++y) {
        const Tfloat xf =
            (static_cast<Tfloat>(x) - winCenter[0]) / renderedZoom +
            renderedCenter[0];
        const Tfloat yf =
            (static_cast<Tfloat>(y) - winCenter[1]) / renderedZoom +
            renderedCenter[1];

        glm::vec3 vColor(0.0f);
        /*printVec3(vColor);*/
        // monte carlo AAoffsets
        for (uint32_t i{config::samples}; i--;) {
          const std::array<Tfloat, 2> off = {AAoffsets[i][0] / renderedZoom,
                                             AAoffsets[i][1] / renderedZoom};
          const float iter = fractal::julia<Tfloat>({xf + off[0], yf + off[1]});
          const float iterRatio =
              iter / static_cast<float>(config::maxIteration);
          /*printf("iter: %f\n", iter);*/

          vColor += glm::vec3(Palette.getVecColorByValue(iterRatio));
          /*printVec3(vColor);*/
        }
        /*printVec3(vColor);*/
        glm::vec3 out = vColor * sampleCoef;
        currGrid.setColor(x, y, palette::vec3toColor(out));
      }
    }
  }

  void render(Tfloat zoom, std::array<Tfloat, 2> center) {
    if (fadetime >= config::fadetime && Tpool.isDone()) {
      renderState<Tfloat> &TempState = states[!stateIndex];
      TempState.zoom = renderedZoom;
      TempState.offset[0] = renderedCenter[0];
      TempState.offset[1] = renderedCenter[1];
      // swap cpu buffers
      stateIndex = !stateIndex;
      // texture updates
      auto data = states[stateIndex].grid->getData();
      Textures[!textureIndex].updateData(data);

      textureIndex = !textureIndex;

      fadetime = 0.0f;
      setupRequestedWork();
      printf("new render\n");

    } // after

    requestedCenter[0] = center[0];
    requestedCenter[1] = center[1];
    requestedzoom = zoom;

    // new_texture
    const float scale_new = getStateScale(stateIndex);
    std::array<float, 2> offset_new = getStateOffset(stateIndex);

    /*std::vector<float> origin = std::vector<float>{WIDTH * 0.5F, HEIGHT *
     * 0.5F};*/
    // something about new texture uniform sets - >
    // offset and scale
    pShaders->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Textures[textureIndex].getTexture());

    pShaders->set2Float("tex1_scale", (float[]){scale_new, scale_new});
    pShaders->set2Float("tex1_offset", offset_new.data());

    // old_texture
    const float scale_old = getStateScale(!stateIndex);
    std::array<float, 2> offset_old = getStateOffset(!stateIndex);
    // something about old texture uniform sets - >
    // offset and scale
    //
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Textures[!textureIndex].getTexture());

    // texture shader binding
    glUniform1i(glGetUniformLocation(pShaders->ID, "tex1"), 0);
    glUniform1i(glGetUniformLocation(pShaders->ID, "tex2"), 1);

    // float noSE T>T
    pShaders->set2Float("tex2_scale", (float[]){scale_old, scale_old});
    pShaders->set2Float("tex2_offset", offset_old.data());

    // draw on display
    const float alpha = std::max(0.0f, 1.0f - fadetime / config::fadetime);

    pShaders->setFloat("alpha", static_cast<float>(alpha));

    fadetime += 0.016f;
  }

  void deactivate_Textures() {
    glActiveTexture(GL_TEXTURE0);    // Make texture unit 0 active
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture on texture unit 0

    glActiveTexture(GL_TEXTURE1);    // Make texture unit 1 active
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture on texture unit 1
  }

  float getStateScale(unsigned idx) const {
    return static_cast<float>(requestedzoom / states[idx].zoom);
  }

  std::array<float, 2> getStateOffset(uint32_t idx) const {
    //  return std::array<float, 2>{
    //      static_cast<float>((-states[idx].offset[0] + requestedCenter[0]) *
    //                         states[idx].zoom / static_cast<Tfloat>(WIDTH)) *
    //          getStateScale(idx),
    //      static_cast<float>((-states[idx].offset[1] + requestedCenter[1]) *
    //                         states[idx].zoom / static_cast<Tfloat>(HEIGHT)) *
    //         getStateScale(idx)};
    //}
    return std::array<float, 2>{
        static_cast<float>((-states[idx].offset[0] + requestedCenter[0]) *
                           states[idx].zoom / static_cast<Tfloat>(WIDTH)),
        static_cast<float>((-states[idx].offset[1] + requestedCenter[1]) *
                           states[idx].zoom / static_cast<Tfloat>(HEIGHT))};
  }

  /*    return std::array<float, 2>{*/
  /*      static_cast<float>(( -states[idx].offset[0] + requestedCenter[0]) **/
  /*                         states[idx].zoom) **/
  /*          getStateScale(idx) ,*/
  /*      static_cast<float>((-states[idx].offset[1] + requestedCenter[1]) **/
  /*                         states[idx].zoom) **/
  /*          getStateScale(idx) };*/
  /*}*/
};
