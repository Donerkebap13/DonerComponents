////////////////////////////////////////////////////////////
//
// MIT License
//
// DonerECS - A Tweaked Entity-Component System
// Copyright(c) 2017 Donerkebap13
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
////////////////////////////////////////////////////////////

#pragma once

#include <donerecs/handle/CHandle.h>
#include <donerecs/common/CECSElement.h>
#include <donerecs/messages/CMsgHandler.h>
#include <donerecs/utils/hash/CTypeHasher.h>

#include <unordered_map>

namespace DonerECS
{
	namespace Json
	{
		class Value;
	}

	class CComponent : public CECSElement
	{
		template<class CComponent> friend class CFactory;
	public:
		virtual ~CComponent();

		void Init();
		void Update(float dt);
		void Destroy();

		operator CHandle();
		const CComponent* operator=(const CHandle& rhs);

		const CComponent& operator=(const CComponent& rhs)
		{
			m_owner = CHandle();
			m_numDeactivations = 1;
			m_initialized = false;
			m_destroyed = false;
			m_initiallyActive = true;
			return *this;
		}

		void SetOwner(CHandle parent) { m_owner = parent; }
		CHandle GetOwner() const { return m_owner; }

		virtual void RegisterMessages() {}

		template<typename T>
		void SendMessage(const T& message)
		{
			if (m_initialized && IsActive() && !m_destroyed)
			{
				static const CTypeHasher::HashId hash = CTypeHasher::Hash<T>();
				auto& registeredMsg = m_messages.find(hash);
				if (registeredMsg != m_messages.end())
				{
					(*registeredMsg).second->Execute(this, message);
				}
			}
		}

		void Activate();
		void Deactivate();

		bool IsInitialized() const { return m_initialized; }
		bool IsActive() const { return m_numDeactivations == 0; }
		bool IsDestroyed() const { return m_destroyed; }

		virtual void ParseAtts(const Json::Value& atts) {}

		bool GetIsInitiallyActive() const { return m_initiallyActive; }
		void SetIsInitiallyActive(bool initiallyActive) { m_initiallyActive = initiallyActive; }
		void CheckFirstActivation();
		void ActivateFromParent();

	protected:
		CComponent();

		void CheckFirstActivationInternal();

		virtual void DoInit() {}
		virtual void DoUpdate(float dt) {}
		virtual void DoDestroy() {}
		virtual void DoActivate() {}
		virtual void DoDeactivate() {}

		template<typename C, typename T>
		void RegisterMessage(void(C::*function)(const T& param))
		{
			m_messages[CTypeHasher::Hash<T>()] = new CMsgHandler<C, T>(function);
		}

		CHandle m_owner;

		std::unordered_map<CTypeHasher::HashId, CMsgHandlerBase*> m_messages;

		int m_numDeactivations;
		bool m_initialized;
		bool m_destroyed;
		bool m_initiallyActive;
	};
}
