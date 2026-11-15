#pragma once

#include <string>

struct GLFWwindow;

class Application {
public:
    Application(int width, int height, const std::string& title);
    ~Application();

    bool Init();
    void Shutdown();

    bool ShouldClose() const;
    void BeginFrame();
    void EndFrame();

    GLFWwindow* GetWindow() const { return m_Window; }
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

private:
    GLFWwindow* m_Window = nullptr;
    int m_Width;
    int m_Height;
    std::string m_Title;
};
