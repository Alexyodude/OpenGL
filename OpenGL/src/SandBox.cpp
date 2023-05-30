#include "SandBox.h"

SandBox::SandBox()
	: m_Proj(glm::ortho(0.0f, (float)Window_Width, 0.0f, (float)Window_Height, -1.0f, 1.0f)),
	m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
	m_Translation(0, 0, 0), m_Renderer(), m_Layout(new VertexBufferLayout())
{
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	m_VAO = std::make_unique<VertexArray>();

	m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
	m_Shader->Bind();
	m_Shader->SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);

	int samplers[3] = { 0, 1, 2 };
	m_Shader->SetUniform1iv("u_Texture", 3, samplers);

	m_Layout->Push<float>(3);
	m_Layout->Push<float>(2);
	m_Layout->Push<unsigned int>(1);

	for (int i = 0; i <= 1000; i+= 2*sideLength + 10)
	{
		for (int j = 0; j <= 500; j += 4 * sideLength + 20) {
		SandBox::AddQuad(0 + i, 0 + j, 1);
		SandBox::AddQuad(0 + i, 10 + 2*sideLength + j, 2);
		}

	}
}

SandBox::~SandBox()
{
}


void SandBox::OnUpdate(float deltaTime)
{
}

void SandBox::OnRender()
{
	GLCall(glClearColor(0.6f, 0.6f, 0.6f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT));

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

void SandBox::AddQuad(float x, float y, unsigned int texindex)
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

	// Using typical arrays

	//vertices[0].m_Position = glm::vec3(-sideLength + x, -sideLength + y, 0.0f);
	//vertices[1].m_Position = glm::vec3(sideLength + x, -sideLength + y, 0.0f);
	//vertices[2].m_Position = glm::vec3(sideLength + x, sideLength + y, 0.0f);
	//vertices[3].m_Position = glm::vec3(-sideLength + x, sideLength + y, 0.0f);

	//vertices[0].m_TexCoord = glm::vec2(0.0f, 0.0f);
	//vertices[1].m_TexCoord = glm::vec2(1.0f, 0.0f);
	//vertices[2].m_TexCoord = glm::vec2(1.0f, 1.0f);
	//vertices[3].m_TexCoord = glm::vec2(0.0f, 1.0f);

	//vertices[0].m_TexIndex = texindex;
	//vertices[1].m_TexIndex = texindex;
	//vertices[2].m_TexIndex = texindex;
	//vertices[3].m_TexIndex = texindex;
	
	//Debug memory address in 24 Columns

	//std::cout << vertices << std::endl;
	//std::cout << &vertices[0] << std::endl;
	//std::cout << &vertices[1] << std::endl;

	//std::cout << sizeof(vertices) << std::endl;
	//std::cout << sizeof(Vertex) * 4 << std::endl;

	vertices1.emplace_back(Vertex(glm::vec3(-sideLength + x, -sideLength + y, 0.0f), glm::vec2(0.0f, 0.0f), texindex));
	vertices1.emplace_back(Vertex(glm::vec3( sideLength + x, -sideLength + y, 0.0f), glm::vec2(1.0f, 0.0f), texindex));
	vertices1.emplace_back(Vertex(glm::vec3( sideLength + x,  sideLength + y, 0.0f), glm::vec2(1.0f, 1.0f), texindex));
	vertices1.emplace_back(Vertex(glm::vec3(-sideLength + x,  sideLength + y, 0.0f), glm::vec2(0.0f, 1.0f), texindex));

	//std::cout << "Start v1" << '\n' << &vertices1 << std::endl;
	//std::cout << &vertices1[0] << std::endl;
	//std::cout << &(vertices1[0]) << std::endl;
	//std::cout << &(vertices1[1]) << std::endl;
	//std::cout << sizeof(&vertices1) << std::endl;
	//std::cout << sizeof(vertices1) << std::endl;
	//std::cout << sizeof(vertices1[1]) << std::endl;

	//std::cout << sizeof(vertices1[0]) << std::endl;
	//std::cout << vertices1.size() << std::endl;
	//for (auto i = 0; i < vertices1.size(); i++) {
	//	std::cout << sizeof(vertices1[i]) << std::endl;
	//}
	vertexCount += 4;
}

void SandBox::TileMap()
{
}

void SandBox::SetRenderer(Renderer* m_Renderer) {
	m_Renderer = m_Renderer;
}

void SandBox::SetTranslation(glm::vec3 m_Translate) {
	m_Translation += m_Translate;
}
