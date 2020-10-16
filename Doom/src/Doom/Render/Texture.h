#pragma once
#include <map>
#include <vector>
#include "../Core/ColoredOutput.h"

namespace Doom {

	class DOOM_API Texture {
	private:
		static std::map<std::string, Texture*> textures;
		std::string m_FilePath;
		std::string m_Name;
		unsigned char* m_LocalBuffer = nullptr;
		int m_width = 0, m_height = 0, m_BPP = 0;
		Texture(const std::string& path, int flip = 1);
		Texture();
		friend class SkyBox;
		friend class Editor;
	public:
		~Texture();
		unsigned int m_RendererID = -1;
		static int bindedAmount;
		static Texture* WhiteTexture;
		static double VRAMused;
		void Bind(unsigned int slot = 0) const;
		void UnBind() const;

		static void ShutDown();
		inline int GetWidth() const { return m_width; }
		inline int GetHeight() const { return m_height; }
		inline std::string GetFilePath() const { return m_FilePath; }

		static Texture* Get(std::string filePath, bool showErrors = true);
		static void UnloadFromRAM(std::string filePath);
		static void UnloadFromVRAM(std::string filePath);
		static Texture* ColoredTexture(std::string name, uint32_t color);
		static Texture* Create(std::string filePath, bool flip = true);
		static void LoadTextureInRAM(std::string filePath, bool flip = false);
		static void LoadTextureInVRAM(std::string filePath, bool unloadFromRam = true);
		static unsigned int LoadCubeMap(std::vector<std::string> faces);

		friend class Editor;
	};

}