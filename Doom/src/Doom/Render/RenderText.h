#pragma once

namespace Doom {

	class DOOM_API RenderText {
	public:
		virtual ~RenderText() {}
		virtual void OnRunning(OrthographicCamera& camera) {}
		virtual bool GetIsUpdating() { return false; }

	};

}