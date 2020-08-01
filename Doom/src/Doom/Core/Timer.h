#pragma once
#include <chrono>
#include <iomanip>

namespace Doom {

	class DOOM_API Timer {
		static double start;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimePoint;
	public:
		static void Until(double seconds, Task task) {

			if (start > seconds) {
				task();
				start = 0;
			}
			start += DeltaTime::GetDeltaTime();
		}

		Timer() {
			m_startTimePoint = std::chrono::high_resolution_clock::now();
		}

		~Timer() {
			Stop();
		}

		void Stop() {
			auto endTimePoint = std::chrono::high_resolution_clock::now();

			auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimePoint).time_since_epoch().count();

			auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

			auto duration = end - start;

			double ms = duration * 0.001;

			std::setprecision(6);
			std::cout << duration << "us (" << ms << "ms)\n";
		}
	};

}