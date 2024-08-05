#include "util.h"

std::string util::getcwd()
{
    std::filesystem::path currentPath = std::filesystem::current_path();
    // If you need it explicitly as a std::string
    return currentPath.string();
}

float util::Random()
{
    // Create a random number generator
    std::random_device rd;   // obtain a random number from hardware
    std::mt19937 gen(rd());  // seed the generator
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);  // define the range

    // Generate a random float between 0 and 1
    return dis(gen);
}

float util::RandomMax(float max)
{
    return Random()*max;
}

void util::PrintMat4(const glm::mat4& matrix) {
    const float* mat = glm::value_ptr(matrix);
    for (int i = 0; i < 4; ++i) {
        std::cout << "| ";
        for (int j = 0; j < 4; ++j) {
            std::cout << mat[j * 4 + i] << "\t";
        }
        std::cout << "|\n";
    }
}

void util::checkGLError()
{
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }
}

uint32_t util::GetMemoryUsageKb()
{
    #ifdef __linux__
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        return usage.ru_maxrss;
    #endif
    return 0;
}
