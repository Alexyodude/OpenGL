#pragma once

#include <GL/glew.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

static int Window_Width = 1000;
static int Window_Height = 500;

class Renderer
{
public:
	Renderer();
	~Renderer();

	void SetClearColor(float r, float g, float b, float a) const;
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	void Clear() const;
private:

};