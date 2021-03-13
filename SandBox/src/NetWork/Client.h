#pragma once

#include "Application.h"


namespace Doom {

	enum class GameMsg : uint32_t
	{
		Server_GetStatus,
		Server_GetPing,

		Client_Accepted,
		Client_AssignID,
		Client_RegisterWithServer,
		Client_UnregisterWithServer,

		Game_AddPlayer,
		Game_RemovePlayer,
		Game_UpdatePlayer,
	};

	struct sPlayerDescription
	{
		float fRadius = 0.5f;
	};

	class Client : public Application, public olc::net::client_interface<int> {
	private:
		
	public:

		Client(std::string name = "Client", int width = 800, int height = 600, bool Vsync = false) : Application(name, TYPE_3D, width, height, Vsync) {}

		void OnStart();
		void OnUpdate();
		void OnClose();
		void OnGuiRender();
	};

}