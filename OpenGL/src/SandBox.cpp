#define EnableEdge


#define DetectArrayError
//#define ArrayError
//#define ArrayError1
//#define ArrayError1a
//#define ArrayErrorVML8
//#define ArrayErrorVML8a
//#define ArrayErrorVML9
//#define ArrayErrorVML9a
//#define ArrayErrorSVM10
//#define ArrayErrorSVM10A
//#define ArrayErrorSVM10B
//#define ArrayErrorSVM10C
//#define ArrayErrorSVM11
//#define ArrayErrorSVM11A
//#define ArrayErrorSVM11B
//#define ArrayErrorSVM11C
//#define ArrayErrorSVM12
//#define ArrayErrorSVM13
//#define ArrayErrorSVM14

#ifdef HI
#define ArrayErrorA
#define ArrayErrorB
#define ArrayErrorC
#define ArrayErrorD
#define ArrayError0
#define ArrayError1
#define ArrayError2
#define ArrayError3
#define ArrayError4
#define ArrayErrorVML0
#define ArrayErrorVML0a
#define ArrayErrorVML1
#define ArrayErrorVML1a
#define ArrayErrorVML2
#define ArrayErrorVML2a
#define ArrayErrorVML3
#define ArrayErrorVML3a
#define ArrayErrorVML4
#define ArrayErrorVML4a
#define ArrayErrorVML5
#define ArrayErrorVML5a
#define ArrayErrorVML6
#define ArrayErrorVML6a
#define ArrayErrorVML7
#define ArrayErrorVML7a
#define ArrayErrorVML8
#define ArrayErrorVML8a
#define ArrayErrorVML9
#define ArrayErrorVML9a
#define ArrayErrorSVM1
#define ArrayErrorSVM2
#define ArrayErrorSVM3
#define ArrayErrorSVM4
#define ArrayErrorSVM5
#define ArrayErrorSVM6
#define ArrayErrorSVM7
#define ArrayErrorSVM8
#define ArrayErrorSVM9
#define ArrayErrorSVM10
#define ArrayErrorSVM10A
#define ArrayErrorSVM10B
#define ArrayErrorSVM10C
#define ArrayErrorSVM11
#define ArrayErrorSVM11A
#define ArrayErrorSVM11B
#define ArrayErrorSVM11C
#define ArrayErrorSVM12
#define ArrayErrorSVM13
#define ArrayErrorSVM14
#endif

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
#ifdef ArrayErrorA
	std::cout << "A" << std::endl;
#endif // ArrayError
#ifdef EnableEdge
	Vertices1MovementLogic(0, gridLength - 1, 1, gridLength - 1, 0, gridLength - 2, deltaTime);
#endif
	/*
	|	|	0
	|	|	|
	|	|	|
	*/
#ifdef ArrayErrorB
	std::cout << "B" << std::endl;
#endif // ArrayError
#ifdef EnableEdge
	Vertices1MovementLogic(gridLength - 1, gridLength - 1, 0, gridLength - 2, 0, gridLength - 2, deltaTime);
#endif
	/*
	|	|	|
	|	|	|
	0	|	|
	*/
#ifdef ArrayErrorC
	std::cout << "C" << std::endl;
#endif // ArrayError
#ifdef EnableEdge
	Vertices1MovementLogic(0, 0, 1, gridLength - 1, 1, gridLength - 1, deltaTime);
#endif
	/*
	0	|	|
	|	|	|
	|	|	|
	*/
#ifdef ArrayErrorD
	std::cout << "D" << std::endl;
#endif // ArrayError
#ifdef EnableEdge
	Vertices1MovementLogic(gridLength - 1, 0, 0, gridLength - 2, 1, gridLength - 1, deltaTime);
#endif
	for (int i = 1; i < gridLength-1; i++) {
		/*
		|	|	|
		0	|	|
		|	|	|
		*/
#ifdef ArrayError0
		std::cout << 0 << std::endl;
#endif // ArrayError
#ifdef EnableEdge
		Vertices1MovementLogic(i, 0, i + 1, i - 1, 1, gridLength - 1, deltaTime);
#endif
		/*
		|	|	|
		|	|	0
		|	|	|
		*/
#ifdef ArrayError1
		std::cout << 1 << std::endl;
#endif // ArrayError
#ifdef EnableEdge
#ifdef ArrayError1A
		std::cout << "1A" << std::endl;
#endif // ArrayError
		Vertices1MovementLogic(i, gridLength - 1, i + 1, i - 1, 0, gridLength - 2, deltaTime);
#ifdef ArrayError1B
		std::cout << "1B" << std::endl;
#endif // ArrayError
#endif
		/*
		|	|	|
		|	|	|
		|	0	|
		*/
#ifdef ArrayError2
		std::cout << 2 << std::endl;
#endif // ArrayError
#ifdef EnableEdge
		Vertices1MovementLogic(0, i, 1, gridLength - 1, i + 1, i - 1, deltaTime);
#endif
		/*
		|	0	|
		|	|	|
		|	|	|
		*/
#ifdef ArrayError3
		std::cout << 3 << std::endl;
#endif // ArrayError

#ifdef EnableEdge
		Vertices1MovementLogic(gridLength - 1, i, 0, gridLength - 2, i + 1, i - 1, deltaTime);
#endif
		for (int j = 1; j < gridLength-1; j++) {
			/*
			|	|	|
			|	0	|
			|	|	|
			*/
			Vertices1MovementLogic(i, j, i + 1, i - 1, j + 1, j - 1, deltaTime);
		}
	}
}

