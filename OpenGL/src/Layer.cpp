#include "Layer.h"

Layer::Layer()
{
	m_SandBox = std::make_unique<SandBox>();
}

Layer::~Layer()
{
}

void Layer::OnUpdate(float deltaTime)
{
}

void Layer::OnEvent(bool* Key)
{
	if (Key[GLFW_KEY_A]){
		m_SandBox->SetTranslation(glm::vec3( 1.0f, 0, 0));
	}else if (Key[GLFW_KEY_D]) {
		m_SandBox->SetTranslation(glm::vec3(-1.0f, 0, 0));
	}
	if (Key[GLFW_KEY_W]) {
		m_SandBox->SetTranslation(glm::vec3(0, -1.0f, 0));
	}
	else if (Key[GLFW_KEY_S]) {
		m_SandBox->SetTranslation(glm::vec3(0, 1.0f, 0));
	}
}

void Layer::OnRender()
{
	m_SandBox->OnRender();
}

void Layer::OnImGuiRender()
{
}