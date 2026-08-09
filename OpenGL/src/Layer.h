#pragma once
#include <memory> // std::unique_ptr - MSVC's STL pulls this in transitively, libstdc++ does not
#include "SandBox.h"
#include "GLFW/glfw3.h"

class Layer
{
private:
	std::unique_ptr<SandBox> m_SandBox;
	bool* m_Key;
public:
	Layer();
	~Layer();

	void OnUpdate(float deltaTime);
	void OnEvent(bool* m_Key);
	void OnRender();
	void OnImGuiRender();
};

