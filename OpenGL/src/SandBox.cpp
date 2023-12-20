#define EnableEdge
#define ImGui_DEBUG

//#define DetectArrayError

#include "SandBox.h"
#include "ShaderSource.h" // This is where the shader source code is stored


SandBox::SandBox()
	: m_Proj(glm::ortho(0.0f, (float)Window_Width, 0.0f, (float)Window_Height, -1.0f, 1.0f)),
	m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
	m_Translation(0, 0, 0), m_Renderer(), m_Layout(new VertexBufferLayout()), m_Player(0, 0)
{
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	m_Player_Cam = glm::vec2(gridLengthBackend * 0.5, gridLengthBackend * 0.5);
	m_VAO = std::make_unique<VertexArray>();

	m_Shader = std::make_unique<Shader>(
		ShaderSource::BasicVertexShader,
		ShaderSource::BasicFragmentShader
	);
	//m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
	m_Shader->Bind();

	m_Layout->Push<float>(2);
	m_Layout->Push<float>(4);

	/*
	Initilizes the backend grid and the front end grid
	*/
	for (int i = 0; i < gridLengthBackend; i++) {
		for (int j = 0; j < gridLengthBackend; j++) {
			float mass = std::rand() / (float)RAND_MAX-0.9;
			if (i - 10 < (int)gridLengthBackend * 0.5 && j - 10 < (int)gridLengthBackend * 0.5
				&& i + 10 > (int)gridLengthBackend * 0.5 && j + 10 > (int)gridLengthBackend * 0.5) {
				if (i == (int)gridLengthBackend * 0.5 && j == (int)gridLengthBackend * 0.5) {
					m_Player = glm::vec2(i, j * gridLengthBackend);
					SandBox::AddQuadBackend(1, glm::vec4(0.8, 0.4, 0.2, 1.0f));
					std::cout << "i: " << i << " j: " << j << std::endl;
				}
				else {
					SandBox::AddQuadBackend(0, glm::vec4(1.0f, 1.0f, 1.0f, 0));
				}
			}
			else if (mass <= 0) {
				SandBox::AddQuadBackend(0, glm::vec4(1.0f, 1.0f, 1.0f, 0));
			}
			else if (mass > 0) {
				SandBox::AddQuadBackend(1, glm::vec4(0.6, 1, 0.6, 1.0f));
			}
		}
	}
	for (int i = 0; i < gridLength; i++) {
		for (int j = 0; j < gridLength; j++) {
			SandBox::AddQuad((i - gridLength * 0.5) * (sideLength + gap) + Window_Width * 0.5, (j - gridLength * 0.5) * (sideLength + gap) + Window_Height * 0.5, verticesBackend[m_Player[0] - (gridLength-1) * 0.5 + i + m_Player[1] + (j - (gridLength - 1) * 0.5) * gridLengthBackend].m_Color);
		}
	}
}

SandBox::~SandBox()
{
}

void SandBox::OnUpdate(float deltaTime)
{
	//std::cout << m_Player[0] << " " << m_Player[1] << std::endl;
	/*
	|	|	|
	|	|	|
	|	|	0
	*/
#ifdef EnableEdge
	Vertices1MovementLogic(0, gridLengthBackend - 1, 1, gridLengthBackend - 1, 0, gridLengthBackend - 2, deltaTime);
#endif
	/*
	|	|	0
	|	|	|
	|	|	|
	*/
#ifdef EnableEdge
	Vertices1MovementLogic(gridLengthBackend - 1, gridLengthBackend - 1, 0, gridLengthBackend - 2, 0, gridLengthBackend - 2, deltaTime);
#endif
	/*
	|	|	|
	|	|	|
	0	|	|
	*/
#ifdef EnableEdge
	Vertices1MovementLogic(0, 0, 1, gridLengthBackend - 1, 1, gridLengthBackend - 1, deltaTime);
#endif
	/*
	0	|	|
	|	|	|
	|	|	|
	*/
#ifdef EnableEdge
	Vertices1MovementLogic(gridLengthBackend - 1, 0, 0, gridLengthBackend - 2, 1, gridLengthBackend - 1, deltaTime);
#endif
	for (int i = 1; i < gridLengthBackend-1; i++) {
		/*
		|	|	|
		0	|	|
		|	|	|
		*/
#ifdef EnableEdge
		Vertices1MovementLogic(i, 0, i + 1, i - 1, 1, gridLengthBackend - 1, deltaTime);
#endif
		/*
		|	|	|
		|	|	0
		|	|	|
		*/
#ifdef EnableEdge
		Vertices1MovementLogic(i, gridLengthBackend - 1, i + 1, i - 1, 0, gridLengthBackend - 2, deltaTime);
#endif
		/*
		|	|	|
		|	|	|
		|	0	|
		*/
#ifdef EnableEdge
		Vertices1MovementLogic(0, i, 1, gridLengthBackend - 1, i + 1, i - 1, deltaTime);
#endif
		/*
		|	0	|
		|	|	|
		|	|	|
		*/

#ifdef EnableEdge
		Vertices1MovementLogic(gridLengthBackend - 1, i, 0, gridLengthBackend - 2, i + 1, i - 1, deltaTime);
#endif
		for (int j = 1; j < gridLengthBackend-1; j++) {
			/*
			|	|	|
			|	0	|
			|	|	|
			*/
			Vertices1MovementLogic(i, j, i + 1, i - 1, j + 1, j - 1, deltaTime);
		}
	}

	for (int i = 0; i < gridLength; i++) {
		for (int j = 0; j < gridLength; j++) {
			if (m_Player[0] - (gridLength - 1) * 0.5 + i < 0) {
				if (m_Player[1] + ((1 - gridLength) * 0.5 + j) * gridLengthBackend < 0) {
#ifdef DetectArrayError
					Error(i + j * gridLength + 0, "i + j * gridLength + 0", vertices.size());
					Error(m_Player[0] - (gridLength - 1) * 0.5 + i + gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j + gridLengthBackend)) * gridLengthBackend, "m_Player[0] - (gridLength - 1) * 0.5 + i + gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j + gridLengthBackend)) * gridLengthBackend", verticesBackend.size());
#endif
					vertices[(i + j * gridLength) * 4 + 0].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j + gridLengthBackend)) * gridLengthBackend].m_Color;
					vertices[(i + j * gridLength) * 4 + 1].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j + gridLengthBackend)) * gridLengthBackend].m_Color;
					vertices[(i + j * gridLength) * 4 + 2].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j + gridLengthBackend)) * gridLengthBackend].m_Color;
					vertices[(i + j * gridLength) * 4 + 3].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j + gridLengthBackend)) * gridLengthBackend].m_Color;
				}
				else if (m_Player[1] + ((1 - gridLength) * 0.5 + j) * gridLengthBackend > (gridLengthBackend - 1) * gridLengthBackend) {
#ifdef DetectArrayError
					Error(i + j * gridLength + 0, "i + j * gridLength + 0", vertices.size());
					Error(m_Player[0] - (gridLength - 1) * 0.5 + i + gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j - gridLengthBackend)) * gridLengthBackend, "m_Player[0] - (gridLength - 1) * 0.5 + i + gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j - gridLengthBackend)) * gridLengthBackend", verticesBackend.size());
