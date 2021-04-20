#include "../pch.h"
#include "ThreadPool.h"
#include <iostream>
using namespace std;
using namespace Doom;

ThreadPool::ThreadPool(int n)
{
	m_NumThreads = n;
	InfiniteLoopFunction();
}

ThreadPool::~ThreadPool()
{
	Shutdown();
}

void ThreadPool::InfiniteLoopFunction()
{
	for (unsigned int i = 0u; i < m_NumThreads; ++i)
	{
		m_Threads.emplace_back([=] {
				while (true) 
				{
					if (m_MainId == std::this_thread::get_id())
						return;
					auto iter = m_IsThreadBusy.find(std::this_thread::get_id());
					
					Task task;
					{
						std::unique_lock <std::mutex> umutex(m_Mutex);
						m_CondVar.wait(umutex, [=] { return m_IsStoped || !m_Tasks.empty(); });
						if (m_Tasks.empty() && m_IsStoped) break;

						task = std::move(m_Tasks.front());
						m_Tasks.pop();

						if (iter != m_IsThreadBusy.end())
							iter->second = true;
						//std::cout << std::this_thread::get_id() << std::endl;
					}
   					task();
					if (iter != m_IsThreadBusy.end())
						iter->second = false;
				}
		});
		m_IsThreadBusy.insert(std::make_pair(m_Threads.back().get_id(),false));
	}
}
void ThreadPool::Shutdown()
{
	{
		std::unique_lock <std::mutex> umutex(m_Mutex);
		m_IsStoped = true;
	}
	m_CondVar.notify_all();
	for (thread& thread : m_Threads)
	{
		thread.detach();
	}
}

void ThreadPool::Init()
{
	if (m_IsInitialized == false)
	{
		s_Instance = new ThreadPool(thread::hardware_concurrency());
		Logger::Success("Thread pool has been initialized!");
		m_IsInitialized = true;
	}
}

void ThreadPool::Enqueue(Task task)
{
	{
		std::unique_lock<std::mutex> lock{ m_Mutex };
		m_Tasks.emplace(std::move(task));
	}
	m_CondVar.notify_one();
}
