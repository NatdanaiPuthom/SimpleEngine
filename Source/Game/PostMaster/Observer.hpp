//#pragma once
//#include <any>
//
//namespace Simple
//{
//	template<typename MessageType>
//	struct Message
//	{
//		MessageType myType;
//		std::any myData;
//	};
//
//	template<typename MessageType>
//	class Observer
//	{
//		template<typename MessageType>
//		friend class PostMaster;
//	public:
//		virtual ~Observer() = default;
//	private:
//		virtual void ReciveMessage(const Message<MessageType>& aMessage) = 0;
//	};
//}