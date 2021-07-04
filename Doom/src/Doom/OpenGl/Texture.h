#pragma once
#include <map>
#include <vector>
#include "../Enums/ColoredOutput.h"
#include "../Core/ThreadPool.h"
#include <utility>

namespace Doom {

	struct DOOM_API TexParameteri {
		GLenum m_Target;
		GLenum m_Pname;
		GLint m_Param;
	};

	class DOOM_API Texture {
	private:

		static std::multimap<std::string, std::function<void(Texture* t)>> s_WaitingForTextures;
		static std::mutex s_LockTextureLoadingMtx;
		static std::vector<Texture*> s_LoadedTextures;
		static std::unordered_map<std::string, Texture*> s_Textures;

		unsigned char* m_LocalBuffer = nullptr;

		Texture(const std::string& path, int flip = 1);
		Texture();

		static void DispatchLoadedTextures();

		friend class SkyBox;
		friend class Editor;
		friend class EntryPoint;
	public:

		static Texture* s_WhiteTexture;
		static std::vector<TexParameteri> s_TexParameters;

		std::string m_FilePath;
		std::string m_Name;
		unsigned int m_RendererID = UINT_MAX;
		int m_width = 0, m_height = 0, m_BPP = 0;

		void Bind(unsigned int slot = 0) const;
		void UnBind() const;

		static void ShutDown();
		static void Delete(Texture* texture);
		static void AsyncCreate(const std::string& filePath);
		static std::vector<Texture*> GetLoadedTexturesFromFolder(const std::string& filePath);
		static Texture* Get(const std::string filePath, bool showErrors = true);
		static void AsyncGet(std::function<void(Texture* t)> function, std::string filePath);
		static void RemoveFromGetAsync(const std::string& filePath);
		static Texture* ColoredTexture(const std::string& name, uint32_t color);
		static Texture* Create(const std::string& filePath, bool flip = true);
		static bool UnloadFromRAM(const std::string& filePath);
		static bool UnloadFromVRAM(const std::string& filePath);
		static bool LoadTextureInRAM(const std::string& filePath, bool flip = false);
		static bool LoadTextureInVRAM(const std::string& filePath, bool unloadFromRam = true);
		static unsigned int LoadCubeMap(std::vector<std::string>& faces);

		~Texture();
	};

}