#endif
					vertices[(i + j * gridLength) * 4 + 0].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j - gridLengthBackend)) * gridLengthBackend].m_Color;
					vertices[(i + j * gridLength) * 4 + 1].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j - gridLengthBackend)) * gridLengthBackend].m_Color;
					vertices[(i + j * gridLength) * 4 + 2].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j - gridLengthBackend)) * gridLengthBackend].m_Color;
					vertices[(i + j * gridLength) * 4 + 3].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j - gridLengthBackend)) * gridLengthBackend].m_Color;
				}
				else {
#ifdef DetectArrayError
					Error(i + j * gridLength + 0, "i + j * gridLength + 0", vertices.size());
					Error(m_Player[0] - (gridLength - 1) * 0.5 + i + gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j)) * gridLengthBackend, "m_Player[0] - (gridLength - 1) * 0.5 + i + gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j)) * gridLengthBackend", verticesBackend.size());
#endif
					vertices[(i + j * gridLength) * 4 + 0].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j)) * gridLengthBackend].m_Color;
					vertices[(i + j * gridLength) * 4 + 1].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j)) * gridLengthBackend].m_Color;
					vertices[(i + j * gridLength) * 4 + 2].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j)) * gridLengthBackend].m_Color;
					vertices[(i + j * gridLength) * 4 + 3].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j)) * gridLengthBackend].m_Color;
				}
			}
			else if (m_Player[0] - (gridLength - 1) * 0.5 + i > gridLengthBackend - 1) {
				if (m_Player[1] + ((1 - gridLength) * 0.5 + j) * gridLengthBackend < 0) {
#ifdef DetectArrayError
					Error(i + j * gridLength + 0, "i + j * gridLength + 0", vertices.size());
					Error(m_Player[0] - (gridLength - 1) * 0.5 + i - gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j + gridLengthBackend)) * gridLengthBackend, "m_Player[0] - (gridLength - 1) * 0.5 + i - gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j + gridLengthBackend)) * gridLengthBackend", verticesBackend.size());
#endif
					vertices[(i + j * gridLength) * 4 + 0].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i - gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j + gridLengthBackend)) * gridLengthBackend].m_Color;
					vertices[(i + j * gridLength) * 4 + 1].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i - gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j + gridLengthBackend)) * gridLengthBackend].m_Color;
					vertices[(i + j * gridLength) * 4 + 2].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i - gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j + gridLengthBackend)) * gridLengthBackend].m_Color;
					vertices[(i + j * gridLength) * 4 + 3].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i - gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j + gridLengthBackend)) * gridLengthBackend].m_Color;
				}
				else if (m_Player[1] + ((1 - gridLength) * 0.5 + j) * gridLengthBackend > (gridLengthBackend - 1) * gridLengthBackend) {
#ifdef DetectArrayError
					Error(i + j * gridLength + 0, "i + j * gridLength + 0", vertices.size());
					Error(m_Player[0] - (gridLength - 1) * 0.5 + i - gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j - gridLengthBackend)) * gridLengthBackend, "m_Player[0] - (gridLength - 1) * 0.5 + i - gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j - gridLengthBackend)) * gridLengthBackend", verticesBackend.size());
#endif
					vertices[(i + j * gridLength) * 4 + 0].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i - gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j - gridLengthBackend)) * gridLengthBackend].m_Color;
					vertices[(i + j * gridLength) * 4 + 1].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i - gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j - gridLengthBackend)) * gridLengthBackend].m_Color;
					vertices[(i + j * gridLength) * 4 + 2].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i - gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j - gridLengthBackend)) * gridLengthBackend].m_Color;
					vertices[(i + j * gridLength) * 4 + 3].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i - gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j - gridLengthBackend)) * gridLengthBackend].m_Color;
				}
				else {
#ifdef DetectArrayError
					Error(i + j * gridLength + 0, "i + j * gridLength + 0", vertices.size());
					Error(m_Player[0] - (gridLength - 1) * 0.5 + i - gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j)) * gridLengthBackend, "m_Player[0] - (gridLength - 1) * 0.5 + i - gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j)) * gridLengthBackend", verticesBackend.size());
