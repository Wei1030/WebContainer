#pragma once
#include <memory>
#include <thread>
#include <condition_variable>
#include <set>
#include <chrono> 
#include "AnyTask.hpp"

class AnyTaskQueue;

class TaskItem
{
public:
	bool  operator==(const TaskItem& o) const
	{
		//永不相等
		return false;
	}

	bool operator<(const TaskItem& o) const
	{
		return m_expireTime < o.m_expireTime;
	}

protected:
	friend class AnyTaskQueue;

	TaskItem(){}

	template<class _Fx, class... _Args>
	TaskItem(const std::chrono::milliseconds& duration_time,_Fx&& _Func, _Args&&... _Ax)
	{
		m_expireTime = std::chrono::steady_clock::now() + duration_time;
		m_pTask = std::shared_ptr<_AnyTask>(new(std::nothrow) _AnyTaskImpl<_Fx, _Args...>(std::forward<_Fx>(_Func), std::forward<_Args>(_Ax)...));
	}

	void _Start()
	{
		if (m_pTask)
		{
			m_pTask->_Run();
		}
	}

	bool IsExpire() const
	{
		return std::chrono::steady_clock::now() >= m_expireTime;
	}

private:
	std::shared_ptr<_AnyTask> m_pTask;
	std::chrono::time_point<std::chrono::steady_clock> m_expireTime;
};

class AnyTaskQueue
{
public:
	AnyTaskQueue():m_bLoop(false){}
	~AnyTaskQueue() { Stop(); }

	bool Start()
	{
		if (m_srvThread.joinable())
		{
			return true;
		}

		m_bLoop = true;
		m_srvThread = std::thread(&AnyTaskQueue::run,this);
		return m_srvThread.joinable();
	}

	void Stop()
	{
		if (m_srvThread.joinable())
		{
			notify_to_stop();
			m_srvThread.join();
		}		
	}

	//************************************
	// Method:    PostTask
	// FullName:  TaskQueue::PostTask
	// Access:    public 
	// Returns:   void
	// Parameter: 任意函数;
	//************************************
	template<class _Fx, class... _Args>
	void PostTask(_Fx&& _Func, _Args&&... _Ax)
	{
		addTask(TaskItem(std::chrono::milliseconds(0),std::forward<_Fx>(_Func), std::forward<_Args>(_Ax)...));
	}

	template<class _Fx, class... _Args>
	void PostDelayTask(unsigned long long duration_time,_Fx&& _Func, _Args&&... _Ax)
	{
		addTask(TaskItem(std::chrono::milliseconds(duration_time), std::forward<_Fx>(_Func), std::forward<_Args>(_Ax)...));
	}

	//************************************
	// Method:    ClearAndThenPost
	// FullName:  TaskQueue::ClearAndThenPost
	// Access:    public 
	// Returns:   void
	// Parameter: 任意函数;
	//************************************
	template<class _Fx, class... _Args>
	void ClearAndThenPost(_Fx&& _Func, _Args&&... _Ax)
	{
		addTask(TaskItem(std::chrono::milliseconds(0),std::forward<_Fx>(_Func), std::forward<_Args>(_Ax)...),true);
	}

	template<class _Fx, class... _Args>
	void ClearAndThenPostDelay(unsigned long long duration_time, _Fx&& _Func, _Args&&... _Ax)
	{
		addTask(TaskItem(std::chrono::milliseconds(duration_time), std::forward<_Fx>(_Func), std::forward<_Args>(_Ax)...), true);
	}
private:
	void run()
	{
		while (m_bLoop)
		{
			TaskItem item;
			if (false == handleTasks(std::chrono::milliseconds(100),item))
			{
				continue;
			}			
			item._Start();
		}
	}

	void addTask(const TaskItem& Item, bool bClear = false)
	{
		std::lock_guard<std::mutex> guard(m_mtLock);

		if (bClear)	m_setTask.clear();

		m_setTask.insert(Item);
		m_cond.notify_all();
	}

	void notify_to_stop()
	{
		m_bLoop = false;
		addTask(TaskItem(), true);
	}

	bool handleTasks(const std::chrono::milliseconds& duration_time,TaskItem& Item)
	{
		std::unique_lock<std::mutex> guard(m_mtLock);

		while (m_setTask.empty()
			|| false == (*m_setTask.begin()).IsExpire())
		{
			if (std::cv_status::timeout == m_cond.wait_for(guard, duration_time))
			{
				return false;
			}			
		}		
		const auto& it = m_setTask.begin();		
		Item = *it;
		m_setTask.erase(it);
		return true;
	}

private:
	std::thread m_srvThread;
	std::mutex m_mtLock;
	std::condition_variable m_cond;
	std::set<TaskItem> m_setTask;

	bool	m_bLoop;
};

