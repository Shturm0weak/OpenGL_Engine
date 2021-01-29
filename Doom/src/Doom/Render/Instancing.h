#include "../Core/Core.h"
#include <map>
#include <condition_variable>
#include <mutex>

namespace Doom {

	class DOOM_API Instancing {
	private:

		struct glBuffers {
			VertexBuffer* m_Vbo = nullptr;
			VertexArray* m_Vao = nullptr;
			VertexBuffer* m_VboDynamic = nullptr;
			VertexBufferLayout* m_LayoutDynamic = nullptr;
			VertexBufferLayout* m_Layout = nullptr;
			IndexBuffer* m_Ibo;
			uint32_t m_PrevObjectSize = 0;
			float* m_VertAttrib = nullptr;
		};

		std::map<Mesh*, glBuffers> m_Buffers;
		std::condition_variable m_CondVar;
		std::mutex m_Mtx;
		std::atomic<bool>* m_Ready = nullptr;
		Shader* m_Shader = nullptr;
		uint32_t m_SizeOfAttribs = 12 + 16;//pos, color,scale,(ambient,specular) + mat4 rotation
		uint32_t m_NThreads = ThreadPool::GetInstance().GetAmountOfThreads();
	public:

		float m_DrawShadows = 1.0f; //TODO: Need to move somewhere
		std::map<Mesh*, std::vector<Renderer3D*>> m_InstancedObjects;

		Instancing();

		inline static Instancing* GetInstance() { static Instancing instance; return &instance; }
		void Create(Mesh* mesh);
		void Render();
		void BakeShadows();
		void PrepareVertexAtrrib();
		void ShutDown();
		friend class Renderer;
	};

}