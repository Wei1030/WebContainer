#pragma once
#include <memory>
#include <thread>
#include <condition_variable>
#include <list>

#include "AnyTask.hpp"

class AnyTaskQueue;

class TaskItem
{
protected:
	friend class AnyTaskQueue;

	TaskItem() {}

	template<class _Fx, class... _Args>
	TaskItem(_Fx&& _Func, _Args&&... _Ax)
	{
		m_pTask = std::shared_ptr<_AnyTask>(new(std::nothrow) _AnyTaskImpl<_Fx, _Args...>(std::forward<_Fx>(_Func), std::forward<_Args>(_Ax)...));
	}

	void _Start()
	{
		if (m_pTask)
		{
			m_pTask->_Run();
		}
	}

private:
	std::shared_ptr<_AnyTask> m_pTask;
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
		addTask(TaskItem(std::forward<_Fx>(_Func), std::forward<_Args>(_Ax)...));
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
		addTask(TaskItem(std::forward<_Fx>(_Func), std::forward<_Args>(_Ax)...),true);
	}
private:
	void run()
	{
		while (m_bLoop)
		{
			TaskItem item;
			handleTasks(item);
			item._Start();
		}
	}

	void addTask(const TaskItem& Item, bool bClear = false)
	{
		std::lock_guard<std::mutex> guard(m_mtLock);

		if (bClear)	m_listTask.clear();		

		m_listTask.push_back(Item);
		m_cond.notify_all();
	}

	void notify_to_stop()
	{
		m_bLoop = false;
		addTask(TaskItem(), true);
	}

	void handleTasks(TaskItem& Item)
	{
		std::unique_lock<std::mutex> guard(m_mtLock);

		m_cond.wait(guard, [this] {return !m_listTask.empty(); });

		Item = m_listTask.front();
		m_listTask.pop_front();
	}

private:
	std::thread m_srvThread;
	std::mutex m_mtLock;
	std::condition_variable m_cond;
	std::list<TaskItem> m_listTask;

	bool	m_bLoop;
};

