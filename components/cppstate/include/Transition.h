#pragma once 


template<typename TType>class State;
/// <summary>
/// 表示状态转换。
/// </summary>
/// <typeparam name="TType"></typeparam>
template<typename TType>
class Transition
{
public:
	Transition()
	{
		m_state = nullptr;
	}
	/// <summary>
	/// 状态转换。
	/// </summary>
	/// <param name="type"></param>
	/// <param name="state"></param>
	Transition(const TType& type, State<TType> *state)
	{
		m_type = type;
		m_state = state;
	}
	/// <summary>
	/// 设置转换的类型。
	/// </summary>
	/// <param name="type"></param>
	void setType(TType type)
	{
		m_type = type;
	}
	/// <summary>
	/// 获取转换的类型。
	/// </summary>
	/// <returns></returns>
	TType type() const
	{
		return m_type;
	}
	/// <summary>
	/// 获取状态。
	/// </summary>
	/// <typeparam name="TType"></typeparam>
	State<TType>* state() const
	{
		return m_state;
	}
	/// <summary>
	/// 设置状态。
	/// </summary>
	/// <param name="state"></param>
	void setState(State<TType>* state)
	{
		m_state = state;
	}
private:
	TType m_type;
	State<TType> *m_state;
};

