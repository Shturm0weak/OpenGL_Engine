#pragma once

namespace Doom {

	class LuaState;

	class DOOM_API ScriptComponent : public Component
	{
	private:
		LuaState* lState = nullptr;
	public:
		void AssignScript(const char* filePath);
		ScriptComponent();
		~ScriptComponent();
	};

}