void SandBox::SetVerticesMovement(int i, int j, int ioffset, int joffset){
	//std::cout << "SetVerticesMovement" << std::endl;
	if (
#ifdef ArrayErrorSVM1
		std::cout << "svm1" << std::endl &&
#endif // ArrayError
		ioffset == 1 && joffset == 1) {
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity[0] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[0] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[0] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[0] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity[1] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[1] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[1] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[1] -= sqrtTwoOTwo;
	}
	else if (
#ifdef ArrayErrorSVM2
		std::cout << "svm2" << std::endl &&
#endif // ArrayError
		ioffset == 1 && joffset == -1) {
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity[0] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[0] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[0] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[0] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity[1] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[1] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[1] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[1] += sqrtTwoOTwo;
	}
	else if (
#ifdef ArrayErrorSVM3
		std::cout << "svm3" << std::endl &&
#endif // ArrayError
		ioffset == -1 && joffset == 1) {
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity[0] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[0] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[0] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[0] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity[1] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[1] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[1] -= sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[1] -= sqrtTwoOTwo;
	}
	else if (
#ifdef ArrayErrorSVM4
		std::cout << "svm4" << std::endl &&
#endif // ArrayError
		ioffset == -1 && joffset == -1) {
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity[0] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[0] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[0] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[0] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity[1] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[1] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[1] += sqrtTwoOTwo;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[1] += sqrtTwoOTwo;
	}
	else if (
#ifdef ArrayErrorSVM5
		std::cout << "svm5" << std::endl &&
#endif // ArrayError
		ioffset == 0 && joffset == 1) {
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity[1] -= 1;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[1] -= 1;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[1] -= 1;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[1] -= 1;
	}															
	else if (
#ifdef ArrayErrorSVM6
		std::cout << "svm6" << std::endl &&
#endif // ArrayError
		ioffset == 0 && joffset == -1) {
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity[1] += 1;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[1] += 1;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[1] += 1;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[1] += 1;
	}
	else if (
#ifdef ArrayErrorSVM7
		std::cout << "svm7" << std::endl &&
#endif // ArrayError
		ioffset == 1 && joffset == 0) {
		vertices1[(i + j * gridLength) * 4    ].m_CountVelocity[0] -= 1;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[0] -= 1;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[0] -= 1;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[0] -= 1;
	}
	else if (
#ifdef ArrayErrorSVM8
		std::cout << "svm8" << std::endl &&
#endif // ArrayError
		ioffset == -1 && joffset == 0) {
		vertices1[(i + j * gridLength) * 4    ].m_CountVelocity[0] += 1;
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity[0] += 1;
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity[0] += 1;
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity[0] += 1;
	}
	if (
#ifdef ArrayErrorSVM10
		std::cout << "svm10" << std::endl &&
#endif // ArrayError
		j + joffset < 0) {
		if (
#ifdef ArrayErrorSVM10A
			std::cout << "svm10A" << std::endl &&
#endif // ArrayError
			i + ioffset < 0) {
			if (m_Player == glm::vec2(i * 4, j * gridLength * 4)) {
				m_Player = glm::vec2((gridLength + ioffset) * 4, (gridLength + joffset) * gridLength * 4);
			}
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4	  ].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4 + 1].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4 + 2].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4 + 3].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4	  ].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4 + 1].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4 + 2].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4 + 3].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4	  ].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4 + 1].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4 + 2].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4 + 3].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4	  ].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4 + 1].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4 + 2].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4 + 3].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4 + 1].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4 + 2].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4 + 3].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4 + 1].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4 + 2].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(gridLength + ioffset + (gridLength + joffset) * gridLength) * 4 + 3].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
		}
		else if (
#ifdef ArrayErrorSVM10B
			std::cout << "svm10B" << std::endl &&
#endif // ArrayError
			i + ioffset > gridLength - 1) {
			if (m_Player == glm::vec2(i * 4, j * gridLength * 4)) {
				m_Player = glm::vec2((ioffset) * 4, (gridLength + joffset) * gridLength * 4);
			}
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4	 ].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4 + 1].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4 + 2].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4 + 3].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4	].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4 + 1].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4 + 2].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4 + 3].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4	].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4 + 1].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4 + 2].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4 + 3].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4	].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4 + 1].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4 + 2].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4 + 3].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4 + 1].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4 + 2].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4 + 3].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4 + 1].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4 + 2].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(ioffset + (gridLength + joffset) * gridLength) * 4 + 3].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
		}
		else {
#ifdef ArrayErrorSVM10C
			std::cout << "svm10C" << std::endl;
#endif // ArrayError
			if (m_Player == glm::vec2(i * 4, j * gridLength * 4)) {
				m_Player = glm::vec2((i + ioffset) * 4, (gridLength + joffset) * gridLength * 4);
			}
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4	 ].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4 + 1].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4 + 2].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4 + 3].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4	].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4 + 1].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4 + 2].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4 + 3].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4	].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4 + 1].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4 + 2].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4 + 3].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4	].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4 + 1].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4 + 2].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4 + 3].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4 + 1].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4 + 2].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4 + 3].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4 + 1].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4 + 2].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(i + ioffset + (gridLength + joffset) * gridLength) * 4 + 3].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
#ifdef ArrayErrorSVM10C
			std::cout << "svm10C" << std::endl;
