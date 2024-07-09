#ifndef UTIL_H
#define UTIL_H
#include <string>
#include <vector>
#include <fstream>
#include <glad/glad.h>
#include <sstream>
#include <random>
#include <filesystem>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace util
{
    std::string getcwd();
    float Random();
    void PrintMat4(const glm::mat4& matrix);
    void checkGLError();
}





#endif