#endif
					vertices[(i + j * gridLength) * 4 + 0].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i - gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j)) * gridLengthBackend].m_Color;
					vertices[(i + j * gridLength) * 4 + 1].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i - gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j)) * gridLengthBackend].m_Color;
					vertices[(i + j * gridLength) * 4 + 2].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i - gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j)) * gridLengthBackend].m_Color;
					vertices[(i + j * gridLength) * 4 + 3].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i - gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j)) * gridLengthBackend].m_Color;
				}
			}
			else if (m_Player[1] + ((1 - gridLength) * 0.5 + j) * gridLengthBackend < 0) {
#ifdef DetectArrayError
				Error(i + j * gridLength + 0, "i + j * gridLength + 0", vertices.size());
				Error(m_Player[0] - (gridLength - 1) * 0.5 + i + m_Player[1] + (((1 - gridLength) * 0.5 + j + gridLengthBackend)) * gridLengthBackend, "m_Player[0] - (gridLength - 1) * 0.5 + i - gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j + gridLengthBackend)) * gridLengthBackend", verticesBackend.size());
#endif
				vertices[(i + j * gridLength) * 4 + 0].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + m_Player[1] + (((1 - gridLength) * 0.5 + j + gridLengthBackend)) * gridLengthBackend].m_Color;
				vertices[(i + j * gridLength) * 4 + 1].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + m_Player[1] + (((1 - gridLength) * 0.5 + j + gridLengthBackend)) * gridLengthBackend].m_Color;
				vertices[(i + j * gridLength) * 4 + 2].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + m_Player[1] + (((1 - gridLength) * 0.5 + j + gridLengthBackend)) * gridLengthBackend].m_Color;
				vertices[(i + j * gridLength) * 4 + 3].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + m_Player[1] + (((1 - gridLength) * 0.5 + j + gridLengthBackend)) * gridLengthBackend].m_Color;
			}
			else if (m_Player[1] + ((1 - gridLength) * 0.5 + j) * gridLengthBackend > (gridLengthBackend - 1) * gridLengthBackend) {
#ifdef DetectArrayError
				Error(i + j * gridLength + 0, "i + j * gridLength + 0", vertices.size());
				Error(m_Player[0] - (gridLength - 1) * 0.5 + i + m_Player[1] + (((1 - gridLength) * 0.5 + j - gridLengthBackend)) * gridLengthBackend, "m_Player[0] - (gridLength - 1) * 0.5 + i - gridLengthBackend + m_Player[1] + (((1 - gridLength) * 0.5 + j - gridLengthBackend)) * gridLengthBackend", verticesBackend.size());
#endif
				vertices[(i + j * gridLength) * 4 + 0].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + m_Player[1] + (((1 - gridLength) * 0.5 + j - gridLengthBackend)) * gridLengthBackend].m_Color;
				vertices[(i + j * gridLength) * 4 + 1].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + m_Player[1] + (((1 - gridLength) * 0.5 + j - gridLengthBackend)) * gridLengthBackend].m_Color;
				vertices[(i + j * gridLength) * 4 + 2].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + m_Player[1] + (((1 - gridLength) * 0.5 + j - gridLengthBackend)) * gridLengthBackend].m_Color;
				vertices[(i + j * gridLength) * 4 + 3].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + m_Player[1] + (((1 - gridLength) * 0.5 + j - gridLengthBackend)) * gridLengthBackend].m_Color;
			}
			else {
#ifdef DetectArrayError
				Error(i + j * gridLength + 0, "i + j * gridLength + 0", vertices.size());
				Error(m_Player[0] - (gridLength - 1) * 0.5 + i + m_Player[1] + (((1 - gridLength) * 0.5 + j)) * gridLengthBackend, "m_Player[0] - (gridLength - 1) * 0.5 + i + m_Player[1] + (((1 - gridLength) * 0.5 + j)) * gridLengthBackend", verticesBackend.size());
#endif
				vertices[(i + j * gridLength) * 4 + 0].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + m_Player[1] + (((1 - gridLength) * 0.5 + j)) * gridLengthBackend].m_Color;
				vertices[(i + j * gridLength) * 4 + 1].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + m_Player[1] + (((1 - gridLength) * 0.5 + j)) * gridLengthBackend].m_Color;
				vertices[(i + j * gridLength) * 4 + 2].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + m_Player[1] + (((1 - gridLength) * 0.5 + j)) * gridLengthBackend].m_Color;
				vertices[(i + j * gridLength) * 4 + 3].m_Color = verticesBackend[m_Player[0] - (gridLength - 1) * 0.5 + i + m_Player[1] + (((1 - gridLength) * 0.5 + j)) * gridLengthBackend].m_Color;
			}
		}
	}
}