#endif // ArrayError
		}
	}
	else if (
#ifdef ArrayErrorSVM11
		std::cout << "svm11" << std::endl &&
#endif // ArrayError
		j + joffset > gridLength - 1) {
		if (
#ifdef ArrayErrorSVM11A
			std::cout << "svm11A" << std::endl &&
#endif // ArrayError
			i + ioffset < 0) {
			if (m_Player == glm::vec2(i * 4, j * gridLength * 4)) {
				m_Player = glm::vec2((gridLength + ioffset) * 4, (joffset) * gridLength * 4);
			}
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4 + 1].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4 + 2].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4 + 3].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4 + 1].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4 + 2].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4 + 3].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4 + 1].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4 + 2].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4 + 3].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4 + 1].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4 + 2].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4 + 3].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4 + 1].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4 + 2].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4 + 3].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4 + 1].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4 + 2].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(gridLength + ioffset + (joffset) * gridLength) * 4 + 3].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
		}
		else if (
#ifdef ArrayErrorSVM11B
			std::cout << "svm11B" << std::endl &&
#endif // ArrayError
			i + ioffset > gridLength - 1) {
			if (m_Player == glm::vec2(i * 4, j * gridLength * 4)) {
				m_Player = glm::vec2((ioffset) * 4, (joffset) * gridLength * 4);
			}
			vertices1[(ioffset + (joffset) * gridLength) * 4].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(ioffset + (joffset) * gridLength) * 4 + 1].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(ioffset + (joffset) * gridLength) * 4 + 2].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(ioffset + (joffset) * gridLength) * 4 + 3].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(ioffset + (joffset) * gridLength) * 4].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(ioffset + (joffset) * gridLength) * 4 + 1].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(ioffset + (joffset) * gridLength) * 4 + 2].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(ioffset + (joffset) * gridLength) * 4 + 3].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(ioffset + (joffset) * gridLength) * 4].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(ioffset + (joffset) * gridLength) * 4 + 1].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(ioffset + (joffset) * gridLength) * 4 + 2].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(ioffset + (joffset) * gridLength) * 4 + 3].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(ioffset + (joffset) * gridLength) * 4].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(ioffset + (joffset) * gridLength) * 4 + 1].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(ioffset + (joffset) * gridLength) * 4 + 2].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(ioffset + (joffset) * gridLength) * 4 + 3].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(ioffset + (joffset) * gridLength) * 4].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(ioffset + (joffset) * gridLength) * 4 + 1].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(ioffset + (joffset) * gridLength) * 4 + 2].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(ioffset + (joffset) * gridLength) * 4 + 3].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(ioffset + (joffset) * gridLength) * 4].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(ioffset + (joffset) * gridLength) * 4 + 1].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(ioffset + (joffset) * gridLength) * 4 + 2].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(ioffset + (joffset) * gridLength) * 4 + 3].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
		}
		else {
#ifdef ArrayErrorSVM11C
			std::cout << "svm11C " << (gridLength - 1 + (gridLength - 1) * gridLength) * 4 << " " << (i + ioffset + (joffset) * gridLength) * 4 <<  " " << (i + j * gridLength) * 4  << std::endl;
#endif // ArrayError
			if (m_Player == glm::vec2(i * 4, j * gridLength * 4)) {
				m_Player = glm::vec2((i + ioffset) * 4, (joffset) * gridLength * 4);
			}
			vertices1[(i + ioffset + (joffset) * gridLength) * 4].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(i + ioffset + (joffset) * gridLength) * 4 + 1].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(i + ioffset + (joffset) * gridLength) * 4 + 2].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(i + ioffset + (joffset) * gridLength) * 4 + 3].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(i + ioffset + (joffset) * gridLength) * 4].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(i + ioffset + (joffset) * gridLength) * 4 + 1].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(i + ioffset + (joffset) * gridLength) * 4 + 2].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(i + ioffset + (joffset) * gridLength) * 4 + 3].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(i + ioffset + (joffset) * gridLength) * 4].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(i + ioffset + (joffset) * gridLength) * 4 + 1].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(i + ioffset + (joffset) * gridLength) * 4 + 2].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(i + ioffset + (joffset) * gridLength) * 4 + 3].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(i + ioffset + (joffset) * gridLength) * 4].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(i + ioffset + (joffset) * gridLength) * 4 + 1].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(i + ioffset + (joffset) * gridLength) * 4 + 2].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(i + ioffset + (joffset) * gridLength) * 4 + 3].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(i + ioffset + (joffset) * gridLength) * 4].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(i + ioffset + (joffset) * gridLength) * 4 + 1].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(i + ioffset + (joffset) * gridLength) * 4 + 2].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(i + ioffset + (joffset) * gridLength) * 4 + 3].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(i + ioffset + (joffset) * gridLength) * 4].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(i + ioffset + (joffset) * gridLength) * 4 + 1].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(i + ioffset + (joffset) * gridLength) * 4 + 2].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(i + ioffset + (joffset) * gridLength) * 4 + 3].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
#ifdef ArrayErrorSVM11C
			std::cout << "svm11C" << std::endl;
