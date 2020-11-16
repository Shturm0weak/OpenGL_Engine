#pragma once

namespace Doom {

	class DOOM_API SkyBox : public GameObject {

		uint32_t m_RendererID;

	public:
		SkyBox(std::vector<std::string> faces,Mesh* mesh);
		std::vector<std::string> faces;
	};

}