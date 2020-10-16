#include "../Core/Core.h"
#include <map>
#include <condition_variable>
#include <mutex>

namespace Doom {

	class DOOM_API Instancing {
	private:
		std::condition_variable cv;
		std::mutex m;
		std::atomic<bool>* ready = nullptr;
		uint32_t nThreads = ThreadPool::Instance()->GetAmountOfThreads();
		struct glBuffers{
			uint32_t prevObjectSize = 0;
			float* pos = nullptr;
			VertexBuffer* vbo = nullptr;
			VertexArray* vao = nullptr;
			VertexBuffer* vboDynamic = nullptr;
			VertexBufferLayout* layoutDynamic = nullptr;
			VertexBufferLayout* layout = nullptr;
			IndexBuffer* ibo;
		};

		Shader* shader = nullptr;
		uint32_t prev3DObjectsSize = 0;
		uint32_t sizeOfAttribs = 12 + 16;//pos, color,scale,(ambient,specular) + mat4 rotation

		std::map<Mesh*, glBuffers> buffers;
	public:
		float drawShadows = 1.0f;
		std::map<Mesh*, std::vector<Renderer3D*>> instancedObjects;
		Instancing();
		inline static Instancing* Instance() { static Instancing instance; return &instance; }
		void Create(Mesh* mesh);
		void Render();
		void BakeShadows();
		void PrepareVertexAtrrib();
		friend class Renderer;
	};

}