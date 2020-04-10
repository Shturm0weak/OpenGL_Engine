#pragma once

namespace Doom {

	class GuiItem {
		struct Position {
			float x = 0;
			float y = 0;
		};
		struct Scale {
			float x = 1;
			float y = 1;
		};


		glm::mat4 scale = glm::mat4(1.f);
		glm::mat4 pos = glm::mat4(1.f);
		glm::mat4 viewXscale = glm::mat4(1.f);
		glm::mat4 MVP = glm::mat4(1.f);
		glm::mat4 view = glm::mat4(1.f);

		unsigned int indeces2D[6] = { 0,1,2,3,2,0 };
		float mesh2D[20] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f
		};

		glm::mat4 ViewProjecTionRelatedToScreen = glm::ortho(-16.f, 16.f, -9.f, 9.f, 1.f, -1.f);
		glm::vec4 color = glm::vec4(82, 82, 82, 1);
		GLuint vao;
		VertexBufferLayout* layout = new VertexBufferLayout;
		VertexBuffer* vb;
		VertexArray* va = new VertexArray;
		IndexBuffer* ib = new IndexBuffer(indeces2D, 6);
		Texture* texture = nullptr;
		Shader* shader = nullptr;
		Position position;
		Scale _scale;

		friend class Button;
		friend class Batch;
	public:
		void Scale(float scalex, float scaley);
		void Translate(float x, float y);
	};

}