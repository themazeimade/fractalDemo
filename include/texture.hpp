#pragma once
#include "utilities.h"
#include <cstdint>

class Mytexture {
public:
  GLuint textureID;
  unsigned width;
  unsigned height;

  Mytexture(){}
  Mytexture(uint32_t width, uint32_t height) :width(width), height(height) {

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE); // Horizontal wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    GL_CLAMP_TO_EDGE); // Vertical wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, nullptr);
    /*glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,*/
    /*             GL_UNSIGNED_BYTE, nullptr);*/
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void updateData (const unsigned char *data) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  int getHeight() { return height; }

  int getWidth() { return width; }

  void draw() {}

  GLuint &getTexture() { return textureID; }
};