#endif // ArrayError
		}
	}
	else if (
#ifdef ArrayErrorSVM12
		std::cout << "svm12" << std::endl &&
#endif // ArrayError
		i + ioffset < 0) {
			if (m_Player == glm::vec2(i * 4, j * gridLength * 4)) {
				m_Player = glm::vec2((gridLength + ioffset) * 4, (j + joffset) * gridLength * 4);
			}
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4	 ].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4 + 1].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4 + 2].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4 + 3].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4	].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4 + 1].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4 + 2].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4 + 3].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4	].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4 + 1].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4 + 2].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4 + 3].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4	].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4 + 1].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4 + 2].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4 + 3].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4 + 1].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4 + 2].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4 + 3].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4 + 1].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4 + 2].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
			vertices1[(gridLength + ioffset + (j + joffset) * gridLength) * 4 + 3].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
		}
	else if (
#ifdef ArrayErrorSVM13
			std::cout << "svm13" << std::endl &&
#endif // ArrayError
			i + ioffset > gridLength - 1) {
				if (m_Player == glm::vec2(i * 4, j * gridLength * 4)) {
					m_Player = glm::vec2((ioffset) * 4, (j + joffset) * gridLength * 4);
				}
		vertices1[(ioffset + (j + joffset) * gridLength) * 4	].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
		vertices1[(ioffset + (j + joffset) * gridLength) * 4 + 1].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
		vertices1[(ioffset + (j + joffset) * gridLength) * 4 + 2].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
		vertices1[(ioffset + (j + joffset) * gridLength) * 4 + 3].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
		vertices1[(ioffset + (j + joffset) * gridLength) * 4	].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
		vertices1[(ioffset + (j + joffset) * gridLength) * 4 + 1].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
		vertices1[(ioffset + (j + joffset) * gridLength) * 4 + 2].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
		vertices1[(ioffset + (j + joffset) * gridLength) * 4 + 3].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
		vertices1[(ioffset + (j + joffset) * gridLength) * 4	].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
		vertices1[(ioffset + (j + joffset) * gridLength) * 4 + 1].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
		vertices1[(ioffset + (j + joffset) * gridLength) * 4 + 2].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
		vertices1[(ioffset + (j + joffset) * gridLength) * 4 + 3].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
		vertices1[(ioffset + (j + joffset) * gridLength) * 4	].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
		vertices1[(ioffset + (j + joffset) * gridLength) * 4 + 1].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
		vertices1[(ioffset + (j + joffset) * gridLength) * 4 + 2].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
		vertices1[(ioffset + (j + joffset) * gridLength) * 4 + 3].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
		vertices1[(ioffset + (j + joffset) * gridLength) * 4].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
		vertices1[(ioffset + (j + joffset) * gridLength) * 4 + 1].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
		vertices1[(ioffset + (j + joffset) * gridLength) * 4 + 2].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
		vertices1[(ioffset + (j + joffset) * gridLength) * 4 + 3].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
		vertices1[(ioffset + (j + joffset) * gridLength) * 4].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
		vertices1[(ioffset + (j + joffset) * gridLength) * 4 + 1].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
		vertices1[(ioffset + (j + joffset) * gridLength) * 4 + 2].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
		vertices1[(ioffset + (j + joffset) * gridLength) * 4 + 3].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
	}
	else {
#ifdef ArrayErrorSVM14
		std::cout << "svm14 " << (gridLength - 1 + (gridLength - 1) * gridLength) * 4 << " " << (i + ioffset + (j + joffset) * gridLength) * 4 << std::endl;
#endif // ArrayError
		if (m_Player == glm::vec2(i * 4, j * gridLength * 4)) {
			m_Player = glm::vec2((i + ioffset) * 4, (j + joffset) * gridLength * 4);
		}
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4	].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 1].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 2].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 3].m_Mass = vertices1[(i + j * gridLength) * 4].m_Mass;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4	].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 1].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 2].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 3].m_Color[0] = vertices1[(i + (j)*gridLength) * 4].m_Color[0];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4	].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 1].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 2].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 3].m_Color[1] = vertices1[(i + (j)*gridLength) * 4].m_Color[1];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4	].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 1].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 2].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 3].m_Color[2] = vertices1[(i + (j)*gridLength) * 4].m_Color[2];
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 1].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 2].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 3].m_CountVelocity = vertices1[(i + j * gridLength) * 4].m_CountVelocity;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 1].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 2].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
		vertices1[(i + ioffset + (j + joffset) * gridLength) * 4 + 3].m_Velocity = vertices1[(i + j * gridLength) * 4].m_Velocity;
#ifdef ArrayErrorSVM14
		std::cout << "svm14" << std::endl;
#endif // ArrayError
	}		
		vertices1[(i + j * gridLength) * 4	].m_Mass = 0;
		vertices1[(i + j * gridLength) * 4 + 1].m_Mass = 0;
		vertices1[(i + j * gridLength) * 4 + 2].m_Mass = 0;
		vertices1[(i + j * gridLength) * 4 + 3].m_Mass = 0;

		vertices1[(i + (j) * gridLength) * 4	].m_Color = glm::vec4(glm::vec3(1,1,1), vertices1[(i + (j) * gridLength) * 4].m_Color[3]);
		vertices1[(i + (j) * gridLength) * 4 + 1].m_Color = glm::vec4(glm::vec3(1,1,1), vertices1[(i + (j) * gridLength) * 4].m_Color[3]);
		vertices1[(i + (j) * gridLength) * 4 + 2].m_Color = glm::vec4(glm::vec3(1,1,1), vertices1[(i + (j) * gridLength) * 4].m_Color[3]);
		vertices1[(i + (j) * gridLength) * 4 + 3].m_Color = glm::vec4(glm::vec3(1,1,1), vertices1[(i + (j) * gridLength) * 4].m_Color[3]);
		
		vertices1[(i + j * gridLength) * 4	  ].m_CountVelocity = glm::vec2(0, 0);
		vertices1[(i + j * gridLength) * 4 + 1].m_CountVelocity = glm::vec2(0, 0);
		vertices1[(i + j * gridLength) * 4 + 2].m_CountVelocity = glm::vec2(0, 0);
		vertices1[(i + j * gridLength) * 4 + 3].m_CountVelocity = glm::vec2(0, 0);

		vertices1[(i + j * gridLength) * 4	  ].m_Velocity = glm::vec2(0,0);
		vertices1[(i + j * gridLength) * 4 + 1].m_Velocity = glm::vec2(0,0);
		vertices1[(i + j * gridLength) * 4 + 2].m_Velocity = glm::vec2(0,0);
		vertices1[(i + j * gridLength) * 4 + 3].m_Velocity = glm::vec2(0,0);
}

