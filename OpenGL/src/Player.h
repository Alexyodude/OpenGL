#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
class Player
{
	float position[2];
	float scale;
	float player_Vertex[8];
	unsigned int player_Index_Vertex[6];
	unsigned int player_Buffer;
	unsigned int player_Index_Buffer;

	public:
		Player();
		~Player();
	void Draw();
	void Update();
	void Input();
};

//string PlayerVertexShader =
//"#version 330 core\n"
//"\n"
//"layout(location = 0) in vec4 position;\n"
//"\n"
//"void main()\n"
//"{\n"
//"	gl_Position = position;\n"
//"}\n";
//
//string PlayerFragmentShader =
//"#version 330 core\n"
//"\n"
//"layout(location = 0) out vec4 color;\n"
//"\n"
//"void main()\n"
//"{\n"
//"	color = vec4(1.0, 0.0, 0.0, 1.0);\n"
//"}\n";

