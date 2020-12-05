#pragma once

namespace Doom {

	class DOOM_API SkyBox : public GameObject {
	private:

		uint32_t m_RendererID;
	public:

		std::vector<std::string> m_Faces;

		SkyBox(std::vector<std::string> faces,Mesh* mesh);
	};

}