void SandBox::Vertices1MovementLogic(int i, int j, int ip1, int im1, int jp1, int jm1, float deltaTime) {
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
	vertices1[(i + j * gridLength) * 4].m_Wave
		= (vertices1[(i + j * gridLength) * 4].m_Wave
			+ vertices1[(ip1 + j * gridLength) * 4].m_Wave
			+ vertices1[(im1 + j * gridLength) * 4].m_Wave
			+ vertices1[(i + (jp1) * gridLength) * 4].m_Wave
			+ vertices1[(i + (jm1) * gridLength) * 4].m_Wave
			+ vertices1[(ip1 + (jp1) * gridLength) * 4].m_Wave
			+ vertices1[(ip1 + (jm1) * gridLength) * 4].m_Wave
			+ vertices1[(im1 + (jp1) * gridLength) * 4].m_Wave
			+ vertices1[(im1 + (jm1) * gridLength) * 4].m_Wave) * dispersion
		+ vertices1[(i + j * gridLength) * 4].m_Mass * mDispersion;
	//std::cout << "Wave "<< i << " " << j << " " << vertices1[(i + j * gridLength) * 4].m_Wave << " " << vertices1[(i + j * gridLength) * 4].m_Mass << std::endl;
	vertices1[(i + j * gridLength) * 4].m_Color[3] = vertices1[(i + j * gridLength) * 4].m_Wave * intensity;
	vertices1[(i + j * gridLength) * 4 + 1].m_Color[3] = vertices1[(i + j * gridLength) * 4].m_Wave * intensity;
	vertices1[(i + j * gridLength) * 4 + 2].m_Color[3] = vertices1[(i + j * gridLength) * 4].m_Wave * intensity;
	vertices1[(i + j * gridLength) * 4 + 3].m_Color[3] = vertices1[(i + j * gridLength) * 4].m_Wave * intensity;
	vertices1[(i + j * gridLength) * 4].m_Wave
		= (vertices1[(i + j * gridLength) * 4].m_Wave
			+ vertices1[(ip1 + j * gridLength) * 4].m_Wave
			+ vertices1[(im1 + j * gridLength) * 4].m_Wave
			+ vertices1[(i + (jp1) * gridLength) * 4].m_Wave
			+ vertices1[(i + (jm1) * gridLength) * 4].m_Wave
			+ vertices1[(ip1 + (jp1) * gridLength) * 4].m_Wave
			+ vertices1[(ip1 + (jm1) * gridLength) * 4].m_Wave
			+ vertices1[(im1 + (jp1) * gridLength) * 4].m_Wave
			+ vertices1[(im1 + (jm1) * gridLength) * 4].m_Wave) * dispersion
		+ vertices1[(i + j * gridLength) * 4].m_Mass * mDispersion;
	//std::cout << "Wave "<< i << " " << j << " " << vertices1[(i + j * gridLength) * 4].m_Wave << " " << vertices1[(i + j * gridLength) * 4].m_Mass << std::endl;
	vertices1[(i + j * gridLength) * 4].m_Color[3] = vertices1[(i + j * gridLength) * 4].m_Wave * intensity;
	vertices1[(i + j * gridLength) * 4 + 1].m_Color[3] = vertices1[(i + j * gridLength) * 4].m_Wave * intensity;
	vertices1[(i + j * gridLength) * 4 + 2].m_Color[3] = vertices1[(i + j * gridLength) * 4].m_Wave * intensity;
	vertices1[(i + j * gridLength) * 4 + 3].m_Color[3] = vertices1[(i + j * gridLength) * 4].m_Wave * intensity;
	accel = glm::vec2(
		vertices1[(ip1 + j * gridLength) * 4].m_Wave * 0.05
		- vertices1[(im1 + j * gridLength) * 4].m_Wave * 0.05
		+ (
			-vertices1[(ip1 + j * gridLength) * 4].m_Mass
			+ vertices1[(im1 + j * gridLength) * 4].m_Mass) * repulse
		+ sqrtTwoOTwo * (
			vertices1[(ip1 + (jp1) * gridLength) * 4].m_Wave
			+ vertices1[(ip1 + (jm1) * gridLength) * 4].m_Wave
			- vertices1[(im1 + (jp1) * gridLength) * 4].m_Wave
			- vertices1[(im1 + (jm1) * gridLength) * 4].m_Wave
			+ (-vertices1[(ip1 + (jp1) * gridLength) * 4].m_Mass
				- vertices1[(ip1 + (jm1) * gridLength) * 4].m_Mass
				+ vertices1[(im1 + (jp1) * gridLength) * 4].m_Mass
				+ vertices1[(im1 + (jm1) * gridLength) * 4].m_Mass) * repulse
			)
		,
		vertices1[(i + (jp1) * gridLength) * 4].m_Wave
		- vertices1[(i + (jm1) * gridLength) * 4].m_Wave
		+ (
			-vertices1[(i + (jp1) * gridLength) * 4].m_Mass
			+ vertices1[(i + (jm1) * gridLength) * 4].m_Mass) * repulse
		+ sqrtTwoOTwo * (
			vertices1[(ip1 + (jp1) * gridLength) * 4].m_Wave
			+ vertices1[(im1 + (jp1) * gridLength) * 4].m_Wave
			- vertices1[(ip1 + (jm1) * gridLength) * 4].m_Wave
			- vertices1[(im1 + (jm1) * gridLength) * 4].m_Wave
			+ (-vertices1[(ip1 + (jp1) * gridLength) * 4].m_Mass
				- vertices1[(im1 + (jp1) * gridLength) * 4].m_Mass
				+ vertices1[(ip1 + (jm1) * gridLength) * 4].m_Mass
				+ vertices1[(im1 + (jm1) * gridLength) * 4].m_Mass) * repulse
			)
	);
	if (vertices1[(i + j * gridLength) * 4].m_Mass > 0) {
		vertices1[(i + j * gridLength) * 4].m_Velocity[0] = vertices1[(i + j * gridLength) * 4].m_Velocity[0] + tanh(accel[0] * deltaTime * accelDamp);
		vertices1[(i + j * gridLength) * 4].m_Velocity[1] = vertices1[(i + j * gridLength) * 4].m_Velocity[1] + tanh(accel[1] * deltaTime * accelDamp);
	}

	if (vertices1[(i + j * gridLength) * 4].m_CountVelocity[0]
		* vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] < 1) {
		vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] += vertices1[(i + j * gridLength) * 4].m_Velocity[0] * deltaTime;
	#ifdef ArrayErrorVML0
			std::cout << "vml0" << std::endl;
	#endif // ArrayError
	}
	if (vertices1[(i + j * gridLength) * 4].m_CountVelocity[1]
		* vertices1[(i + j * gridLength) * 4].m_CountVelocity[1] < 1) {
		vertices1[(i + j * gridLength) * 4].m_CountVelocity[1] += vertices1[(i + j * gridLength) * 4].m_Velocity[1] * deltaTime;	
	#ifdef ArrayErrorVML1
		std::cout << "vml1" << std::endl;
	#endif // ArrayError
	}

	if (
	#ifdef ArrayErrorVML2
			std::cout << "vml2" << std::endl &&
	#endif // ArrayError
		vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] > sqrtTwoOTwo
		&& vertices1[(i + j * gridLength) * 4].m_CountVelocity[1] > sqrtTwoOTwo && vertices1[(ip1 + jp1 * gridLength) * 4].m_Mass == 0) {
	#ifdef ArrayErrorVML2a
		std::cout << "vml2a" << std::endl;
	#endif // ArrayError
		SetVerticesMovement(i, j, 1, 1);
	}
	else if (
#ifdef ArrayErrorVML3
		std::cout << "vml3" << std::endl &&
#endif // ArrayError
		vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] > sqrtTwoOTwo
		&& vertices1[(i + j * gridLength) * 4].m_CountVelocity[1] < -sqrtTwoOTwo && vertices1[(ip1 + jm1 * gridLength) * 4].m_Mass == 0) {
		
	#ifdef ArrayErrorVML3a
		std::cout << "vml3a" << std::endl;
	#endif // ArrayError
		SetVerticesMovement(i, j, 1, -1);
	}
	else if (
#ifdef ArrayErrorVML4
		std::cout << "vml4" << std::endl &&
#endif // ArrayError
		vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] < -sqrtTwoOTwo
		&& vertices1[(i + j * gridLength) * 4].m_CountVelocity[1] > sqrtTwoOTwo && vertices1[(im1 + jp1 * gridLength) * 4].m_Mass == 0) {
		
	#ifdef ArrayErrorVML4a
			std::cout << "vml4a" << std::endl;
	#endif // ArrayError
		SetVerticesMovement(i, j, -1, 1);
	}
	else if (
#ifdef ArrayErrorVML5
		std::cout << "vml5" << std::endl &&
#endif // ArrayError
		vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] < -sqrtTwoOTwo
		&& vertices1[(i + j * gridLength) * 4].m_CountVelocity[1] < -sqrtTwoOTwo && vertices1[(im1 + jm1 * gridLength) * 4].m_Mass == 0) {
		
	#ifdef ArrayErrorVML5a
			std::cout << "vml5a" << std::endl;
	#endif // ArrayError
			SetVerticesMovement(i, j, -1, -1);
	}
	else if (
#ifdef ArrayErrorVML6
		std::cout << "vml6" << std::endl &&
#endif // ArrayError
		vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] < -1 && vertices1[(im1 + j * gridLength) * 4].m_Mass == 0) {
#ifdef ArrayErrorVML6a
	std::cout << "vml6a" << std::endl;
#endif // ArrayError
SetVerticesMovement(i, j, -1, 0);
	}
	else if (
#ifdef ArrayErrorVML7
		std::cout << "vml7" << std::endl &&
#endif // ArrayError
		vertices1[(i + j * gridLength) * 4].m_CountVelocity[0] > 1 && vertices1[(ip1 + j * gridLength) * 4].m_Mass == 0) {
		
#ifdef ArrayErrorVML7a
	std::cout << "vml7a" << std::endl;
#endif // ArrayError
SetVerticesMovement(i, j, 1, 0);
	}

	else if (
#ifdef ArrayErrorVML8
		std::cout << "vml8: " << (gridLength - 1 + (gridLength - 1) * gridLength) * 4 << " " << (i + jm1 * gridLength) * 4 << std::endl &&
#endif // ArrayError
		vertices1[(i + j * gridLength) * 4].m_CountVelocity[1] < -1 && vertices1[(i + jm1 * gridLength) * 4].m_Mass == 0) {
		
#ifdef ArrayErrorVML8a
	std::cout << "vml8a" << std::endl;
#endif // ArrayError
SetVerticesMovement(i, j, 0, -1);
	}
	//Vertices1MovementLogic(i, gridLength - 1, i + 1, i - 1, 0, gridLength - 2, deltaTime);
	else if (
#ifdef ArrayErrorVML9
		std::cout << "vml9: " << (i + jp1 * gridLength) * 4 << std::endl &&
#endif // ArrayError
		vertices1[(i + j * gridLength) * 4].m_CountVelocity[1] > 1 && vertices1[(i + jp1 * gridLength) * 4].m_Mass == 0) {
#ifdef ArrayErrorVML9a
		std::cout << "vml9a" << std::endl;
#endif // ArrayError
		SetVerticesMovement(i, j, 0, 1);
	}
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
	if (m_Player[0] + 4 > (gridLength - 1) * 4) {
		//std::cout << accel.x << " " << accel.y << std::endl;
		//std::cout << vertices1[m_Player[0] + m_Player[1]].m_CountVelocity.x << " " << vertices1[m_Player[0] + m_Player[1]].m_CountVelocity.y << std::endl;
#ifdef DetectArrayError
		if (m_Player[0] + m_Player[1] - (gridLength) * 4 + 4 < 0 ||
			m_Player[0] + m_Player[1] - (gridLength) * 4 + 4 > (gridLength - 1) * gridLength * 4 + gridLength * 4) {
			std::cout << "Error: PlayerMovementS if m_Player[0] + m_Player[1] - (gridLength) * 4 + 4"
				<< m_Player[0] + m_Player[1] - (gridLength) * 4 + 4 << ", " << vertices1.size() <<
				std::endl;
		}
#endif
		vertices1[m_Player[0] + m_Player[1] - (gridLength) * 4 + 4].m_Wave += well;
	}
	else {
#ifdef DetectArrayError
		if (m_Player[0] + m_Player[1] + 4 < 0 || 
			m_Player[0] + m_Player[1] + 4 > (gridLength - 1) * gridLength * 4 + gridLength * 4) {
			std::cout << "Error: PlayerMovementA else m_Player[0] + m_Player[1] + 4"
				<< m_Player[0] + m_Player[1] + 4 << ", " << vertices1.size() <<
				std::endl;
		}
#endif
		vertices1[m_Player[0] + m_Player[1] + 4].m_Wave += well;
	} 
	if (m_Player[0] - 4 < 0) {
#ifdef DetectArrayError
		if (m_Player[0] + m_Player[1] + (gridLength * 4) - 4 < 0 ||
			m_Player[0] + m_Player[1] + (gridLength * 4) - 4 > (gridLength - 1) * gridLength * 4 + gridLength * 4) {
			std::cout << "Error: PlayerMovementS if m_Player[0] + m_Player[1] + (gridLength * 4) - 4"
				<< m_Player[0] + m_Player[1] + (gridLength * 4) - 4 << ", " << vertices1.size() <<
				std::endl;
		}
#endif
		vertices1[m_Player[0] + m_Player[1] + (gridLength * 4) - 4].m_Wave -= well;
	}
	else {
#ifdef DetectArrayError
		if (m_Player[0] + m_Player[1] - 4 < 0 || 
			m_Player[0] + m_Player[1] - 4 > (gridLength - 1) * gridLength * 4 + gridLength * 4) {
			std::cout << "Error: PlayerMovementA else m_Player[0] + m_Player[1] - 4"
				<< m_Player[0] + m_Player[1] - 4 << ", " << vertices1.size() <<
				std::endl;
		}
#endif
		vertices1[m_Player[0] + m_Player[1] - 4].m_Wave -= well;
	}
}

