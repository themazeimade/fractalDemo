#pragma once
#include "utilities.h" // include glad to get all the required OpenGL headers
#include <string>
  

class Shader
{
public:
    // the program ID
    unsigned int ID;
  
    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const std::string &name, bool value) const;  
    void setInt(const std::string &name, int value) const;   
    void setFloat(const std::string &name, float value) const;
    void set3Float(const std::string &name, float* value) const;
    void set2Float(const std::string &name, float* value) const;
};


