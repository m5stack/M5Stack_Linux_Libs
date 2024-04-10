#pragma once 
#include <list>
#include <iostream>
#include <algorithm>
#include "State.h"
using namespace std;

/// <summary>
/// 表示状态机。
/// </summary>
/// <typeparam name="TType"></typeparam>
template<typename TType>
class StateMachine
{
public:
	StateMachine()
	{
		m_currentState = nullptr;
	}
	~StateMachine()
	{
		for_each(m_states.begin(), m_states.end(), [](State<TType>* item) {delete item; }); 

		m_states.clear();
	}
	/// <summary>
	/// 添加状态。
	/// </summary>
	/// <param name="state"></param>
	void addState(State<TType>* state)
	{
		if (state != nullptr)
		{
			m_states.push_back(state);
		}
	}
	/// <summary>
	/// 初始的状态。
	/// </summary>
	/// <param name="state"></param>
	void initState(State<TType>* state)
	{
		m_currentState = state;
	}
	/// <summary>
	/// 启动状态机，失败返回false。
	/// </summary>
	/// <returns></returns>
	bool start()
	{ 
		if (!m_currentState)
		{
			return false;
		}
		m_currentState->onEnter();
		m_currentState->onExec();
		return true;
	}
	/// <summary>
	/// 执行处理。
	/// </summary>
	/// <param name="action"></param>
	/// <returns></returns>
	State<TType>* doAction(const Action<TType>& action)
	{
		auto state = m_currentState->doAction(action);
		if (!state)
		{
			return state;
		}
		// 状态未变化的情况。
		if (m_currentState != state)
		{
			m_currentState->onExit();
			m_currentState = state;
			m_currentState->onEnter();
		}
		m_currentState->onExec();
		return state;
	}
private:
	list<State<TType>*> m_states; // 状态机集合。
	State<TType>* m_currentState; // 当前状态。
};

