#pragma once
#include <map>
#include <vector>

namespace Doom {

	class DOOM_API Texture {
	private:
		static std::map<unsigned int, unsigned int> Texturesused;
		std::string m_FilePath;
		unsigned char* m_LocalBuffer;
		int m_width, m_height, m_BPP;
		bool isExisted = false;
	public:
		unsigned int m_RendererID;
		Texture(const std::string& path, int flip = 1);
		Texture();
		Texture(std::string path, bool IsEmpty);
		~Texture();
		void GenTexture();
		static int bindedAmount;
		static Texture* WhiteTexture;
		static std::vector<Texture*> texturesArray;
		static double VRAMused;
		void Bind(unsigned int slot = 0) const;
		void UnBind() const;

		static void ShutDown();
		inline int GetWidth() const { return m_width; }
		inline int GetHeight() const { return m_height; }
		inline std::string GetFilePath() { return m_FilePath; }

		friend class Editor;
	};

}