void SandBox::PlayerMovementA() {
	if (m_Player[1] + (2 * gridLength) * 4 > (gridLength - 1) * gridLength * 4) {
		
#ifdef DetectArrayError
		if (m_Player[0] + m_Player[1] + (2 * gridLength) * 4 - (gridLength - 1) * (gridLength * 4) < 0 || 
			m_Player[0] + m_Player[1] + (2 * gridLength) * 4 - (gridLength - 1) * (gridLength * 4) > (gridLength - 1) * gridLength * 4 + gridLength * 4) {
			std::cout << "Error: PlayerMovementA if m_Player[0] + m_Player[1] + (2 * gridLength) * 4 - (gridLength * 4) " 
				<< m_Player[0] + m_Player[1] + (2 * gridLength) * 4 - (gridLength - 1) * (gridLength * 4) << ", " << vertices1.size() <<
				std::endl;
		}
#endif
		vertices1[m_Player[0] + m_Player[1] + (2 * gridLength) * 4 - (gridLength - 1) * (gridLength * 4)].m_Wave -= well;
	}
	else 
	{
#ifdef DetectArrayError
		if (m_Player[0] + m_Player[1] + (2 * gridLength) * 4 < 0 || 
			m_Player[0] + m_Player[1] + (2 * gridLength) * 4 > (gridLength - 1) * gridLength * 4 + gridLength*4) {
			std::cout << "Error: PlayerMovementA else m_Player[0] + m_Player[1] + (2 * gridLength) * 4" 
				<< m_Player[0] + m_Player[1] + (2 * gridLength) * 4 << ", " << vertices1.size() <<
				std::endl;
		}
#endif

		vertices1[m_Player[0] + m_Player[1] + (2 * gridLength) * 4].m_Wave -= well;
	}
	if (m_Player[1] - (gridLength) * 4 < 0) {
		
#ifdef DetectArrayError
		if (m_Player[0] + m_Player[1] - (gridLength) * 4 + (gridLength * 4) < 0 ||
			m_Player[0] + m_Player[1] - (gridLength) * 4 + (gridLength * 4) > (gridLength - 1) * gridLength * 4 + gridLength * 4) {
			std::cout << "Error: PlayerMovementA if m_Player[0] + m_Player[1] - (gridLength) * 4 + (gridLength * 4) "
				<< m_Player[0] + m_Player[1] - (gridLength) * 4 + (gridLength * 4) << ", " << vertices1.size() <<
				std::endl;
		}
#endif
		vertices1[m_Player[0] + m_Player[1] - (gridLength) * 4 + (gridLength * 4)].m_Wave += well;
	}
	else {
		
#ifdef DetectArrayError
		if (m_Player[0] + m_Player[1] - (gridLength) * 4 < 0 ||
			m_Player[0] + m_Player[1] - (gridLength) * 4 > (gridLength - 1) * gridLength * 4 + gridLength * 4) {
			std::cout << "Error: PlayerMovementA else m_Player[0] + m_Player[1] - (gridLength) * 4"
				<< m_Player[0] + m_Player[1] - (gridLength) * 4 + (gridLength * 4) << ", " << vertices1.size() <<
				std::endl;
		}
#endif
		vertices1[m_Player[0] + m_Player[1] - (gridLength) * 4].m_Wave += well;
	}
	
}

