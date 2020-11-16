#pragma once
#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <future>
#include "Core.h"

namespace Doom {

	using Task = std::function<void()>;
	class DOOM_API ThreadPool {
	private:

		static ThreadPool* thread_pool;
		std::vector<std::thread> m_Threads;
		std::map <std::thread::id,bool> isThreadBusy;
		std::mutex m_mutex;
		std::thread::id main = std::this_thread::get_id();
		std::condition_variable m_condition_var;
		std::queue <Task> m_Tasks;
		int num_Threads = 0;
		void Infinite_loop_function();
		bool m_stopping = false;
		static bool initialized;

	public:

		inline uint32_t GetAmountOfThreads() { return m_Threads.size(); }
		ThreadPool(int n);
		~ThreadPool();
		void Shutdown()noexcept;
		void Enqueue(Task task);
		static ThreadPool* GetInstance() { return thread_pool; }
		static void Init();

		friend class Editor;

	};
}