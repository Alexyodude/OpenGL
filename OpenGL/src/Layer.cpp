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
		#ifdef ScreenMovement
			m_SandBox->SetTranslation(glm::vec3( 5.0f, 0, 0));
		#endif // ScreenMovement
		m_SandBox->PlayerMovementA();
	}else if (Key[GLFW_KEY_D]) {
		#ifdef ScreenMovement
			m_SandBox->SetTranslation(glm::vec3(-5.0f, 0, 0));
		#endif // ScreenMovement	
		m_SandBox->PlayerMovementD();
	}
	if (Key[GLFW_KEY_W]) {
		#ifdef ScreenMovement
				m_SandBox->SetTranslation(glm::vec3(0, -5.0f, 0));
		#endif // ScreenMovement
		m_SandBox->PlayerMovementW();
	}
	else if (Key[GLFW_KEY_S]) {
		#ifdef ScreenMovement
				m_SandBox->SetTranslation(glm::vec3(0, 5.0f, 0));
		#endif // ScreenMovement
		m_SandBox->PlayerMovementS();
	}
}

void Layer::OnRender()
{
	m_SandBox->OnRender();
}

void Layer::OnImGuiRender()
{
}