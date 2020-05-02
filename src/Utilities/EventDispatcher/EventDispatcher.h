// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
// and /or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <memory>
#include <functional>
#include <unordered_set>
#include <typeindex>
#include <map>
#include <algorithm>

#include "Utilities/Profiler/Profiler.h"

namespace MxEngine
{
	#define MAKE_EVENT_BASE(name) struct name { inline virtual size_t GetEventType() const = 0; virtual ~name() = default; }

	#define MAKE_EVENT \
	template<typename T> friend class EventDispatcher;\
	public: inline virtual size_t GetEventType() const override { return this->eventType; } private:\
	inline static size_t eventType

	template<typename EventBase>
	class EventDispatcher
	{
		using CallbackBaseFunction = std::function<void(EventBase&)>;
		using NamedCallback = std::pair<std::string, CallbackBaseFunction>;
		using CallbackList = std::vector<NamedCallback>;
		using EventList = std::vector<std::unique_ptr<EventBase>>;
		using EventTypeIndex = size_t;
		using EventTypeMap = std::map<std::type_index, EventTypeIndex>;

		EventList events;
		std::unordered_map<EventTypeIndex, CallbackList> callbacks;
		std::unordered_map<EventTypeIndex, CallbackList> toAddCache;
		std::vector<std::string> toRemoveCache;
		EventTypeMap typeMap;

		inline void ProcessEvent(EventBase& event)
		{
			for (const auto& [typeIndex, eventCallbacks] : this->callbacks)
			{
				for (const auto& [name, callback] : eventCallbacks)
				{
					callback(event);
				}
			}
		}

		template<typename EventType>
		inline void AddCallbackImpl(std::string name, CallbackBaseFunction func)
		{
			this->toAddCache[EventType::eventType].emplace_back(std::move(name), std::move(func));
		}

		inline void RemoveEventByName(CallbackList& callbacks, const std::string& name)
		{
			auto it = std::remove_if(callbacks.begin(), callbacks.end(), [&name](const auto& p)
			{
				return p.first == name;
			});
			callbacks.resize(it - callbacks.begin());
		}

		inline void RemoveQueuedEvents()
		{
			for (auto it = this->toRemoveCache.begin(); it != this->toRemoveCache.end(); it++)
			{
				for (auto& [event, callbacks] : this->callbacks)
				{
					RemoveEventByName(callbacks, *it); 
				}
			}
			this->toRemoveCache.clear();

			for (auto it = this->toAddCache.begin(); it != this->toAddCache.end(); it++)
			{
				auto& [event, funcs] = *it;
				for (auto& func : funcs)
				{
					callbacks[event].push_back(std::move(func));
				}
			}
			for (auto& list : this->toAddCache) list.second.clear();
		}

	public:
		EventDispatcher<EventBase> Clone()
		{
			EventDispatcher<EventBase> cloned;
			cloned.typeMap = this->typeMap;
			return cloned;
		}

		template<typename EventType>
		inline void RegisterEventType()
		{
			if (EventType::eventType == 0)
			{
				auto& typeId = typeid(EventType);
				if (this->typeMap.find(typeId) == this->typeMap.end())
				{
					this->typeMap.emplace(typeId, typeMap.size() + 1);
				}
				EventType::eventType = this->typeMap[typeId];
			}
			else
			{
				this->typeMap[typeid(EventType)] = EventType::eventType;
			}
		}

		template<typename EventType>
		void AddEventListener(const std::string& name, std::function<void(EventType&)> func)
		{
			this->RegisterEventType<EventType>();
			this->AddCallbackImpl<EventType>(name, [func = std::move(func)](EventBase& e)
			{
				if (e.GetEventType() == EventType::eventType)
					func((EventType&)e);
			});
		}

		template<typename FunctionType>
		void AddEventListener(const std::string& name, FunctionType&& func)
		{
			this->AddEventListener(name, std::function(std::forward<FunctionType>(func)));
		}

		void RemoveEventListener(const std::string& name)
		{
			this->toRemoveCache.push_back(name);
		}

		void Invoke(EventBase& event)
		{
			this->RemoveQueuedEvents();
			this->ProcessEvent(event);
		}

		void AddEvent(std::unique_ptr<EventBase> event)
		{
			this->events.push_back(std::move(event));
		}

		void InvokeAll()
		{
			this->RemoveQueuedEvents();

			for (size_t i = 0; i < this->events.size(); i++)
			{
				MAKE_SCOPE_PROFILER(typeid(*this->events[i]).name());
				this->ProcessEvent(*this->events[i]);
			}
			this->events.clear();
		}
	};
}