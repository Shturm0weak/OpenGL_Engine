#include "../Core/Core.h"
#include <map>
#include <condition_variable>
#include <mutex>
#include <array>

namespace Doom {

	class DOOM_API Instancing {
	private:

		const unsigned int maxTextureSlots = 32;

		struct glBuffers {
			std::array<uint32_t, 32> m_TextureSlots;
			unsigned int m_TextureSlotsIndex = 0;
			VertexBuffer m_VboDynamic;
			VertexBufferLayout m_LayoutDynamic;
			uint32_t m_PrevObjectSize = 0;
			float* m_VertAttrib = nullptr;
		};

		std::map<Mesh*, glBuffers> m_Buffers;
		std::condition_variable m_CondVar;
		std::mutex m_Mtx;
		std::atomic<bool>* m_Ready = nullptr;
		Shader* m_Shader = nullptr;
		uint32_t m_SizeOfAttribs = 12 + 16 + 1 + 1;//pos, color, scale, (ambient,specular) + mat4 rotation + texture index + emissive
		uint32_t m_NThreads = ThreadPool::GetInstance().GetAmountOfThreads() - 1;
	public:

		bool m_FinishPrepareVertAtribb = true;
		std::map<Mesh*, std::vector<Renderer3D*>> m_InstancedObjects;

		Instancing();

		inline static Instancing* GetInstance() { static Instancing instance; return &instance; }
		void Create(Mesh* mesh);
		void Render();
		void BakeShadows();
		void PrepareVertexAtrrib();
		void ShutDown();

		friend class Renderer;
		friend class World;
		friend class Editor;
		friend class ParticleEmitter;
	};

}