void SandBox::SetVerticesMovement(int& i, int& j, int ioffset, int joffset){
	//std::cout << "SetVerticesMovement" << std::endl;
#ifdef DetectArrayError
	Error((i + j * gridLengthBackend), "(i + j * gridLengthBackend)", verticesBackend.size());
#endif
	if (
		ioffset == 1 && joffset == 1) {
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[0] -= sqrtTwoOTwo;
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[1] -= sqrtTwoOTwo;
	}
	else if (
		ioffset == 1 && joffset == -1) {
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[0] -= sqrtTwoOTwo;
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[1] += sqrtTwoOTwo;
	}
	else if (
		ioffset == -1 && joffset == 1) {
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[0] += sqrtTwoOTwo;
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[1] -= sqrtTwoOTwo;
	}
	else if (
		ioffset == -1 && joffset == -1) {
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[0] += sqrtTwoOTwo;
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[1] += sqrtTwoOTwo;
	}
	else if (
		ioffset == 0 && joffset == 1) {
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[1] -= 1;
	}															
	else if (
		ioffset == 0 && joffset == -1) {
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[1] += 1;
	}
	else if (
		ioffset == 1 && joffset == 0) {
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[0] -= 1;
	}
	else if (
		ioffset == -1 && joffset == 0) {
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[0] += 1;
	}
	if (
		j + joffset < 0) {
		if (
			i + ioffset < 0) {
			if (m_Player == glm::vec2(i , j * gridLengthBackend )) {
				m_Player = glm::vec2((gridLengthBackend + ioffset) , (gridLengthBackend + joffset) * gridLengthBackend );
			}
#ifdef DetectArrayError
			Error((gridLengthBackend + ioffset + (gridLengthBackend + joffset) * gridLengthBackend), "(gridLengthBackend + ioffset + (gridLengthBackend + joffset) * gridLengthBackend)", verticesBackend.size());
#endif
			verticesBackend[(gridLengthBackend + ioffset + (gridLengthBackend + joffset) * gridLengthBackend)].m_Mass = verticesBackend[(i + j * gridLengthBackend)].m_Mass;
			verticesBackend[(gridLengthBackend + ioffset + (gridLengthBackend + joffset) * gridLengthBackend)].m_Color[0] = verticesBackend[(i + j * gridLengthBackend)].m_Color[0];
			verticesBackend[(gridLengthBackend + ioffset + (gridLengthBackend + joffset) * gridLengthBackend)].m_Color[1] = verticesBackend[(i + j * gridLengthBackend)].m_Color[1];
			verticesBackend[(gridLengthBackend + ioffset + (gridLengthBackend + joffset) * gridLengthBackend)].m_Color[2] = verticesBackend[(i + j * gridLengthBackend)].m_Color[2];
			verticesBackend[(gridLengthBackend + ioffset + (gridLengthBackend + joffset) * gridLengthBackend)].m_CountVelocity = verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity;
			verticesBackend[(gridLengthBackend + ioffset + (gridLengthBackend + joffset) * gridLengthBackend)].m_Velocity = verticesBackend[(i + j * gridLengthBackend)].m_Velocity;
		}
		else if (
			i + ioffset > gridLengthBackend - 1) {
			if (m_Player == glm::vec2(i , j * gridLengthBackend)) {
				m_Player = glm::vec2((ioffset), (gridLengthBackend + joffset) * gridLengthBackend);
			}
#ifdef DetectArrayError
			Error((ioffset + (gridLengthBackend + joffset) * gridLengthBackend), "(ioffset + (gridLengthBackend + joffset) * gridLengthBackend)", verticesBackend.size());
#endif
			verticesBackend[(ioffset + (gridLengthBackend + joffset) * gridLengthBackend)].m_Mass = verticesBackend[(i + j * gridLengthBackend)].m_Mass;
			verticesBackend[(ioffset + (gridLengthBackend + joffset) * gridLengthBackend)].m_Color[0] = verticesBackend[(i + j * gridLengthBackend)].m_Color[0];
			verticesBackend[(ioffset + (gridLengthBackend + joffset) * gridLengthBackend)].m_Color[1] = verticesBackend[(i + j * gridLengthBackend)].m_Color[1];
			verticesBackend[(ioffset + (gridLengthBackend + joffset) * gridLengthBackend)].m_Color[2] = verticesBackend[(i + j * gridLengthBackend)].m_Color[2];
			verticesBackend[(ioffset + (gridLengthBackend + joffset) * gridLengthBackend)].m_CountVelocity = verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity;
			verticesBackend[(ioffset + (gridLengthBackend + joffset) * gridLengthBackend)].m_Velocity = verticesBackend[(i + j * gridLengthBackend)].m_Velocity;
		}
		else {
			if (m_Player == glm::vec2(i, j * gridLengthBackend)) {
				m_Player = glm::vec2((i + ioffset), (gridLengthBackend + joffset) * gridLengthBackend);
			}
#ifdef DetectArrayError
			Error((i + ioffset + (gridLengthBackend + joffset) * gridLengthBackend), "(i + ioffset + (gridLengthBackend + joffset) * gridLengthBackend)", verticesBackend.size());
#endif
			verticesBackend[(i + ioffset + (gridLengthBackend + joffset) * gridLengthBackend)].m_Mass = verticesBackend[(i + j * gridLengthBackend)].m_Mass;
			verticesBackend[(i + ioffset + (gridLengthBackend + joffset) * gridLengthBackend)].m_Color[0] = verticesBackend[(i + j * gridLengthBackend)].m_Color[0];
			verticesBackend[(i + ioffset + (gridLengthBackend + joffset) * gridLengthBackend)].m_Color[1] = verticesBackend[(i + j * gridLengthBackend)].m_Color[1];
			verticesBackend[(i + ioffset + (gridLengthBackend + joffset) * gridLengthBackend)].m_Color[2] = verticesBackend[(i + j * gridLengthBackend)].m_Color[2];
			verticesBackend[(i + ioffset + (gridLengthBackend + joffset) * gridLengthBackend)].m_CountVelocity = verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity;
			verticesBackend[(i + ioffset + (gridLengthBackend + joffset) * gridLengthBackend)].m_Velocity = verticesBackend[(i + j * gridLengthBackend)].m_Velocity;
		}
	}
	else if (
		j + joffset > gridLengthBackend - 1) {
		if (
			i + ioffset < 0) {
			if (m_Player == glm::vec2(i, j * gridLengthBackend)) {
				m_Player = glm::vec2((gridLengthBackend + ioffset), (joffset) * gridLengthBackend);
			}
#ifdef DetectArrayError
			Error((gridLengthBackend + ioffset + (joffset) * gridLengthBackend), "(gridLengthBackend + ioffset + (joffset) * gridLengthBackend)11A", verticesBackend.size());
#endif
			verticesBackend[(gridLengthBackend + ioffset + (joffset) * gridLengthBackend)].m_Mass = verticesBackend[(i + j * gridLengthBackend)].m_Mass;
			verticesBackend[(gridLengthBackend + ioffset + (joffset) * gridLengthBackend)].m_Color[0] = verticesBackend[(i + j * gridLengthBackend)].m_Color[0];
			verticesBackend[(gridLengthBackend + ioffset + (joffset) * gridLengthBackend)].m_Color[1] = verticesBackend[(i + j * gridLengthBackend)].m_Color[1];
			verticesBackend[(gridLengthBackend + ioffset + (joffset) * gridLengthBackend)].m_Color[2] = verticesBackend[(i + j * gridLengthBackend)].m_Color[2];
			verticesBackend[(gridLengthBackend + ioffset + (joffset) * gridLengthBackend)].m_CountVelocity = verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity;
			verticesBackend[(gridLengthBackend + ioffset + (joffset) * gridLengthBackend)].m_Velocity = verticesBackend[(i + j * gridLengthBackend)].m_Velocity;
		}
		else if (
			i + ioffset > gridLengthBackend - 1) {
			if (m_Player == glm::vec2(i, j * gridLengthBackend)) {
				m_Player = glm::vec2((ioffset), (joffset) * gridLengthBackend);
			}
		#ifdef DetectArrayError
			Error((ioffset + (joffset) * gridLengthBackend), "(ioffset + (joffset) * gridLengthBackend)11B", verticesBackend.size());
		#endif
			verticesBackend[(ioffset + (joffset) * gridLengthBackend)].m_Mass = verticesBackend[(i + j * gridLengthBackend)].m_Mass;
			verticesBackend[(ioffset + (joffset) * gridLengthBackend)].m_Color[0] = verticesBackend[(i + j * gridLengthBackend)].m_Color[0];
			verticesBackend[(ioffset + (joffset) * gridLengthBackend)].m_Color[1] = verticesBackend[(i + j * gridLengthBackend)].m_Color[1];
			verticesBackend[(ioffset + (joffset) * gridLengthBackend)].m_Color[2] = verticesBackend[(i + j * gridLengthBackend)].m_Color[2];
			verticesBackend[(ioffset + (joffset) * gridLengthBackend)].m_CountVelocity = verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity;
			verticesBackend[(ioffset + (joffset) * gridLengthBackend)].m_Velocity = verticesBackend[(i + j * gridLengthBackend)].m_Velocity;
		}
		else {
			if (m_Player == glm::vec2(i, j * gridLengthBackend)) {
				m_Player = glm::vec2((i + ioffset), (joffset) * gridLengthBackend);
			}
		#ifdef DetectArrayError
			Error(i + ioffset + (joffset)*gridLengthBackend, "i + ioffset + (joffset) * gridLengthBackend", verticesBackend.size());
		#endif
			verticesBackend[(i + ioffset + (joffset) * gridLengthBackend)].m_Mass = verticesBackend[(i + j * gridLengthBackend)].m_Mass;
			verticesBackend[(i + ioffset + (joffset) * gridLengthBackend)].m_Color[0] = verticesBackend[(i + j * gridLengthBackend)].m_Color[0];
			verticesBackend[(i + ioffset + (joffset) * gridLengthBackend)].m_Color[1] = verticesBackend[(i + j * gridLengthBackend)].m_Color[1];
			verticesBackend[(i + ioffset + (joffset) * gridLengthBackend)].m_Color[2] = verticesBackend[(i + j * gridLengthBackend)].m_Color[2];
			verticesBackend[(i + ioffset + (joffset) * gridLengthBackend)].m_CountVelocity = verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity;
			verticesBackend[(i + ioffset + (joffset) * gridLengthBackend)].m_Velocity = verticesBackend[(i + j * gridLengthBackend)].m_Velocity;
		}
	}
	else if (
		i + ioffset < 0) {
			if (m_Player == glm::vec2(i, j * gridLengthBackend)) {
				m_Player = glm::vec2((gridLengthBackend + ioffset), (j + joffset) * gridLengthBackend);
			}
			#ifdef DetectArrayError
				Error((gridLengthBackend + ioffset + (j + joffset) * gridLengthBackend), "(gridLengthBackend + ioffset + (j + joffset) * gridLengthBackend)12", verticesBackend.size());
			#endif
			verticesBackend[(gridLengthBackend + ioffset + (j + joffset) * gridLengthBackend)].m_Mass			= verticesBackend[(i + j * gridLengthBackend)].m_Mass;
			verticesBackend[(gridLengthBackend + ioffset + (j + joffset) * gridLengthBackend)].m_Color[0]		= verticesBackend[(i + j * gridLengthBackend)].m_Color[0];
			verticesBackend[(gridLengthBackend + ioffset + (j + joffset) * gridLengthBackend)].m_Color[1]		= verticesBackend[(i + j * gridLengthBackend)].m_Color[1];
			verticesBackend[(gridLengthBackend + ioffset + (j + joffset) * gridLengthBackend)].m_Color[2]		= verticesBackend[(i + j * gridLengthBackend)].m_Color[2];
			verticesBackend[(gridLengthBackend + ioffset + (j + joffset) * gridLengthBackend)].m_CountVelocity	= verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity;
			verticesBackend[(gridLengthBackend + ioffset + (j + joffset) * gridLengthBackend)].m_Velocity		= verticesBackend[(i + j * gridLengthBackend)].m_Velocity;
		}
	else if (
			i + ioffset > gridLengthBackend - 1) {
				if (m_Player == glm::vec2(i, j * gridLengthBackend)) {
					m_Player = glm::vec2((ioffset), (j + joffset) * gridLengthBackend);
				}
		#ifdef DetectArrayError
				Error((ioffset + (j + joffset) * gridLengthBackend), "(ioffset + (j + joffset) * gridLengthBackend)13", verticesBackend.size());
		#endif
		verticesBackend[(ioffset + (j + joffset) * gridLengthBackend)].m_Mass = verticesBackend[(i + j * gridLengthBackend)].m_Mass;
		verticesBackend[(ioffset + (j + joffset) * gridLengthBackend)].m_Color[0] = verticesBackend[(i + j * gridLengthBackend)].m_Color[0];
		verticesBackend[(ioffset + (j + joffset) * gridLengthBackend)].m_Color[1] = verticesBackend[(i + j * gridLengthBackend)].m_Color[1];
		verticesBackend[(ioffset + (j + joffset) * gridLengthBackend)].m_Color[2] = verticesBackend[(i + j * gridLengthBackend)].m_Color[2];
		verticesBackend[(ioffset + (j + joffset) * gridLengthBackend)].m_CountVelocity = verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity;
		verticesBackend[(ioffset + (j + joffset) * gridLengthBackend)].m_Velocity = verticesBackend[(i + j * gridLengthBackend)].m_Velocity;
	}
	else {
		if (m_Player == glm::vec2(i, j * gridLengthBackend)) {
			m_Player = glm::vec2((i + ioffset), (j + joffset) * gridLengthBackend);
		}
		verticesBackend[(i + ioffset + (j + joffset) * gridLengthBackend)].m_Mass = verticesBackend[(i + j * gridLengthBackend)].m_Mass;
		verticesBackend[(i + ioffset + (j + joffset) * gridLengthBackend)].m_Color[0] = verticesBackend[(i + j * gridLengthBackend)].m_Color[0];
		verticesBackend[(i + ioffset + (j + joffset) * gridLengthBackend)].m_Color[1] = verticesBackend[(i + j * gridLengthBackend)].m_Color[1];
		verticesBackend[(i + ioffset + (j + joffset) * gridLengthBackend)].m_Color[2] = verticesBackend[(i + j * gridLengthBackend)].m_Color[2];
		verticesBackend[(i + ioffset + (j + joffset) * gridLengthBackend)].m_CountVelocity = verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity;
		verticesBackend[(i + ioffset + (j + joffset) * gridLengthBackend)].m_Velocity = verticesBackend[(i + j * gridLengthBackend)].m_Velocity;
	}		
		#ifdef DetectArrayError
			Error((i + j * gridLengthBackend), "(i + j * gridLengthBackend)", verticesBackend.size());
		#endif
		verticesBackend[(i + j * gridLengthBackend)].m_Mass = 0;

		verticesBackend[(i + j * gridLengthBackend)].m_Color[0] = 1;
		verticesBackend[(i + j * gridLengthBackend)].m_Color[1] = 1;
		verticesBackend[(i + j * gridLengthBackend)].m_Color[2] = 1;

		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity = glm::vec2(0, 0);
		verticesBackend[(i + j * gridLengthBackend)].m_Velocity = glm::vec2(0,0);
}

