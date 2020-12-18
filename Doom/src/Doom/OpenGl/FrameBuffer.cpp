#include "../pch.h"
#include "FrameBuffer.h"
#include "../Core/Window.h"

using namespace Doom;

FrameBuffer::FrameBuffer(int width, int height, int TextureParameterinternalFormat, int TextureParametertextureType, int TextureParamLinear, int TextureParameterClamp,int FrameBufferAttachment,bool NeedRBO,bool ReadBuffer,bool DrawBuffer, int NumOfTextures)
{
	size = glm::vec2(width, height);

	glGenFramebuffers(1, &m_Fbo);

	for (uint32_t i = 0; i < NumOfTextures; i++)
	{
		m_Textures.push_back(0);
		glGenTextures(1, &m_Textures[i]);
		glBindTexture(GL_TEXTURE_2D, m_Textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, TextureParameterinternalFormat, width, height, 0, TextureParameterinternalFormat, TextureParametertextureType, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureParamLinear);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureParamLinear);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureParameterClamp);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureParameterClamp);
		if (TextureParameterClamp == GL_CLAMP_TO_BORDER) {
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, FrameBufferAttachment + i, GL_TEXTURE_2D, m_Textures[i], 0);
	}

	m_HasRbo = NeedRBO;
	if (NeedRBO) {
		glGenRenderbuffers(1, &m_Rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, m_Rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Rbo);
	}

	if(!ReadBuffer)
		glReadBuffer(GL_NONE);

	if(!DrawBuffer)
		glDrawBuffer(GL_NONE);
	

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &m_Fbo);
	glDeleteRenderbuffers(1, &m_Rbo);
}

void Doom::FrameBuffer::Resize(float width, float height)
{
	for (uint32_t i = 0; i < m_Textures.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_Textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
		if (m_HasRbo) {
			glBindRenderbuffer(GL_RENDERBUFFER, m_Rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Rbo);
		}
	}
}

void FrameBuffer::Bind()
{
	//glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
}


void FrameBuffer::UnBind()
{
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}