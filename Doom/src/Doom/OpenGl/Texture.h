#pragma once
#include <map>
#include <vector>
#include "../Enums/ColoredOutput.h"
#include "../Core/ThreadPool.h"

namespace Doom {

	class DOOM_API Texture {
	private:

		static std::unordered_map<std::string, Texture*> s_Textures;
		static bool s_IsTextureAdded;
		unsigned char* m_LocalBuffer = nullptr;

		Texture(const std::string& path, int flip = 1, bool repeat = false);
		Texture();

		static void DispatchLoadedTextures();

		friend class SkyBox;
		friend class Editor;
		friend class EntryPoint;
	public:

		static std::unordered_map<void*, std::function<Texture* ()>> s_WaitingForTextures;
		static std::mutex s_LockTextureLoadingMtx;
		static std::vector<Texture*> s_LoadedTextures;
		static Texture* s_WhiteTexture;

		std::string m_FilePath;
		std::string m_Name;
		unsigned int m_RendererID = UINT_MAX;
		int m_width = 0, m_height = 0, m_BPP = 0;

		void Bind(unsigned int slot = 0) const;
		void UnBind() const;

		static void ShutDown();
		static void Delete(Texture* texture);
		static void AsyncLoadTexture(const std::string& filePath);
		static std::vector<Texture*> GetLoadedTexturesFromFolder(std::string filePath);
		static Texture* Get(const std::string filePath, bool showErrors = true);
		static void GetAsync(void* ptr, std::function<Texture* ()> f);
		static void RemoveFromGetAsync(void* ptr);
		static Texture* ColoredTexture(const std::string& name, uint32_t color);
		static Texture* Create(const std::string& filePath, bool flip = true, bool repeat = false);
		static bool UnloadFromRAM(const std::string& filePath);
		static bool UnloadFromVRAM(const std::string& filePath);
		static bool LoadTextureInRAM(const std::string& filePath, bool flip = false);
		static bool LoadTextureInVRAM(const std::string& filePath, bool unloadFromRam = true);
		static unsigned int LoadCubeMap(std::vector<std::string> faces);

		~Texture();
	};

}