void SandBox::SetVerticesMomentum(int& i, int& j, int i1, int j1) {
#ifdef DetectArrayError
	Error((i1 + j1 * gridLengthBackend), "(i1 + j1 * gridLengthBackend)", verticesBackend.size());
	Error((i + j * gridLengthBackend), "(i + j * gridLengthBackend)", verticesBackend.size());
#endif
	verticesBackend[(i1 + j1 * gridLengthBackend)].m_CountVelocity = verticesBackend[(i + j * gridLengthBackend)].m_Mass * verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity / verticesBackend[(i1 + j1 * gridLengthBackend)].m_Mass;
	verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity = glm::vec2(0);
	verticesBackend[(i1 + j1 * gridLengthBackend)].m_Velocity = verticesBackend[(i + j * gridLengthBackend)].m_Mass * verticesBackend[(i + j * gridLengthBackend)].m_Velocity / verticesBackend[(i1 + j1 * gridLengthBackend)].m_Mass;
	//verticesBackend[(i1 + j1 * gridLengthBackend)].m_Velocity += verticesBackend[(i + j * gridLengthBackend)].m_Velocity;
	verticesBackend[(i + j * gridLengthBackend)].m_Velocity = glm::vec2(0);
}

void SandBox::Vertices1MovementLogic(int i, int j, int ip1, int im1, int jp1, int jm1, float deltaTime) {
#ifdef DetectArrayError
	Error((i + j * gridLengthBackend), "(i + j * gridLengthBackend)", verticesBackend.size());
	Error((ip1 + j * gridLengthBackend), "(ip1 + j * gridLengthBackend)", verticesBackend.size());
	Error((im1 + j * gridLengthBackend), "(im1 + j * gridLengthBackend)", verticesBackend.size());
	Error((i + (jp1) * gridLengthBackend), "(i + (jp1) * gridLengthBackend)", verticesBackend.size());
	Error((i + (jm1) * gridLengthBackend), "(i + (jm1) * gridLengthBackend)", verticesBackend.size());
	Error((ip1 + (jp1) * gridLengthBackend), "(ip1 + (jp1) * gridLengthBackend)", verticesBackend.size());
	Error((ip1 + (jm1) * gridLengthBackend), "(ip1 + (jm1) * gridLengthBackend)", verticesBackend.size());
	Error((im1 + (jp1) * gridLengthBackend), "(im1 + (jp1) * gridLengthBackend)", verticesBackend.size());
	Error((im1 + (jm1) * gridLengthBackend), "(im1 + (jm1) * gridLengthBackend)", verticesBackend.size());
#endif
	verticesBackend[(i + j * gridLengthBackend)].m_Wave
		= (verticesBackend[(i + j * gridLengthBackend)].m_Wave
			+ verticesBackend[(ip1 + j * gridLengthBackend)].m_Wave
			+ verticesBackend[(im1 + j * gridLengthBackend)].m_Wave
			+ verticesBackend[(i + (jp1) * gridLengthBackend)].m_Wave
			+ verticesBackend[(i + (jm1) * gridLengthBackend)].m_Wave
			+ verticesBackend[(ip1 + (jp1) * gridLengthBackend)].m_Wave
			+ verticesBackend[(ip1 + (jm1) * gridLengthBackend)].m_Wave
			+ verticesBackend[(im1 + (jp1) * gridLengthBackend)].m_Wave
			+ verticesBackend[(im1 + (jm1) * gridLengthBackend)].m_Wave) * dispersion
		+ verticesBackend[(i + j * gridLengthBackend)].m_Mass * mDispersion;
	//std::cout << "Wave "<< i << " " << j << " " << vertices1[(i + j * gridLengthBackend) ].m_Wave << " " << vertices1[(i + j * gridLengthBackend) ].m_Mass << std::endl;
	verticesBackend[(i + j * gridLengthBackend)].m_Color[3] = tanh(verticesBackend[(i + j * gridLengthBackend)].m_Wave * intensity);
	//std::cout << "Color " << i << " " << j << " " << verticesBackend[(i + j * gridLengthBackend)].m_Color[3] << std::endl;
#ifdef DetectArrayError
	Error((i + j * gridLengthBackend), "(i + j * gridLengthBackend)", verticesBackend.size());
#endif
	if (verticesBackend[(i + j * gridLengthBackend)].m_Mass > 0) {
		accel = glm::vec2(
		verticesBackend[(ip1 + j * gridLengthBackend)].m_Wave
		- verticesBackend[(im1 + j * gridLengthBackend)].m_Wave 
		+ (
			-verticesBackend[(ip1 + j * gridLengthBackend)].m_Mass
			+ verticesBackend[(im1 + j * gridLengthBackend)].m_Mass) * repulse
		+ sqrtTwoOTwo * (
			verticesBackend[(ip1 + (jp1) * gridLengthBackend)].m_Wave
			+ verticesBackend[(ip1 + (jm1) * gridLengthBackend)].m_Wave
			- verticesBackend[(im1 + (jp1) * gridLengthBackend)].m_Wave
			- verticesBackend[(im1 + (jm1) * gridLengthBackend)].m_Wave
			+ (- verticesBackend[(ip1 + (jp1) * gridLengthBackend)].m_Mass
				- verticesBackend[(ip1 + (jm1) * gridLengthBackend)].m_Mass
				+ verticesBackend[(im1 + (jp1) * gridLengthBackend)].m_Mass
				+ verticesBackend[(im1 + (jm1) * gridLengthBackend)].m_Mass) * repulse
			)
		,
		verticesBackend[(i + (jp1) * gridLengthBackend)].m_Wave
		- verticesBackend[(i + (jm1) * gridLengthBackend)].m_Wave
		+ (
			- verticesBackend[(i + (jp1) * gridLengthBackend)].m_Mass
			+ verticesBackend[(i + (jm1) * gridLengthBackend)].m_Mass) * repulse
		+ sqrtTwoOTwo * (
			verticesBackend[(ip1 + (jp1) * gridLengthBackend)].m_Wave
			+ verticesBackend[(im1 + (jp1) * gridLengthBackend)].m_Wave
			- verticesBackend[(ip1 + (jm1) * gridLengthBackend)].m_Wave
			- verticesBackend[(im1 + (jm1) * gridLengthBackend)].m_Wave
			+ (- verticesBackend[(ip1 + (jp1) * gridLengthBackend)].m_Mass
				- verticesBackend[(im1 + (jp1) * gridLengthBackend)].m_Mass
				+ verticesBackend[(ip1 + (jm1) * gridLengthBackend)].m_Mass
				+ verticesBackend[(im1 + (jm1) * gridLengthBackend)].m_Mass) * repulse
			)
	);
#ifdef DetectArrayError
		Error((i + j * gridLengthBackend), "(i + j * gridLengthBackend)", verticesBackend.size());
#endif
		verticesBackend[(i + j * gridLengthBackend)].m_Velocity += accel * deltaTime;
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity += verticesBackend[(i + j * gridLengthBackend)].m_Velocity * deltaTime;
	}

	

	if (
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[0] > sqrtTwoOTwo
		&& verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[1] > sqrtTwoOTwo) {
		if (verticesBackend[(ip1 + jp1 * gridLengthBackend)].m_Mass == 0) {
			SetVerticesMovement(i, j, 1, 1);
		}
		else {
			SetVerticesMomentum(i, j, ip1, jp1);
		}
	}
	else if (
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[0] > sqrtTwoOTwo
		&& verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[1] < -sqrtTwoOTwo) {
		if (verticesBackend[(ip1 + jm1 * gridLengthBackend)].m_Mass == 0) {
			SetVerticesMovement(i, j, 1, -1);
		}
		else {
			SetVerticesMomentum(i, j, ip1, jm1);
		}
	}
	else if (
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[0] < -sqrtTwoOTwo
		&& verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[1] > sqrtTwoOTwo) {
		if (verticesBackend[(im1 + jp1 * gridLengthBackend)].m_Mass == 0) {
			SetVerticesMovement(i, j, -1, 1);
		}
		else {
			SetVerticesMomentum(i, j, im1, jp1);
		}
	}
	else if (
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[0] < -sqrtTwoOTwo
		&& verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[1] < -sqrtTwoOTwo) {
		if (verticesBackend[(im1 + jm1 * gridLengthBackend)].m_Mass == 0) {
			SetVerticesMovement(i, j, -1, -1);
		}
		else {
			SetVerticesMomentum(i, j, ip1, jp1);
		}
	}
	else if (
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[0] < -1) {
		if(verticesBackend[(im1 + j * gridLengthBackend)].m_Mass == 0) {
			SetVerticesMovement(i, j, -1, 0);
		}
		else {
			SetVerticesMomentum(i, j, im1, j);
		}
	}
	else if (
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[0] > 1) {
		if (verticesBackend[(ip1 + j * gridLengthBackend)].m_Mass == 0) {
			SetVerticesMovement(i, j, 1, 0);
		}
		else {
			SetVerticesMomentum(i, j, ip1, j);
		}
	}

	else if (
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[1] < -1) {
		if (verticesBackend[(i + jm1 * gridLengthBackend)].m_Mass == 0) {
			SetVerticesMovement(i, j, 0, -1);
		}
		else {
			SetVerticesMomentum(i, j, i, jm1);
		}
	}
	//Vertices1MovementLogic(i, gridLengthBackend - 1, i + 1, i - 1, 0, gridLengthBackend - 2, deltaTime);
	else if (
		verticesBackend[(i + j * gridLengthBackend)].m_CountVelocity[1] > 1) {
		if (verticesBackend[(i + jp1 * gridLengthBackend)].m_Mass == 0) {
			SetVerticesMovement(i, j, 0, 1);
		}
		else {
			SetVerticesMomentum(i, j, i, jp1);
		}
	}
}

