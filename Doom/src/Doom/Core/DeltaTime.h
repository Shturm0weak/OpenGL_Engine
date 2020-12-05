#ifndef  DELTATIME_H
#define DELTATIME_H
#include <mutex>

namespace Doom {

	class DOOM_API DeltaTime {
	private:

		static std::mutex s_Mtx;
		static float s_Time;
		static float s_Lasttime;
	public:

		static float s_Deltatime;
		static float GetDeltaTime() { return s_Deltatime; };
		static double calculateDeltaTime() {
			std::lock_guard<std::mutex> lock(s_Mtx);
			s_Time = (float)glfwGetTime();
			s_Deltatime = fabs(s_Lasttime - s_Time);
			s_Lasttime = s_Time;
			return s_Deltatime;
		};
	};

}

#endif // ! DELTATIME_H