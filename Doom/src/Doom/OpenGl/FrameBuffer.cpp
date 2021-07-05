#include "../pch.h"
#include "FrameBuffer.h"
#include "../Core/Window.h"

using namespace Doom;

FrameBuffer::FrameBuffer(const FrameBufferParams& params)
{
	m_Params = params;

	glGenFramebuffers(1, &m_Fbo);

	for (uint32_t i = 0; i < params.m_TexturesAmount; i++)
	{
		m_Textures.push_back(0);
		glGenTextures(1, &m_Textures[i]);
		glBindTexture(GL_TEXTURE_2D, m_Textures[i]);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			params.m_TextureInternalFormat,
			params.m_Width,
			params.m_Height,
			0,
			params.m_TextureInternalFormat,
			params.TextureType,
			NULL);

		for (size_t i = 0; i < Texture::s_TexParameters.size(); i++)
		{
			glTexParameteri(Texture::s_TexParameters[i].m_Target, Texture::s_TexParameters[i].m_Pname, Texture::s_TexParameters[i].m_Param);
			if (Texture::s_TexParameters[i].m_Param == GL_CLAMP_TO_BORDER)
			{
				float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
			}
		}

		glBindTexture(GL_TEXTURE_2D, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, params.m_FrameBufferAttachment + i, GL_TEXTURE_2D, m_Textures[i], 0);
	}

	if (params.m_HasRBO)
	{
		glGenRenderbuffers(1, &m_Rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, m_Rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, params.m_Width, params.m_Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Rbo);
	}

	if(!params.m_ReadBuffer) glReadBuffer(GL_NONE);
	if(!params.m_DrawBuffer) glDrawBuffer(GL_NONE);
	

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Logger::Error("Framebuffer is not complete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &m_Fbo);
	glDeleteRenderbuffers(1, &m_Rbo);
}

void Doom::FrameBuffer::Resize(uint32_t width, uint32_t height)
{
	m_Params.m_Width = width;
	m_Params.m_Height = height;
	for (uint32_t i = 0; i < m_Textures.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_Textures[i]);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			m_Params.m_TextureInternalFormat,
			m_Params.m_Width,
			m_Params.m_Height,
			0,
			m_Params.m_TextureInternalFormat,
			m_Params.TextureType,
			NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
		if (m_Params.m_HasRBO)
		{
			glBindRenderbuffer(GL_RENDERBUFFER, m_Rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Params.m_Width, m_Params.m_Height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Rbo);
		}
	}
}

void FrameBuffer::Bind() const
{
	//glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
}


void FrameBuffer::UnBind() const
{
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}