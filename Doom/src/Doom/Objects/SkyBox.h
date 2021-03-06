#pragma once

namespace Doom {

	class DOOM_API SkyBox {
	public:

		static std::vector<std::string> s_Faces;
		static GameObject* CreateSkyBox(std::vector<std::string> faces);
	};

}