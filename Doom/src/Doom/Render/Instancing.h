#include "../Core/Core.h"
#include <map>

namespace Doom {

	

	class DOOM_API Instancing {
	private:
		struct glBuffers{
			VertexBuffer* vbo = nullptr;
			VertexArray* vao = nullptr;
			VertexBuffer* vboDynamic = nullptr;
			VertexBufferLayout* layoutDynamic = nullptr;
			VertexBufferLayout* layout = nullptr;
			IndexBuffer* ibo;
		};

		float* pos = nullptr;
		Shader* shader = nullptr;
		uint32_t prev3DObjectsSize = 0;
		std::map<Mesh*, glBuffers> buffers;
	public:
		std::map<Mesh*, std::vector<Renderer3D*>> instancedObjects;
		Instancing();
		inline static Instancing* Instance() { static Instancing instance; return &instance; }
		void Create(Mesh* mesh);
		void Render();
		friend class Renderer;
	};

}