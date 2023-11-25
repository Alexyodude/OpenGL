#include "SandBox.h"

SandBox::SandBox()
	: m_Proj(glm::ortho(0.0f, (float)Window_Width, 0.0f, (float)Window_Height, -1.0f, 1.0f)),
	m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
	m_Translation(0, 0, 0), m_Renderer(), m_Layout(new VertexBufferLayout()), m_Player(0, 0)
{
	//GLCall(glEnable(GL_BLEND));
	//GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	m_VAO = std::make_unique<VertexArray>();

	m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
	m_Shader->Bind();
	//m_Shader->SetUniform4f("u_Color", 0.7f, 0.6f, 0.5f, 1.0f);

	m_Layout->Push<float>(2);
	m_Layout->Push<float>(4);
	m_Layout->Push<float>(1);
	m_Layout->Push<float>(1);
	m_Layout->Push<float>(2);
	m_Layout->Push<float>(2);

	for (int i = 0; i < gridLength; i++) {
		tileSetSL[i] = i * (sideLength + gap);
		tileSetNL[i] = i * (gap);
		//tileSetTL[i][2] = -sideLength + x, -sideLength + i * (2 * sideLength + 5);
		//tileSetTL[i][1]
	}
	//float testMass[10][10] = {
	//	  {0, 1, 1, 0, 1, 0, 1, 1, 1, 0}
	//	, {0, 1, 1, 1, 1, 0, 0, 1, 0, 1}
	//	, {0, 0, 1, 0, 0, 1, 1, 1, 0, 0}
	//	, {1, 1, 0, 0, 0, 0, 0, 0, 0, 1}
	//	, {0, 0, 0, 0, 1, 1, 0, 0, 0, 0}
	//	, {1, 0, 0, 0, 1, 1, 0, 0, 0, 0}
	//	, {1, 0, 0, 0, 1, 0, 0, 0, 0, 0}
	//	, {0, 0, 0, 0, 1, 1, 0, 0, 0, 0}
	//	, {0, 1, 1, 1, 1, 1, 1, 1, 1, 1}
	//	, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0} };
	for (int i = 0; i < gridLength; i++) {
		for (int j = 0; j < gridLength; j++) {
			//SandBox::AddQuad(tileSetSL[i], tileSetSL[j], glm::vec4(std::rand() / (float)RAND_MAX, std::rand() / (float)RAND_MAX, std::rand() / (float)RAND_MAX, 1.0f));
			float mass = std::rand() / (float)RAND_MAX-0.9;
			//SandBox::AddQuad(tileSetSL[i], tileSetSL[j], testMass[j][i], glm::vec4(1.0f, 1.0f, 1.0f, testMass[j][i]));
			if (i - 10 < (int)gridLength * 0.5 && j - 10 < (int)gridLength * 0.5
				&& i + 10 > (int)gridLength * 0.5 && j + 10 > (int)gridLength * 0.5) {
				if (i == (int)gridLength * 0.5 && j == (int)gridLength * 0.5) {
					m_Player = glm::vec2(i*4, j* gridLength*4);
					SandBox::AddQuad(tileSetSL[i], tileSetSL[j], 1, glm::vec4(0.8, 0.4, 0.2, 1.0f));
					std::cout << "i: " << i << " j: " << j << std::endl;
				}
				else {
					SandBox::AddQuad(tileSetSL[i], tileSetSL[j], 0, glm::vec4(1.0f, 1.0f, 1.0f, 0));
				}
			}else if (mass <= 0) {
				SandBox::AddQuad(tileSetSL[i], tileSetSL[j], 0, glm::vec4(1.0f, 1.0f, 1.0f, 0));
			}else if (mass > 0) {
				SandBox::AddQuad(tileSetSL[i], tileSetSL[j], 1, glm::vec4(mass*10, 1, mass * 10, 1.0f));
			}
			
			//vertMat[i][j] = &(vertices1[4 * (i + gridLength * j)]);
		}
	}
}

SandBox::~SandBox()
{
}