void SandBox::PlayerMovementS() {
	//vertices1[m_Player[0] + m_Player[1]].CountVelocity[0] -= 0.3;
	if (m_Player[0] + 8 > (gridLength - 1) * 4) {
		vertices1[m_Player[0] + m_Player[1] - (gridLength) * 4 + 8].m_Wave -= well;
#ifdef DetectArrayError
		if (m_Player[0] + m_Player[1] - (gridLength) * 4 + 8 < 0 ||
			m_Player[0] + m_Player[1] - (gridLength) * 4 + 8 > (gridLength - 1) * gridLength * 4 + gridLength * 4) {
			std::cout << "Error: PlayerMovementS if m_Player[0] + m_Player[1] - (gridLength) * 4 + 8"
				<< m_Player[0] + m_Player[1] - (gridLength) * 4 + 8 << ", " << vertices1.size() <<
				std::endl;
		}
#endif
	}
	else
	{
#ifdef DetectArrayError
		if (m_Player[0] + m_Player[1] + 8 < 0 ||
			m_Player[0] + m_Player[1] + 8 > (gridLength - 1) * gridLength * 4 + gridLength * 4) {
			std::cout << "Error: PlayerMovementS else m_Player[0] + m_Player[1] + 8"
				<< m_Player[0] + m_Player[1] + 8 << ", " << vertices1.size() <<
				std::endl;
		}
#endif
		vertices1[m_Player[0] + m_Player[1] + 8].m_Wave -= well;
	}

	if (m_Player[0] - 4 < 0) {
#ifdef DetectArrayError
		if (m_Player[0] + m_Player[1] + (gridLength * 4) - 4 < 0 ||
			m_Player[0] + m_Player[1] + (gridLength * 4) - 4 > (gridLength - 1) * gridLength * 4 + gridLength * 4) {
			std::cout << "Error: PlayerMovementS if m_Player[0] + m_Player[1] + (gridLength * 4) - 4"
				<< m_Player[0] + m_Player[1] + (gridLength * 4) - 4 << ", " << vertices1.size() <<
				std::endl;
		}
#endif
		vertices1[m_Player[0] + m_Player[1] + (gridLength * 4) - 4].m_Wave += well;
	}
	else {
#ifdef DetectArrayError
		if (m_Player[0] + m_Player[1] - 4 < 0 ||
			m_Player[0] + m_Player[1] - 4 > (gridLength - 1) * gridLength * 4 + gridLength * 4) {
			std::cout << "Error: PlayerMovementS else m_Player[0] + m_Player[1] - 4"
				<< m_Player[0] + m_Player[1] - 4 << ", " << vertices1.size() <<
				std::endl;
		}
#endif
		vertices1[m_Player[0] + m_Player[1] - 4].m_Wave += well;
	}
	//vertices1[m_Player[0] + m_Player[1] + 12].m_Wave -= well;
	//vertices1[m_Player[0] + m_Player[1] - 4].m_Wave += well;
}

