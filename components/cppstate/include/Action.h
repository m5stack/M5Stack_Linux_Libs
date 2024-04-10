#pragma once
// 表示动作。
template<typename TType>	
class Action
{
public: 
	Action()
	{

	}
	Action(const TType& type)
	{
		setType(type);
	}
	/// <summary>
	/// 类型。
	/// </summary>
	/// <returns></returns>
	TType type() const {
		return m_type;
	}

	/// <summary>
	///  设置类型。
	/// </summary>
	/// <param name="type"></param>
	void setType(const TType &type)
	{
		m_type = type;
	}
private: 
	TType m_type;
};