void SandBox::OnUpdate(float deltaTime)
{
	/*
	|	|	|
	|	|	|
	|	|	0
	*/
	vertices1[(gridLength - 1) * gridLength * 4].m_Wave
		= ( vertices1[(gridLength - 1) * gridLength * 4].m_Wave
			+ vertices1[(gridLength - 2) * gridLength * 4].m_Wave
			+ vertices1[1 + (gridLength-2) * gridLength * 4].m_Wave
			+ vertices1[1 + (gridLength-1) * gridLength * 4].m_Wave) * dispersion
		+ vertices1[(gridLength - 1) * gridLength * 4].m_Mass * mDispersion;
	vertices1[(gridLength - 1) * gridLength * 4	   ].m_Color[3] = vertices1[(gridLength - 1) * gridLength * 4].m_Wave;
	vertices1[(gridLength - 1) * gridLength * 4 + 1].m_Color[3] = vertices1[(gridLength - 1) * gridLength * 4].m_Wave;
	vertices1[(gridLength - 1) * gridLength * 4 + 2].m_Color[3] = vertices1[(gridLength - 1) * gridLength * 4].m_Wave;
	vertices1[(gridLength - 1) * gridLength * 4 + 3].m_Color[3] = vertices1[(gridLength - 1) * gridLength * 4].m_Wave;

	/*
	|	|	0
	|	|	|
	|	|	|
	*/
	vertices1[(1 + gridLength) * (gridLength - 1) * 4].m_Wave
		= (vertices1[(1 + gridLength) * (gridLength - 1) * 4].m_Wave
			+ vertices1[((gridLength - 2) + (gridLength-1) * gridLength) * 4].m_Wave
			+ vertices1[((gridLength - 2) + (gridLength - 2) * gridLength) * 4].m_Wave
			+ vertices1[((gridLength - 1) + (gridLength - 2) * gridLength) * 4].m_Wave) * dispersion
		+ vertices1[(1 + gridLength) * (gridLength - 1) * 4].m_Mass * mDispersion;
	vertices1[(1 + gridLength) * (gridLength - 1) * 4	 ].m_Color[3] = vertices1[(1 + gridLength) * (gridLength - 1) * 4].m_Wave;
	vertices1[(1 + gridLength) * (gridLength - 1) * 4 + 1].m_Color[3] = vertices1[(1 + gridLength) * (gridLength - 1) * 4].m_Wave;
	vertices1[(1 + gridLength) * (gridLength - 1) * 4 + 2].m_Color[3] = vertices1[(1 + gridLength) * (gridLength - 1) * 4].m_Wave;
	vertices1[(1 + gridLength) * (gridLength - 1) * 4 + 3].m_Color[3] = vertices1[(1 + gridLength) * (gridLength - 1) * 4].m_Wave;

	/*
	|	|	|
	|	|	|
	0	|	|
	*/
	vertices1[0].m_Wave
		= (vertices1[0].m_Wave
			+ vertices1[gridLength * 4].m_Wave
			+ vertices1[(1 + gridLength) * 4].m_Wave
			+ vertices1[4].m_Wave) * dispersion
		+ vertices1[0].m_Mass * mDispersion;
	vertices1[0].m_Color[3] = vertices1[0].m_Wave;
	vertices1[1].m_Color[3] = vertices1[0].m_Wave;
	vertices1[2].m_Color[3] = vertices1[0].m_Wave;
	vertices1[3].m_Color[3] = vertices1[0].m_Wave;
	
	/*
	0	|	|
	|	|	|
	|	|	|
	*/
	vertices1[(gridLength - 1) * 4].m_Wave
		= (vertices1[(gridLength - 1) * 4].m_Wave
			+ vertices1[(gridLength - 2) * 4].m_Wave
			+ vertices1[((gridLength - 2) + gridLength) * 4].m_Wave
			+ vertices1[((gridLength - 1) + gridLength) * 4].m_Wave) * dispersion
		+ vertices1[(gridLength - 1) * 4].m_Mass * mDispersion;
	vertices1[(gridLength - 1) * 4	  ].m_Color[3] = vertices1[(gridLength - 1) * 4].m_Wave;
	vertices1[(gridLength - 1) * 4 + 1].m_Color[3] = vertices1[(gridLength - 1) * 4].m_Wave;
	vertices1[(gridLength - 1) * 4 + 2].m_Color[3] = vertices1[(gridLength - 1) * 4].m_Wave;
	vertices1[(gridLength - 1) * 4 + 3].m_Color[3] = vertices1[(gridLength - 1) * 4].m_Wave;

	for (int i = 1; i < gridLength-1; i++) {
		/*
		|	|	|
		0	|	|
		|	|	|
		*/
		vertices1[i * 4].m_Wave
			= (vertices1[i * 4].m_Wave
				+ vertices1[(i + gridLength) * 4].m_Wave
				+ vertices1[((i + 1) + gridLength) * 4].m_Wave
				+ vertices1[((i - 1) + gridLength) * 4].m_Wave
				+ vertices1[(i + 1) * 4].m_Wave
				+ vertices1[(i - 1) * 4].m_Wave) * dispersion
			+ vertices1[i * 4].m_Mass * mDispersion;
		vertices1[i * 4	   ].m_Color[3] = vertices1[i * 4].m_Wave;
		vertices1[i * 4 + 1].m_Color[3] = vertices1[i * 4].m_Wave;
		vertices1[i * 4 + 2].m_Color[3] = vertices1[i * 4].m_Wave;
		vertices1[i * 4 + 3].m_Color[3] = vertices1[i * 4].m_Wave;
		/*
		|	|	|
		|	|	0
		|	|	|
		*/
		vertices1[(i + (gridLength - 1) * gridLength) * 4].m_Wave
			= (vertices1[(i + (gridLength - 1) * gridLength) * 4].m_Wave
				+ vertices1[(i + (gridLength - 2) * gridLength) * 4].m_Wave
				+ vertices1[((i + 1) + (gridLength - 1) * gridLength) * 4].m_Wave
				+ vertices1[((i + 1) + (gridLength - 2) * gridLength) * 4].m_Wave
				+ vertices1[((i - 1) + (gridLength - 1) * gridLength) * 4].m_Wave
				+ vertices1[((i - 1) + (gridLength - 2) * gridLength) * 4].m_Wave) * dispersion
			+ vertices1[(i + (gridLength - 1) * gridLength) * 4].m_Mass * mDispersion;
		vertices1[(i + (gridLength - 1) * gridLength) * 4	 ].m_Color[3] = vertices1[(i + (gridLength - 1) * gridLength) * 4].m_Wave;
		vertices1[(i + (gridLength - 1) * gridLength) * 4 + 1].m_Color[3] = vertices1[(i + (gridLength - 1) * gridLength) * 4].m_Wave;
		vertices1[(i + (gridLength - 1) * gridLength) * 4 + 2].m_Color[3] = vertices1[(i + (gridLength - 1) * gridLength) * 4].m_Wave;
		vertices1[(i + (gridLength - 1) * gridLength) * 4 + 3].m_Color[3] = vertices1[(i + (gridLength - 1) * gridLength) * 4].m_Wave;
		/*
		|	|	|
		|	|	|
		|	0	|
		*/
		vertices1[(i * gridLength) * 4].m_Wave
			= (vertices1[(i * gridLength) * 4].m_Wave
				+ vertices1[(1 + i * gridLength) * 4].m_Wave
				+ vertices1[(1 + (i + 1) * gridLength) * 4].m_Wave
				+ vertices1[(1 + (i - 1) * gridLength) * 4].m_Wave
				+ vertices1[((i + 1) * gridLength) * 4].m_Wave
				+ vertices1[((i - 1) * gridLength) * 4].m_Wave) * dispersion
			+ vertices1[(i * gridLength) * 4].m_Mass * mDispersion;
		vertices1[(i * gridLength) * 4	  ].m_Color[3] = vertices1[(i * gridLength) * 4].m_Wave;
		vertices1[(i * gridLength) * 4 + 1].m_Color[3] = vertices1[(i * gridLength) * 4].m_Wave;
		vertices1[(i * gridLength) * 4 + 2].m_Color[3] = vertices1[(i * gridLength) * 4].m_Wave;
		vertices1[(i * gridLength) * 4 + 3].m_Color[3] = vertices1[(i * gridLength) * 4].m_Wave;
		/*
		|	0	|
		|	|	|
		|	|	|
		*/
		vertices1[(gridLength - 1 + i * gridLength) * 4].m_Wave
			= (vertices1[(gridLength - 1 + i * gridLength) * 4].m_Wave
				+ vertices1[(gridLength - 2 + i * gridLength) * 4].m_Wave
				+ vertices1[(gridLength - 2 + (i + 1) * gridLength) * 4].m_Wave
				+ vertices1[(gridLength - 2 + (i - 1) * gridLength) * 4].m_Wave
				+ vertices1[(gridLength - 1 + (i + 1) * gridLength) * 4].m_Wave
				+ vertices1[(gridLength - 1 + (i - 1) * gridLength) * 4].m_Wave) * dispersion
			+ vertices1[(gridLength - 1 + i * gridLength) * 4].m_Mass * mDispersion;
		vertices1[(gridLength - 1 + i * gridLength) * 4	   ].m_Color[3] = vertices1[(gridLength - 1 + i * gridLength) * 4].m_Wave;
		vertices1[(gridLength - 1 + i * gridLength) * 4 + 1].m_Color[3] = vertices1[(gridLength - 1 + i * gridLength) * 4].m_Wave;
		vertices1[(gridLength - 1 + i * gridLength) * 4 + 2].m_Color[3] = vertices1[(gridLength - 1 + i * gridLength) * 4].m_Wave;
		vertices1[(gridLength - 1 + i * gridLength) * 4 + 3].m_Color[3] = vertices1[(gridLength - 1 + i * gridLength) * 4].m_Wave;
		for (int j = 1; j < gridLength-1; j++) {
			/*
			|	|	|
			|	0	|
			|	|	|
			*/
			vertices1[(i + j * gridLength) * 4].m_Wave
				= (vertices1[(i + j * gridLength) * 4].m_Wave
					+ vertices1[(i + 1 + j * gridLength) * 4].m_Wave
					+ vertices1[(i - 1 + j * gridLength) * 4].m_Wave
					+ vertices1[(i + (j + 1) * gridLength) * 4].m_Wave
					+ vertices1[(i + (j - 1) * gridLength) * 4].m_Wave
					+ vertices1[(i + 1 + (j + 1) * gridLength) * 4].m_Wave
					+ vertices1[(i + 1 + (j - 1) * gridLength) * 4].m_Wave
					+ vertices1[(i - 1 + (j + 1) * gridLength) * 4].m_Wave
					+ vertices1[(i - 1 + (j - 1) * gridLength) * 4].m_Wave) * dispersion
				+ vertices1[(i + j * gridLength) * 4].m_Mass * mDispersion;
			//std::cout << "Wave "<< i << " " << j << " " << vertices1[(i + j * gridLength) * 4].m_Wave << " " << vertices1[(i + j * gridLength) * 4].m_Mass << std::endl;
			vertices1[(i + j * gridLength) * 4	  ].m_Color[3] = vertices1[(i + j * gridLength) * 4].m_Wave;
			vertices1[(i + j * gridLength) * 4 + 1].m_Color[3] = vertices1[(i + j * gridLength) * 4].m_Wave;
			vertices1[(i + j * gridLength) * 4 + 2].m_Color[3] = vertices1[(i + j * gridLength) * 4].m_Wave;
			vertices1[(i + j * gridLength) * 4 + 3].m_Color[3] = vertices1[(i + j * gridLength) * 4].m_Wave;
			const glm::vec2 accel = glm::vec2(
				  vertices1[(i + 1 + j * gridLength) * 4].m_Wave
				- vertices1[(i - 1 + j * gridLength) * 4].m_Wave
				+ (
				- vertices1[(i + 1 + j * gridLength) * 4].m_Mass
				+ vertices1[(i - 1 + j * gridLength) * 4].m_Mass
				) * repulse
				+ sqrtTwoOTwo * (
					vertices1[(i + 1 + (j + 1) * gridLength) * 4].m_Wave
					+ vertices1[(i + 1 + (j - 1) * gridLength) * 4].m_Wave
					- vertices1[(i - 1 + (j + 1) * gridLength) * 4].m_Wave
					- vertices1[(i - 1 + (j - 1) * gridLength) * 4].m_Wave
					+ (-vertices1[(i + 1 + (j + 1) * gridLength) * 4].m_Mass
						- vertices1[(i + 1 + (j - 1) * gridLength) * 4].m_Mass
						+ vertices1[(i - 1 + (j + 1) * gridLength) * 4].m_Mass
						+ vertices1[(i - 1 + (j - 1) * gridLength) * 4].m_Mass) * repulse
					)
				, 
				
				  vertices1[(i + (j + 1) * gridLength) * 4].m_Wave
				- vertices1[(i + (j - 1) * gridLength) * 4].m_Wave
				+ (
				- vertices1[(i + (j + 1) * gridLength) * 4].m_Mass
				+ vertices1[(i + (j - 1) * gridLength) * 4].m_Mass) * repulse
				+ sqrtTwoOTwo * (
					vertices1[(i + 1 + (j + 1) * gridLength) * 4].m_Wave
					+ vertices1[(i - 1 + (j + 1) * gridLength) * 4].m_Wave
					- vertices1[(i + 1 + (j - 1) * gridLength) * 4].m_Wave
					- vertices1[(i - 1 + (j - 1) * gridLength) * 4].m_Wave
					+ (-vertices1[(i + 1 + (j + 1) * gridLength) * 4].m_Mass
						- vertices1[(i - 1 + (j + 1) * gridLength) * 4].m_Mass
						+ vertices1[(i + 1 + (j - 1) * gridLength) * 4].m_Mass
						+ vertices1[(i - 1 + (j - 1) * gridLength) * 4].m_Mass) * repulse
					)
			);
			//if (accel[0] > 1 || accel[1] > 1) {
			//	std::cout << accel[0] << " " << accel[1] << std::endl;
			//}
			//if (vertices1[(i + j * gridLength) * 4].m_Velocity[0] > 1 || vertices1[(i + j * gridLength) * 4].m_Velocity[1] > 1)
			//{
			//	std::cout << vertices1[(i + j * gridLength) * 4].m_Velocity[0] << " " << vertices1[(i + j * gridLength) * 4].m_Velocity[1] << std::endl;
			//}
			//if (vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] > 1 || vertices1[(i + j * gridLength) * 4].m_CountVelocity[1] > 1) {
			//	std::cout << vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] << " " << vertices1[(i + j * gridLength) * 4].m_CountVelocity[1] << std::endl;
			//	//std::cout << " TH: "
			//	//	<< tanh(vertices1[(i + j * gridLength) * 4].m_CountVelocity + vertices1[(i + j * gridLength) * 4].m_Velocity * deltaTime)[0]
			//	//	<< " "
			//	//	<< tanh(vertices1[(i + j * gridLength) * 4].m_CountVelocity + vertices1[(i + j * gridLength) * 4].m_Velocity * deltaTime)[1] << std::endl;
			//}
			if (vertices1[(i + j * gridLength) * 4].m_Mass > 0) {
				vertices1[(i + j * gridLength) * 4].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity + tanh(accel*deltaTime*accelDamp);
				//std::cout << vertices1[(i + j * gridLength) * 4].m_Velocity[0] << " " << vertices1[(i + j * gridLength) * 4].m_Velocity[1] << std::endl;
			}

			if (vertices1[(i + j * gridLength) * 4].m_CountVelocity[0]
				* vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] < 1) {
				vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] += vertices1[(i + j * gridLength) * 4].m_Velocity[0] * deltaTime;
				//std::cout << vertices1[(i + j * gridLength) * 4].m_Velocity[0] << std::endl;
				//std::cout << vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] << std::endl;
				if (vertices1[(i + j * gridLength) * 4].m_CountVelocity[1]
					* vertices1[(i + j * gridLength) * 4].m_CountVelocity[1] < 1) {
					vertices1[(i + j * gridLength) * 4].m_CountVelocity[1] += vertices1[(i + j * gridLength) * 4].m_Velocity[1] * deltaTime;
				}
			}
			else if (vertices1[(i + j * gridLength) * 4].m_CountVelocity[1]
				* vertices1[(i + j * gridLength) * 4].m_CountVelocity[1] < 1) {
				vertices1[(i + j * gridLength) * 4].m_CountVelocity[1] += vertices1[(i + j * gridLength) * 4].m_Velocity[1] * deltaTime;
				if (vertices1[(i + j * gridLength) * 4].m_CountVelocity[0]
					* vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] < 1) {
					vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] += vertices1[(i + j * gridLength) * 4].m_Velocity[0] * deltaTime;
				}
			}

			if (vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] > sqrtTwoOTwo
				&& vertices1[(i + j * gridLength) * 4].m_CountVelocity[1] > sqrtTwoOTwo && vertices1[(i + 1 + (j + 1) * gridLength) * 4].m_Mass == 0) {
				SetVerticesMovement(i, j, 1, 1);
			}
			else if (vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] > sqrtTwoOTwo
				&& vertices1[(i + j * gridLength) * 4].m_CountVelocity[1] < -sqrtTwoOTwo && vertices1[(i + 1 + (j - 1) * gridLength) * 4].m_Mass == 0) {
				SetVerticesMovement(i, j, 1, -1);
			}
			else if (vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] < -sqrtTwoOTwo
				&& vertices1[(i + j * gridLength) * 4].m_CountVelocity[1] > sqrtTwoOTwo && vertices1[(i - 1 + (j + 1) * gridLength) * 4].m_Mass == 0) {
				SetVerticesMovement(i, j, -1, 1);
			}
			else if (vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] < -sqrtTwoOTwo
				&& vertices1[(i + j * gridLength) * 4].m_CountVelocity[1] < -sqrtTwoOTwo && vertices1[(i - 1 + (j - 1) * gridLength) * 4].m_Mass == 0) {
				SetVerticesMovement(i, j, -1, -1);
			}
			else if (vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] < -1 && vertices1[(i - 1 + j * gridLength) * 4].m_Mass == 0) {
				SetVerticesMovement(i, j, -1, 0);
			}
			else if (vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] > 1 && vertices1[(i + 1 + j * gridLength) * 4].m_Mass == 0) {
				SetVerticesMovement(i, j, 1, 0);
			}
			else if (vertices1[(i + j * gridLength) * 4].m_CountVelocity[1] < -1 && vertices1[(i + (j - 1) * gridLength) * 4].m_Mass == 0) {
				SetVerticesMovement(i, j, 0, -1);
			}
			else if (vertices1[(i + j * gridLength) * 4].m_CountVelocity[1] > 1 && vertices1[(i + (j + 1) * gridLength) * 4].m_Mass == 0) {
				SetVerticesMovement(i, j, 0, 1);
			}
		}
	}
}
void SandBox::SetVerticesMovement(int i, int j, int ioffset, int joffset){
	if (ioffset == 1 && joffset == 1) {
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity[0] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[0] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[0] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[0] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity[1] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[1] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[1] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[1] -= sqrtTwoOTwo;
	}
	else if (ioffset == 1 && joffset == -1) {
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity[0] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[0] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[0] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[0] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity[1] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[1] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[1] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[1] += sqrtTwoOTwo;
	}
	else if (ioffset == -1 && joffset == 1) {
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity[0] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[0] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[0] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[0] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity[1] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[1] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[1] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[1] -= sqrtTwoOTwo;
	}
	else if (ioffset == -1 && joffset == -1) {
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity[0] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[0] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[0] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[0] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity[1] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[1] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[1] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[1] += sqrtTwoOTwo;
	}

	else if (ioffset == 0 && joffset == 1) {
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity[1] -= 1;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[1] -= 1;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[1] -= 1;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[1] -= 1;
	}															
	else if (ioffset == 0 && joffset == -1) {
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity[1] += 1;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[1] += 1;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[1] += 1;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[1] += 1;
	}
	else if (ioffset == 1 && joffset == 0) {
		vertices1[(i + j * gridLength) * 4    ].m_CountVelocity[0] -= 1;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[0] -= 1;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[0] -= 1;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[0] -= 1;
	}
	else if (ioffset == -1 && joffset == 0) {
		vertices1[(i + j * gridLength) * 4    ].m_CountVelocity[0] += 1;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[0] += 1;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[0] += 1;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[0] += 1;
	}
	if (m_Player == glm::vec2(i * 4, j * gridLength * 4)) {
		m_Player = glm::vec2((i + ioffset)*4, (j + joffset) * gridLength * 4);
	}
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4	].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 1].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 2].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 3].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
		vertices1[(i + j * gridLength) * 4].m_Mass = 0;
		vertices1[(i + j * gridLength) * 4 + 1].m_Mass = 0;
		vertices1[(i + j * gridLength) * 4 + 2].m_Mass = 0;
		vertices1[(i + j * gridLength) * 4 + 3].m_Mass = 0;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4	].m_Color[0] = vertices1[(i + (j) * gridLength) * 4].m_Color[0];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 1].m_Color[0] = vertices1[(i + (j) * gridLength) * 4].m_Color[0];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 2].m_Color[0] = vertices1[(i + (j) * gridLength) * 4].m_Color[0];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 3].m_Color[0] = vertices1[(i + (j) * gridLength) * 4].m_Color[0];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4	].m_Color[1] = vertices1[(i + (j) * gridLength) * 4].m_Color[1];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 1].m_Color[1] = vertices1[(i + (j) * gridLength) * 4].m_Color[1];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 2].m_Color[1] = vertices1[(i + (j) * gridLength) * 4].m_Color[1];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 3].m_Color[1] = vertices1[(i + (j) * gridLength) * 4].m_Color[1];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4	].m_Color[2] = vertices1[(i + (j) * gridLength) * 4].m_Color[2];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 1].m_Color[2] = vertices1[(i + (j) * gridLength) * 4].m_Color[2];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 2].m_Color[2] = vertices1[(i + (j) * gridLength) * 4].m_Color[2];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 3].m_Color[2] = vertices1[(i + (j) * gridLength) * 4].m_Color[2];
		vertices1[(i + (j) * gridLength) * 4	].m_Color = glm::vec4(glm::vec3(1,1,1), vertices1[(i + (j) * gridLength) * 4].m_Color[3]);
		vertices1[(i + (j) * gridLength) * 4 + 1].m_Color = glm::vec4(glm::vec3(1,1,1), vertices1[(i + (j) * gridLength) * 4].m_Color[3]);
		vertices1[(i + (j) * gridLength) * 4 + 2].m_Color = glm::vec4(glm::vec3(1,1,1), vertices1[(i + (j) * gridLength) * 4].m_Color[3]);
		vertices1[(i + (j) * gridLength) * 4 + 3].m_Color = glm::vec4(glm::vec3(1,1,1), vertices1[(i + (j) * gridLength) * 4].m_Color[3]);
		
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4	].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 1].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 2].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 3].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity = glm::vec2(0, 0);
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity = glm::vec2(0, 0);
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity = glm::vec2(0, 0);
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity = glm::vec2(0, 0);

		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4	].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 1].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 2].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 3].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
		vertices1[(i + j * gridLength) * 4	  ].m_Velocity = glm::vec2(0,0);
		vertices1[(i + j * gridLength) * 4 + 1].m_Velocity = glm::vec2(0,0);
		vertices1[(i + j * gridLength) * 4 + 2].m_Velocity = glm::vec2(0,0);
		vertices1[(i + j * gridLength) * 4 + 3].m_Velocity = glm::vec2(0,0);
}