void SandBox::PlayerMovementD() {
	//vertices1[m_Player[0] + m_Player[1]].CountVelocity[1] += 0.3;
	if (m_Player[1] - (gridLength) * 4 < 0) {
		//std::cout <<  << std::endl;
#ifdef DetectArrayError
		if (m_Player[0] + m_Player[1] - (gridLength) * 4 + (gridLength * 4) < 0 ||
			m_Player[0] + m_Player[1] - (gridLength) * 4 + (gridLength * 4) > (gridLength - 1) * gridLength * 4 + gridLength * 4) {
			std::cout << "Error: PlayerMovementS if m_Player[0] + m_Player[1] - (gridLength) * 4 + (gridLength * 4)"
				<< m_Player[0] + m_Player[1] - (gridLength) * 4 + (gridLength * 4) << ", " << vertices1.size() <<
				std::endl;
		}
#endif
		vertices1[m_Player[0] + m_Player[1] - (gridLength) * 4 + (gridLength * 4)].m_Wave -= well;
	}
	else {
#ifdef DetectArrayError
		if (m_Player[0] + m_Player[1] - (gridLength) * 4 < 0 ||
			m_Player[0] + m_Player[1] - (gridLength) * 4 > (gridLength - 1) * gridLength * 4 + gridLength * 4) {
			std::cout << "Error: PlayerMovementS else m_Player[0] + m_Player[1] - (gridLength) * 4"
				<< m_Player[0] + m_Player[1] - (gridLength) * 4 << ", " << vertices1.size() <<
				std::endl;
		}
#endif
		vertices1[m_Player[0] + m_Player[1] - (gridLength) * 4].m_Wave -= well;
		//vertices1[m_Player[0] + m_Player[1] - (gridLength) * 4].m_Color = glm::vec4(0, 1, 0, 1);
	}
	//vertices1[m_Player[0] + m_Player[1] - (gridLength) * 4].m_Wave -= well;
	if (m_Player[1] + (gridLength) * 4 > (gridLength * 4) * (gridLength-1)) {
#ifdef DetectArrayError
		if (m_Player[0] + m_Player[1] + (gridLength) * 4 - (gridLength - 1) * (gridLength * 4) < 0 ||
			m_Player[0] + m_Player[1] + (gridLength) * 4 - (gridLength - 1) * (gridLength * 4) > (gridLength - 1) * gridLength * 4 + gridLength * 4) {
			std::cout << "Error: PlayerMovementS if m_Player[0] + m_Player[1] + (gridLength) * 4 - (gridLength - 1) * (gridLength * 4)"
				<< m_Player[0] + m_Player[1] + (gridLength) * 4 - (gridLength - 1) * (gridLength * 4) << ", " << vertices1.size() <<
				std::endl;
		}
#endif
		vertices1[m_Player[0] + m_Player[1] + (gridLength) * 4 - (gridLength - 1) * (gridLength * 4)].m_Wave += well;
	}
	else {
#ifdef DetectArrayError
		if (m_Player[0] + m_Player[1] + (gridLength) * 4 < 0 ||
			m_Player[0] + m_Player[1] + (gridLength) * 4 > (gridLength - 1) * gridLength * 4 + gridLength * 4) {
			std::cout << "Error: PlayerMovementS else m_Player[0] + m_Player[1] + (gridLength) * 4"
				<< m_Player[0] + m_Player[1] + (gridLength) * 4 << ", " << vertices1.size() <<
				std::endl;
		}
#endif
		vertices1[m_Player[0] + m_Player[1] + (gridLength) * 4].m_Wave += well;
		//vertices1[m_Player[0] + m_Player[1] + (gridLength) * 4].m_Color = glm::vec4(1, 0, 0, 1);
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
