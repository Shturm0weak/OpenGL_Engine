#include "Application.h"
#include "EntryPoint.h"
#include "Audio/SoundManager.h"
#include "src/Examples/RayCastTest.h"
#include "src/Examples/TicTacToe.h"
#include "src/Examples/StressTest.h"
#include "src/SpaceFire/SpaceFire.h"
#include "src/Minecraft/Minecraft.h"
#include "src/Examples/BlankProject.h"
#include "FEM/FEM.h"
#include "AimTrainer/AimTrainer.h"
#include "Examples/TestCollision2D.h"
#include "Examples/StressTest2D.h"
#include "Hexagon/Hexagon.h"
#include "Examples/Test.h"

int main()
{
	FEM app;
	Doom::EntryPoint entrypoint(&app);
	entrypoint.Run();
	return 0;
}