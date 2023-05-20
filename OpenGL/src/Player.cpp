#include "Player.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

float position[2] = {0.0f, 0.0f};
float scale = 0.1f;
unsigned int player_Buffer;
unsigned int player_Index_Buffer;

Player::Player()
{
	glGenBuffers(2, &player_Buffer);
	glGenBuffers(2, &player_Index_Buffer);
}

Player::~Player()
{
	glDeleteBuffers(2, &player_Buffer);
	glDeleteBuffers(2, &player_Index_Buffer);
}

float player_Vertex[] =
{
	-scale + position[0],  0.0f, //0
	 0.0f + position[0], -scale, //1
	 scale + position[0],  0.0f, //2
	 0.0f + position[0],  scale  //3
};

unsigned int player_Index_Vertex[] =
{
	0,1,2,
	2,3,0
};

void Player::Draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, player_Buffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), &player_Vertex, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, player_Index_Buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), &player_Index_Vertex, GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void Player::Update()
{

}

void Player::Input()
{
	position[0] += 0.01f;
	//position[1] += 0.01f;
}

