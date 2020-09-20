#pragma once
#include <Kortex/Kortex.hpp>
#include <kxf/EventSystem/EventBroadcastProcessor.h>

namespace Kortex
{
	class BroadcastReciever;
}

namespace Kortex
{
	class BroadcastProcessor: public kxf::EventBroadcastProcessor
	{
		public:
			static BroadcastProcessor& Get();

		public:
			BroadcastProcessor() = default;

		public:
			bool AddReciever(BroadcastReciever& reciever);
			bool RemoveReciever(BroadcastReciever& reciever);
	};
}

namespace Kortex
{
	class BroadcastReciever: public kxf::EventBroadcastReciever
	{
		public:
			BroadcastReciever(BroadcastProcessor& processor)
				:KxBroadcastReciever(processor)
			{
			}
			BroadcastReciever()
				:KxBroadcastReciever(BroadcastProcessor::Get())
			{
			}
	
		public:
			BroadcastProcessor& GetProcessor()
			{
				return static_cast<BroadcastProcessor&>(kxf::EventBroadcastReciever::GetProcessor());
			}
			const BroadcastProcessor& GetProcessor() const
			{
				return static_cast<const BroadcastProcessor&>(kxf::EventBroadcastReciever::GetProcessor());
			}
	};
}

namespace Kortex
{
	class BroadcastEvent: public kxf::BasicEvent
	{
		public:
			BroadcastEvent() = default;

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<BroadcastEvent>(std::move(*this));
			}
	};
}
