#include "Log.h"


void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

void GLCheckError() {
    while (GLenum error = glGetError()) {
        const char* errorString = reinterpret_cast<const char*>(glewGetErrorString(error));
        std::cout << "[OpenGL Error] (" << error << "): " << errorString << std::endl;
    }
}

bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        const char* errorString = reinterpret_cast<const char*>(glewGetErrorString(error));
        std::cout << "[OpenGL Error] (" << error << "): " << errorString <<
            " in " << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}