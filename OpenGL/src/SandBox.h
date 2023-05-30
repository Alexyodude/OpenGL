#pragma once

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"

#include "Log.h"
#include "Renderer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

struct Vertex
{
	glm::vec3 m_Position;
	//glm::vec4 color;
	glm::vec2 m_TexCoord;
	unsigned int m_TexIndex;

	static constexpr unsigned int getcount() {return 6; }

	Vertex(glm::vec3 m_Position = glm::vec3(0, 0, 0), glm::vec2 m_TexCoord = glm::vec2(0, 0), float m_TexIndex = 0)
		: m_Position(m_Position), m_TexCoord(m_TexCoord), m_TexIndex(m_TexIndex)
	{
	}
	//~Vertex();
};

class SandBox
{
public:
	float sideLength = 100.0f / 2; // 100.0f is the width and height of the square

	SandBox();
	~SandBox();

	void OnUpdate(float deltaTime);
	void OnRender();

	void OnImGuiRender();
	void AddQuad(float x, float y, unsigned int texindex);
	void TileMap();

	void SetRenderer(Renderer* m_Renderer);
	void SetTranslation(glm::vec3 m_Translate);


	//unsigned int vertexCount;
	//Vertex vertices[4];
	std::vector<Vertex> vertices1;
	//std::vector<unsigned int> index;
	//unsigned int index[12] = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};
	std::vector<unsigned int> index1;// = { {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4} };

	int vertexCount;

private:
	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<VertexBuffer> m_VertexBuffer;
	std::unique_ptr<IndexBuffer> m_IndexBuffer;
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Renderer> m_Renderer;
	std::unique_ptr<VertexBufferLayout> m_Layout;
	
	glm::mat4 m_Proj, m_View;
	glm::vec3 m_Translation;
	
};