void SandBox::OnRender()
{
	GLCall(glClearColor(0.6f, 0.6f, 0.6f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
	
	OnUpdate(0.1f);
	FollowPlayer();

	m_IndexBuffer = std::make_unique<IndexBuffer>(&index1[0], index1.size());
	m_VertexBuffer = std::make_unique<VertexBuffer>(&vertices1[0], sizeof(Vertex) * vertices1.size());
	m_VAO->AddBuffer(*m_VertexBuffer, *m_Layout);

	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
		glm::mat4 mvp = m_Proj * m_View * model;
		m_Shader->Bind();
		m_Shader->SetUniformMat4f("u_MVP", mvp);
		m_Renderer->Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
	}

	SandBox::OnImGuiRender();
}

void SandBox::OnImGuiRender() {
	ImGui::Begin("Quad");
	ImGui::Text("Hi");
	ImGui::SliderFloat3("Translation A", &m_Translation.x, 0.0f, (float)Window_Width);
	ImGui::End();
}

void SandBox::PlayerMovementW() {
	//vertices1[m_Player[0] + m_Player[1]].CountVelocity[0] += 0.3;
	vertices1[m_Player[0] + m_Player[1] - 4].m_Wave -= well;
	vertices1[m_Player[0] + m_Player[1] + 4].m_Wave += well;
}

void SandBox::PlayerMovementA() {
	//vertices1[m_Player[0] + m_Player[1]].CountVelocity[1] -= 0.3;
	//std::cout << m_Player[0] + m_Player[1] + (gridLength) * 4 << " " << vertices1.size() << std::endl;
	if (m_Player[0] + m_Player[1] + (gridLength) * 4 > vertices1.size()) {
		//std::cout << m_Player[0] + m_Player[1] + (gridLength) * 4 << std::endl;
		vertices1[m_Player[0] + m_Player[1] + (gridLength) * 4 - vertices1.size()].m_Wave -= well;
	}
	else {
		vertices1[m_Player[0] + m_Player[1] + (gridLength) * 4].m_Wave -= well;
	}
	if (m_Player[0] + m_Player[1] - (gridLength) * 4 < 0) {
		vertices1[m_Player[0] + m_Player[1] - (gridLength) * 4 + vertices1.size()].m_Wave += well;
	}
	else {
		vertices1[m_Player[0] + m_Player[1] - (gridLength) * 4].m_Wave += well;
	}
	
}

void SandBox::PlayerMovementS() {
	//vertices1[m_Player[0] + m_Player[1]].CountVelocity[0] -= 0.3;
	vertices1[m_Player[0] + m_Player[1] + 4].m_Wave -= well;
	vertices1[m_Player[0] + m_Player[1] - 4].m_Wave += well;
}

void SandBox::PlayerMovementD() {
	//vertices1[m_Player[0] + m_Player[1]].CountVelocity[1] += 0.3;
	if (m_Player[0] + m_Player[1] - (gridLength) * 4 < 0) {
		//std::cout << m_Player[0] + m_Player[1] - (gridLength)*4 << std::endl;
		vertices1[m_Player[0] + m_Player[1] - (gridLength) * 4 + vertices1.size()].m_Wave -= well;
	}
	else {
		vertices1[m_Player[0] + m_Player[1] - (gridLength) * 4].m_Wave -= well;
	}
	//vertices1[m_Player[0] + m_Player[1] - (gridLength) * 4].m_Wave -= well;
	if (m_Player[0] + m_Player[1] + (gridLength) * 4 > vertices1.size()) {
		vertices1[m_Player[0] + m_Player[1] + (gridLength) * 4 - vertices1.size()].m_Wave += well;
	}
	else {
		vertices1[m_Player[0] + m_Player[1] + (gridLength) * 4].m_Wave += well;
	}
	
}

void SandBox::AddQuad(float x, float y, float mass, glm::vec4 rgba)
{
	/*
	* TO-DO
	Performance check
	Push and add to "vertices" or create 4 vertices and push "vertices".

	use vertexCount or a vertex/index reference
	*/
	//view index memory in 4 coloums
	index1.emplace_back(0 + vertexCount);
	index1.emplace_back(1 + vertexCount);
	index1.emplace_back(2 + vertexCount);
	index1.emplace_back(2 + vertexCount);
	index1.emplace_back(3 + vertexCount);
	index1.emplace_back(0 + vertexCount);

	vertices1.emplace_back(Vertex(glm::vec2(			  x,			   y), glm::vec4(rgba), mass, mass, glm::vec2(0, 0), glm::vec2(0, 0)));
	vertices1.emplace_back(Vertex(glm::vec2( sideLength + x,			   y), glm::vec4(rgba), mass, mass, glm::vec2(0, 0), glm::vec2(0, 0)));
	vertices1.emplace_back(Vertex(glm::vec2( sideLength + x,  sideLength + y), glm::vec4(rgba), mass, mass, glm::vec2(0, 0), glm::vec2(0, 0)));
	vertices1.emplace_back(Vertex(glm::vec2(			  x,  sideLength + y), glm::vec4(rgba), mass, mass, glm::vec2(0, 0), glm::vec2(0, 0)));

	vertexCount += 4;
}


void SandBox::SetRenderer(Renderer* m_Renderer) {
	m_Renderer = m_Renderer;
}

void SandBox::SetTranslation(glm::vec3 m_Translate) {
	m_Translation += m_Translate;
}

void SandBox::FollowPlayer() {
	//vertices1[m_Player[0] + m_Player[1]].m_Position
	m_Translation += (glm::vec3(-vertices1[m_Player[0] + m_Player[1]].m_Position.x + Window_Width * 0.5, -vertices1[m_Player[0] + m_Player[1]].m_Position.y + Window_Height * 0.5, 0)
		- m_Translation) * glm::vec3(0.02, 0.02, 0.02);
	//m_Translation = glm::vec3(-vertices1[m_Player[0] + m_Player[1]].m_Position.x + Window_Width* 0.5, -vertices1[m_Player[0] + m_Player[1]].m_Position.y + Window_Height * 0.5, 0);
}
