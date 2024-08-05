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


#ifdef __linux__
#include <unistd.h>
#include <sys/resource.h>
#endif


namespace util
{
    std::string getcwd();
    float Random();
    float RandomMax(float max);
    void PrintMat4(const glm::mat4& matrix);
    void checkGLError();
    uint32_t GetMemoryUsageKb();
}





#endif