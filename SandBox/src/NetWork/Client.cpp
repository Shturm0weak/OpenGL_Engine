#define OLC_PGE_APPLICATION
#include "olcPGEX_Network.h"
#include "Client.h"

//using namespace olc::net;

void Doom::Client::OnStart()
{
	if (this->Connect("127.0.0.1", 60000))
	{
		std::cout << "Connected" << std::endl;
		return;
	}
	std::cout << "Connection failed" << std::endl;
}

void Doom::Client::OnUpdate()
{
}

void Doom::Client::OnClose()
{
}

void Doom::Client::OnGuiRender()
{
}
