#pragma once

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Shader.h"

#include "Log.h"
#include "Renderer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

struct Vertex
{
	glm::vec2 m_Position;

	glm::vec4 m_Color;
	float m_Mass;
	float m_Wave;
	glm::vec2 m_Velocity;
	glm::vec2 m_CountVelocity;
	//glm::vec3 m_Weight0;
	//glm::vec3 m_Weight1;
	//glm::vec3 m_Weight2;
	//glm::vec2 m_TexCoord;
	//unsigned int m_TexIndex;

	static constexpr unsigned int getcount() {return 6+2+4; }

	Vertex(glm::vec2 m_Position, glm::vec4 m_Color, float m_Mass, float m_Wave, glm::vec2 m_Velocity, glm ::vec2 m_CountVelocity)
		: m_Position(m_Position), m_Color(m_Color), m_Mass(m_Mass), m_Wave(m_Wave), m_Velocity(m_Velocity), m_CountVelocity(m_CountVelocity)
	{
	}
	//~Vertex();
};

//struct VertexBackend 
//{
//	glm::vec2 m_Position;
//
//	glm::vec4 m_Color;
//	float m_Mass;
//	float m_Wave;
//	glm::vec2 m_Velocity;
//	glm::vec2 m_CountVelocity;
//	//glm::vec3 m_Weight0;
//	//glm::vec3 m_Weight1;
//	//glm::vec3 m_Weight2;
//	//glm::vec2 m_TexCoord;
//	//unsigned int m_TexIndex;
//
//	static constexpr unsigned int getcount() { return 6 + 2 + 4; }
//
//	Vertex(glm::vec2 m_Position, glm::vec4 m_Color, float m_Mass, float m_Wave, glm::vec2 m_Velocity, glm::vec2 m_CountVelocity)
//		: m_Position(m_Position), m_Color(m_Color), m_Mass(m_Mass), m_Wave(m_Wave), m_Velocity(m_Velocity), m_CountVelocity(m_CountVelocity)
//	{
//	}
//	//~Vertex();
//};

class SandBox
{
public:
	float dispersion = 0.105;
	float mDispersion = 0.3;
	float repulse = 1.01;
	float sqrtTwoOTwo = 0.70710678118;
	float gap = 1.0f;
	float accelDamp = 0.1f;
	float well = 5.0f;
	float intensity = 1;
	glm::vec2 accel;

	const int gridLength = 100;
	float sideLength = 4.0f ; // 10.0f is the width and height of the square
	unsigned int tileSetSL[100]; //same as gridLength
	unsigned int tileSetNL[100];	//same as gridLength
	Vertex* vertMat[100][100];

	SandBox();
	~SandBox();

	void OnUpdate(float deltaTime);
	void OnRender();

	void OnImGuiRender();
	void AddQuad(float x, float y, float mass, glm::vec4(rgba));
	void SetVerticesMovement(int i, int j, int ioffset, int joffset);
	void Vertices1MovementLogic(int i, int j, int ip1, int im1, int jp1, int jm1, float deltaTime);

	void SetRenderer(Renderer* m_Renderer);
	void SetTranslation(glm::vec3 m_Translate);
	void FollowPlayer();

	void PlayerMovementW();
	void PlayerMovementA();
	void PlayerMovementS();
	void PlayerMovementD();


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
	glm::vec2 m_Player;
	
};
