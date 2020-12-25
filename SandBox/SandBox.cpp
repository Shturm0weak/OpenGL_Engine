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

int main(){
	RayCastTest app;
	Doom::EntryPoint entrypoint(&app);
	entrypoint.Run();
	return 0;
}