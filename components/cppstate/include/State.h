#pragma once
#include <functional>
#include <list>
#include <algorithm>

#include "Transition.h"
#include "Action.h"
using namespace std;

/// <summary>
/// 表示状态。
/// </summary> 
/// <typeparam name="TType"></typeparam>
template<typename TType>
class State
{
public:
	State()
	{
	}

	~State()
	{
		for_each(m_trans.begin(), m_trans.end(), [](Transition<TType>* item) {delete item; });

		m_trans.clear();
	}
	/// <summary>
	/// 添加一个转换。
	/// </summary>
	/// <param name="type"></param>
	/// <param name="dest"></param>
	Transition<TType>* addTransition(const TType& type, State<TType>* dest)
	{
		Transition<TType>* transition = new Transition<TType>(type, dest);
		m_trans.push_back(transition);
		return transition;
	}
	/// <summary>
	/// 执行操作。
	/// </summary>
	/// <param name="action"></param>
	/// <returns>返回操作对应的状态。</returns>
	State<TType>* doAction(const Action<TType>& action)
	{
		Transition<TType>* transition = nullptr;
		 
		for(auto item = m_trans.begin(); item != m_trans.end(); item++)
		{ 
			if ((*item)->type() == action.type())
			{
				transition = (*item);
				break;
			}
		}
		State<TType>* state = nullptr;
		if (transition)
		{
			state = transition->state();
		}
		return state;
	}
public:
	/// <summary>
	/// 设置进入状态的执行回调。
	/// </summary>
	/// <param name="enterCall"></param>
	void setEnter(function<void()> enterCall)
	{
		m_enter = enterCall;
	}
	/// <summary>
	/// 设置执行状态的回调。
	/// </summary>
	/// <param name="execCall"></param>
	void setExec(function<void()> execCall)
	{
		m_exec = execCall;
	}
	/// <summary>
	/// 设置退出状态的回调。
	/// </summary>
	/// <param name="exitCall"></param>
	void setExit(function<void()> exitCall)
	{
		m_exit = exitCall;
	}
	/// <summary>
	/// 进入状态。
	/// </summary>
	void onEnter()
	{
		if (m_enter != nullptr)
		{
			m_enter();
		}
	}
	/// <summary>
	/// 执行状态。
	/// </summary>
	void onExec()
	{
		if (m_exec)
		{
			m_exec();
		}
	}
	/// <summary>
	/// 退出状态。
	/// </summary>
	void onExit()
	{
		if (m_exit)
		{
			m_exit();
		}
	}
private:
	list<Transition<TType> *>  m_trans;
	function<void()> m_enter;
	function<void()> m_exec;
	function<void()> m_exit;
};

