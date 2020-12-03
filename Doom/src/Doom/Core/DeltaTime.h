#ifndef  DELTATIME_H
#define DELTATIME_H
#include <mutex>

namespace Doom {

	class DOOM_API DeltaTime {
	private:

		static std::mutex m_Mtx;
		static float m_Time;
		static float m_Lasttime;
	public:

		static float m_Deltatime;
		static float GetDeltaTime() { return m_Deltatime; };
		static double calculateDeltaTime() {
			std::lock_guard<std::mutex> lock(m_Mtx);
			m_Time = (float)glfwGetTime();
			m_Deltatime = fabs(m_Lasttime - m_Time);
			m_Lasttime = m_Time;
			return m_Deltatime;
		};
	};

}

#endif // ! DELTATIME_H