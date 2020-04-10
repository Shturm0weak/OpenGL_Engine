#pragma once
#include <map>

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
		static std::vector<Texture*> texturesArray;
		static double VRAMused;
		void Bind(unsigned int slot = 0) const;
		void UnBind() const;

		inline int GetWidth() const { return m_width; }
		inline int GetHeight() const { return m_height; }

		friend class Editor;
	};

}