#ifndef  DELTATIME_H
#define DELTATIME_H
#include <mutex>

namespace Doom {

	class DOOM_API DeltaTime {
	private:
		static std::mutex mtx;
		static float time;
		static float lasttime;
	public:
		static float deltatime;
		static float GetDeltaTime() { return deltatime; };
	public:
		static double calculateDeltaTime() {
			std::lock_guard<std::mutex> lock(mtx);
			time = (float)glfwGetTime();
			deltatime = fabs(lasttime - time);
			lasttime = time;
			return deltatime;
		};

	};

}

#endif // ! DELTATIME_H