#include "../pch.h"
#include "Mesh.h"

using namespace Doom;

void Mesh::Init()
{
	m_Layout = new VertexBufferLayout();
	m_Vb = new VertexBuffer(m_VertAttrib, m_VertAttribSize * sizeof(float));
	m_Va = new VertexArray();
	m_Ib = new IndexBuffer(m_Indices, m_IndicesSize);
	m_Layout->Push<float>(3);
	m_Layout->Push<float>(3);
	m_Layout->Push<float>(2);
	m_Layout->Push<float>(3);
	m_Layout->Push<float>(3);
	m_Va->AddBuffer(*this->m_Vb, *this->m_Layout);
	m_Va->UnBind();
	m_Vb->UnBind();
	m_Ib->UnBind();
}

Mesh::~Mesh()
{
	delete[] m_VertAttrib;
	delete[] m_Indices;
	delete m_Layout;
	delete m_Vb;
	delete m_Va;
	delete m_Ib;
}
