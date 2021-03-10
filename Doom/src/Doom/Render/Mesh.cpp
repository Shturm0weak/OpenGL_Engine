#include "../pch.h"
#include "Mesh.h"

using namespace Doom;

void Mesh::Init()
{
	if (m_IsInitialized)
		return;
	m_Va.Init();
	m_Vb.Init(m_VertAttrib, m_VertAttribSize * sizeof(float));
	m_Layout.Push<float>(3);
	m_Layout.Push<float>(3);
	m_Layout.Push<float>(2);
	m_Layout.Push<float>(3);
	m_Layout.Push<float>(3);
	m_Va.AddBuffer(m_Vb, m_Layout);
	m_Ib.Init(m_Indices, m_IndicesSize);
	m_Ib.UnBind();
	m_Vb.UnBind();
	m_Va.UnBind();

	m_IsInitialized = true;
}

Mesh::~Mesh()
{
	delete[] m_VertAttrib;
	delete[] m_Indices;
}