void SandBox::OnRender()
{
	GLCall(glClearColor(0.6f, 0.6f, 0.6f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
	
	OnUpdate(0.05f);
	//FollowPlayer();

	m_IndexBuffer = std::make_unique<IndexBuffer>(&index[0], index.size());
	m_VertexBuffer = std::make_unique<VertexBuffer>(&vertices[0], sizeof(Vertex) * vertices.size());
	m_VAO->AddBuffer(*m_VertexBuffer, *m_Layout);

	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
		glm::mat4 mvp = m_Proj * m_View * model;
		m_Shader->Bind();
		m_Shader->SetUniformMat4f("u_MVP", mvp);
		m_Renderer->Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
	}
#ifdef ImGui_DEBUG
	SandBox::OnImGuiRender();
#endif // ImGui_DEBUG
}

void SandBox::OnImGuiRender() {
#ifdef ImGui_DEBUG
	ImGui::Begin("Quad");
	ImGui::Text("Hi");
	ImGui::SliderFloat("Well", &well, 0.0f, 400);
	ImGui::SliderFloat("mDispersion", &mDispersion, 0.0f, 5);
	ImGui::SliderFloat("Repulse", &repulse, 0.0f, 100);
	ImGui::SliderFloat2("VelocityCount", &verticesBackend[m_Player[0] + m_Player[1]].m_CountVelocity[0], 0, 100);
	ImGui::SliderFloat2("Velocity", &verticesBackend[m_Player[0] + m_Player[1]].m_Velocity[0], 0, 100);
	ImGui::End();
#endif // ImGui_DEBUG
}

void SandBox::PlayerMovementW() {
	if (m_Player[0] + 1 > (gridLengthBackend - 1)) {
#ifdef DetectArrayError
		Error(m_Player[0] + m_Player[1] - (gridLengthBackend)+1, "m_Player[0] + m_Player[1] - (gridLengthBackend)+1", verticesBackend.size());
#endif
		verticesBackend[m_Player[0] + m_Player[1] - (gridLengthBackend) + 1].m_Wave += well;
	}
	else {
#ifdef DetectArrayError
		Error(m_Player[0] + m_Player[1] + 1, "m_Player[0] + m_Player[1] + 1", verticesBackend.size());
#endif
		verticesBackend[m_Player[0] + m_Player[1] + 1].m_Wave += well;
	} 
	if (m_Player[0] - 1 < 0) {
#ifdef DetectArrayError
		Error(m_Player[0] + m_Player[1] + (gridLengthBackend)-1, "m_Player[0] + m_Player[1] + (gridLengthBackend)-1", verticesBackend.size());
#endif
		verticesBackend[m_Player[0] + m_Player[1] + (gridLengthBackend) - 1].m_Wave -= well;
	}
	else {
#ifdef DetectArrayError
		Error(m_Player[0] + m_Player[1] - 1, "m_Player[0] + m_Player[1] - 1", verticesBackend.size());
#endif
		verticesBackend[m_Player[0] + m_Player[1] - 1].m_Wave -= well;
	}
}

void SandBox::PlayerMovementA() {
	if (m_Player[1] + (2 * gridLengthBackend) > (gridLengthBackend - 1) * gridLengthBackend) {
#ifdef DetectArrayError
		Error(m_Player[0] + m_Player[1] + (2 * gridLengthBackend) - (gridLengthBackend - 1) * (gridLengthBackend), "m_Player[0] + m_Player[1] + (2 * gridLengthBackend) - (gridLengthBackend - 1) * (gridLengthBackend)", verticesBackend.size());
#endif
		verticesBackend[m_Player[0] + m_Player[1] + (2 * gridLengthBackend) - (gridLengthBackend - 1) * (gridLengthBackend)].m_Wave -= well;
	}
	else 
	{
#ifdef DetectArrayError
		Error(m_Player[0] + m_Player[1] + (2 * gridLengthBackend), "m_Player[0] + m_Player[1] + (2 * gridLengthBackend)", verticesBackend.size());
#endif
		verticesBackend[m_Player[0] + m_Player[1] + (2 * gridLengthBackend)].m_Wave -= well;
	}
	if (m_Player[1] - (gridLengthBackend)  < 0) {
#ifdef DetectArrayError
		Error(m_Player[0] + m_Player[1] - (gridLengthBackend) + (gridLengthBackend), "m_Player[0] + m_Player[1] - (gridLengthBackend) + (gridLengthBackend)", verticesBackend.size());
#endif
		verticesBackend[m_Player[0] + m_Player[1] - (gridLengthBackend) + (gridLengthBackend)].m_Wave += well;
	}
	else {
#ifdef DetectArrayError
		Error(m_Player[0] + m_Player[1] - (gridLengthBackend), "m_Player[0] + m_Player[1] - (gridLengthBackend)", verticesBackend.size());
#endif
		verticesBackend[m_Player[0] + m_Player[1] - (gridLengthBackend)].m_Wave += well;
	}
	
}

void SandBox::PlayerMovementS() {
	if (m_Player[0] + 2 > (gridLengthBackend - 1)) {
#ifdef DetectArrayError
		Error(m_Player[0] + m_Player[1] - (gridLengthBackend) + 2, "m_Player[0] + m_Player[1] - (gridLengthBackend) + 2", verticesBackend.size());
#endif
		verticesBackend[m_Player[0] + m_Player[1] - (gridLengthBackend) + 2].m_Wave -= well;
	}
	else
	{
#ifdef DetectArrayError
		Error(m_Player[0] + m_Player[1] + 2, "m_Player[0] + m_Player[1] + 2", verticesBackend.size());
#endif
		verticesBackend[m_Player[0] + m_Player[1] + 2].m_Wave -= well;
	}

	if (m_Player[0] - 1 < 0) {
#ifdef DetectArrayError
		Error(m_Player[0] + m_Player[1] + (gridLengthBackend)-1, "m_Player[0] + m_Player[1] + (gridLengthBackend)-1", verticesBackend.size());
#endif
		verticesBackend[m_Player[0] + m_Player[1] + (gridLengthBackend) - 1].m_Wave += well;
	}
	else {
#ifdef DetectArrayError
		Error(m_Player[0] + m_Player[1] - 1, "m_Player[0] + m_Player[1] - 1", verticesBackend.size());
#endif
		verticesBackend[m_Player[0] + m_Player[1] - 1].m_Wave += well;
	}
}

void SandBox::PlayerMovementD() {
	//vertices1[m_Player[0] + m_Player[1]].CountVelocity[1] += 0.3;
	if (m_Player[1] - (gridLengthBackend)  < 0) {
		//std::cout <<  << std::endl;
#ifdef DetectArrayError
		Error(m_Player[0] + m_Player[1], "m_Player[0] + m_Player[1]", verticesBackend.size());
#endif
		verticesBackend[m_Player[0] + m_Player[1]].m_Wave -= well;
	}
	else {
#ifdef DetectArrayError
		Error(m_Player[0] + m_Player[1] - (gridLengthBackend), "m_Player[0] + m_Player[1] - (gridLengthBackend)", verticesBackend.size());
#endif
		verticesBackend[m_Player[0] + m_Player[1] - (gridLengthBackend)].m_Wave -= well;
	}
	if (m_Player[1] + (gridLengthBackend) > (gridLengthBackend) * (gridLengthBackend-1)) {
#ifdef DetectArrayError
		Error(m_Player[0] + m_Player[1] + (gridLengthBackend)-(gridLengthBackend - 1) * (gridLengthBackend), "m_Player[0] + m_Player[1] + (gridLengthBackend)-(gridLengthBackend - 1) * (gridLengthBackend)", verticesBackend.size());
#endif
		verticesBackend[m_Player[0] + m_Player[1] + (gridLengthBackend) - (gridLengthBackend - 1) * (gridLengthBackend)].m_Wave += well;
	}
	else {
#ifdef DetectArrayError
		Error(m_Player[0] + m_Player[1] + (gridLengthBackend), "m_Player[0] + m_Player[1] + (gridLengthBackend)", verticesBackend.size());
#endif
		verticesBackend[m_Player[0] + m_Player[1] + (gridLengthBackend)].m_Wave += well;
		//vertices1[m_Player[0] + m_Player[1] + (gridLengthBackend) ].m_Color = glm::vec4(1, 0, 0, 1);
	}
	
}

void SandBox::AddQuad(float x, float y, glm::vec4 rgba)
{
	/*
	* TO-DO
	Performance check
	Push and add to "vertices" or create 4 vertices and push "vertices".

	use vertexCount or a vertex/index reference
	*/
	//view index memory in 4 coloums
	index.emplace_back(0 + vertexCount);
	index.emplace_back(1 + vertexCount);
	index.emplace_back(2 + vertexCount);
	index.emplace_back(2 + vertexCount);
	index.emplace_back(3 + vertexCount);
	index.emplace_back(0 + vertexCount);

	vertices.emplace_back(Vertex(glm::vec2(			  x,			   y), glm::vec4(rgba)));
	vertices.emplace_back(Vertex(glm::vec2( sideLength + x,			   y), glm::vec4(rgba)));
	vertices.emplace_back(Vertex(glm::vec2( sideLength + x,  sideLength + y), glm::vec4(rgba)));
	vertices.emplace_back(Vertex(glm::vec2(			  x,  sideLength + y), glm::vec4(rgba)));

	vertexCount += 4;
}

void SandBox::AddQuadBackend(float mass, glm::vec4 rgba) 
{
	verticesBackend.emplace_back(VertexBackend(glm::vec4(rgba), mass, mass, glm::vec2(0, 0), glm::vec2(0, 0)));
}

void SandBox::SetRenderer(Renderer* m_Renderer) {
	m_Renderer = m_Renderer;
}

void SandBox::SetTranslation(glm::vec3 m_Translate) {
	m_Translation += m_Translate;
}

void SandBox::FollowPlayer() {
	//vertices1[m_Player[0] + m_Player[1]].m_Position
	//m_Translation += (glm::vec3(-vertices1[m_Player[0] + m_Player[1]].m_Position.x + Window_Width * 0.5, -vertices1[m_Player[0] + m_Player[1]].m_Position.y + Window_Height * 0.5, 0)
	//	- m_Translation) * glm::vec3(0.02, 0.02, 0.02);
	//m_Translation = glm::vec3(-vertices1[m_Player[0] + m_Player[1]].m_Position.x + Window_Width* 0.5, -vertices1[m_Player[0] + m_Player[1]].m_Position.y + Window_Height * 0.5, 0);
	//m_Translation += glm::vec3(-(m_Player[1]/gridLengthBackend* sideLength - 500), -(m_Player[0] * sideLength-500), 0);
	m_Player_Cam += glm::vec2(m_Translation[1], m_Translation[0]) * glm::vec2(0.1);
	m_Translation = (glm::vec3((m_Player[1] / gridLengthBackend - m_Player_Cam[1]) * sideLength
		, (m_Player[0] - m_Player_Cam[0]) * sideLength, 0));
	//m_Translation += glm::vec3(-vertices[m_Player[0] + m_Player[1]].m_Position.x + Window_Width * 0.5
	//	, -vertices[m_Player[0] + m_Player[1]].m_Position.y + Window_Height * 0.5, 0);
	//std::cout << m_Player[0] << " " << m_Player[1] << " " << m_Translation.x << " " << m_Translation.y << " " << vertices[m_Player[0] + m_Player[1]].m_Position.x << " " << vertices[m_Player[0] + m_Player[1]].m_Position.y << std::endl;
	std::cout << m_Player_Cam[0] << " " << m_Player_Cam[1] << " " << m_Translation.x << " " << m_Translation.y << std::endl;
}

void SandBox::Error(int a, const char b[], int size) {
#ifdef DetectArrayError
	if (a < 0 ||
		a > size){
		std::cout << "Error: " << b << " "
			<< a << " " 
			<< size
			<< std::endl;
	}
#endif
}