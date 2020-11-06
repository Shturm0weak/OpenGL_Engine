#include "../pch.h"
#include "Mesh.h"

using namespace Doom;

void Mesh::Init()
{
	layout = new VertexBufferLayout();
	vb = new VertexBuffer(mesh, meshSize * sizeof(float));
	va = new VertexArray();
	ib = new IndexBuffer(indicesForNormals, indicesSize);
	layout->Push<float>(3);
	layout->Push<float>(3);
	layout->Push<float>(2);
	layout->Push<float>(3);
	layout->Push<float>(3);
	va->AddBuffer(*this->vb, *this->layout);
	va->UnBind();
	vb->UnBind();
	ib->UnBind();
}

Mesh::~Mesh()
{
	delete[] mesh;
	delete[] indicesForNormals;
	delete layout;
	delete vb;
	delete va;
	delete ib;
}
