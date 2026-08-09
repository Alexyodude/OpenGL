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
//#include "ShaderSource.h" // This is where the shader source code is stored

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

struct Vertex
{
	glm::vec2 m_Position;
	glm::vec4 m_Color;

	static constexpr unsigned int getcount() {return 6; }

	Vertex(glm::vec2 m_Position, glm::vec4 m_Color)
		: m_Position(m_Position), m_Color(m_Color)
	{
	}
	//~Vertex();
};

struct VertexBackend 
{
	glm::vec4 m_Color;
	float m_Mass;
	float m_Wave;
	glm::vec2 m_Velocity;
	glm::vec2 m_CountVelocity;

	static constexpr unsigned int getcount() { return 4 + 2 + 4; }

	VertexBackend(glm::vec4 m_Color, float m_Mass, float m_Wave, glm::vec2 m_Velocity, glm::vec2 m_CountVelocity)
		: m_Color(m_Color), m_Mass(m_Mass), m_Wave(m_Wave), m_Velocity(m_Velocity), m_CountVelocity(m_CountVelocity)
	{
	}
	//~Vertex();
};

class SandBox
{
public:
	float dispersion = 0.105;
	float mDispersion = 1;
	float repulse = 1.5;
	float sqrtTwoOTwo = 0.70710678118;
	float accelDamp = 1;
	float well = 10.0f;
	float intensity = 1;
	glm::vec2 accel;

	const int gridLength = 151; //odd number
	const int gridLengthBackend = 500;
	float sideLength = 5.0f ; // 10.0f is the width and height of the square
	float gap = 0; // 1.0f is the gap between the squares
	//VertexBackend* vertMat[100][100];

	SandBox();
	~SandBox();

	void OnUpdate(float deltaTime);
	void OnRender();

	void OnImGuiRender();
	void AddQuad(float x, float y, glm::vec4 rgba);
	void AddQuadBackend(float mass, glm::vec4 rgba);
	void SetVerticesMovement(int& i, int& j, int ioffset, int joffset);
	void SetVerticesMomentum(int& i, int& j, int i1, int j1);
	void Vertices1MovementLogic(int i, int j, int ip1, int im1, int jp1, int jm1, float deltaTime);

	void SetRenderer(Renderer* m_Renderer);
	void SetTranslation(glm::vec3 m_Translate);
	void FollowPlayer();

	void PlayerMovementW();
	void PlayerMovementA();
	void PlayerMovementS();
	void PlayerMovementD();

	void Error(int a, const char b[], int size);


	//unsigned int vertexCount;
	//Vertex vertices[4];
	std::vector<VertexBackend> verticesBackend;
	std::vector<Vertex> vertices;
	//std::vector<unsigned int> index;
	//unsigned int index[12] = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};
	std::vector<unsigned int> index;// = { {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4} };

	// Was uninitialised. It seeds every index value in AddQuad, so its garbage
	// value decides whether the grid renders at all. The MSBuild/Windows build
	// happened to start it at 0, which is why that build worked; other
	// toolchains start it at garbage and render nothing. Pinned to 0 to lock in
	// the behaviour the working build already had.
	int vertexCount = 0;

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
	glm::vec2 m_Player_Cam;
	
};
