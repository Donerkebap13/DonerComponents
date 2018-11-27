////////////////////////////////////////////////////////////
//
// MIT License
//
// DonerComponents
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

#include <donercomponents/Defines.h>
#include <donercomponents/ErrorMessages.h>
#include <donercomponents/common/CFactory.h>
#include <donercomponents/component/CComponent.h>
#include <donercomponents/handle/CHandle.h>
#include <donercomponents/messages/CPostMsg.h>
#include <donercomponents/utils/hash/CStrID.h>
#include <donercomponents/tags/CTagsManager.h>

#include <vector>
#include <functional>
#include <stack>

namespace DonerComponents
{
	class CComponentFactoryManager;
	class CGameObjectManager;

	class CGameObject : public CFactoryElement
	{
		template<class CGameObject> friend class CFactory;
	public:
		operator CHandle();
		const CGameObject* operator=(const CHandle& rhs);

		template<typename T>
		CHandle AddComponent()
		{
			if (!HasComponent<T>())
			{
				CComponent* component = m_componentFactoryManager.AddComponent<T>(m_components);
				if (component)
				{
					component->SetOwner(this);
				}
				return component;
			}
			else
			{
				DC_WARNING_MSG(EErrorCode::ComponentdAlreadyFoundInGameObject, "Component already added");
				return CHandle();
			}
			
		}
		template<typename T>
		bool RemoveComponent()
		{
			if (!m_components.empty())
			{
				int componentIdx = m_componentFactoryManager.GetFactoryindex<T>();
				if (m_components[componentIdx])
				{
					return m_componentFactoryManager.DestroyComponent(&m_components[componentIdx]);
				}
			}
			return false;
		}
		template<typename T>
		CHandle GetComponent()
		{
			int componentIdx = m_componentFactoryManager.GetFactoryindex<T>();
			if (componentIdx >= 0)
			{
				return m_components[componentIdx];
			}
			return CHandle();
		}

		template<typename T>
		bool HasComponent() const
		{
			int componentIdx = m_componentFactoryManager.GetFactoryindex<T>();
			if (componentIdx >= 0)
			{
				return m_components[componentIdx] != nullptr;
			}
			return false;
		}

		void SetParent(CGameObject* newParent);
		CHandle GetParent() const { return m_parent; }
		bool AddChild(CHandle newChild);
		bool RemoveChild(CHandle child);
		bool HasChild(CHandle child) const;
		int GetChildrenCount() const { return m_children.size(); }
		CHandle GetChildByName(const std::string& name);
		CHandle GetChildByIndex(std::size_t index);

		CHandle AddComponent(CStrID nameId);
		bool RemoveComponent(CStrID nameId);
		CHandle GetComponent(CStrID nameId);
		bool HasComponent(CStrID nameId) const;

		template<typename... Args>
		bool AddTags(Args... args)
		{
			return m_tagsManager.AddTags(m_tags, std::forward<Args>(args)...);
		}
		template<typename... Args>
		bool RemoveTags(Args... args)
		{
			return m_tagsManager.RemoveTags(m_tags, std::forward<Args>(args)...);
		}
		template<typename... Args>
		bool HasTags(Args... args)
		{
			return m_tagsManager.HasAllTags(m_tags, std::forward<Args>(args)...);
		}
		template<typename... Args>
		bool HasAnyTag(Args... args)
		{
			return m_tagsManager.HasAnyTags(m_tags, std::forward<Args>(args)...);
		}
		template<typename... Args>
		std::vector<CHandle> GetChildrenWithTags(Args... args)
		{
			std::vector<CHandle> children;
			for (CGameObject* child : m_children)
			{
				if (child && child->HasTags(std::forward<Args>(args)...))
				{
					children.emplace_back(child);
				}
			}
			return children;
		}
		template<typename... Args>
		std::vector<CHandle> GetChildrenWithTagsRecursive(Args... args)
		{
			std::vector<CHandle> children;

			std::stack<CGameObject*> stack;
			for (CGameObject* child : m_children)
			{
				stack.push(child);
			}

			while (!stack.empty())
			{
				CGameObject* current = stack.top();
				stack.pop();

				if (current)
				{
					if (current->HasTags(std::forward<Args>(args)...))
					{
						children.emplace_back(current);
					}
					for (int i = 0; i < current->GetChildrenCount(); ++i)
					{
						stack.push(current->GetChildByIndex(i));
					}
				}
			}
			return children;
		}
		template<typename... Args>
		std::vector<CHandle> GetChildrenWithAnyTag(Args... args)
		{
			std::vector<CHandle> children;
			for (CGameObject* child : m_children)
			{
				if (child && child->HasAnyTag(std::forward<Args>(args)...))
				{
					children.emplace_back(child);
				}
			}
			return children;
		}
		template<typename... Args>
		std::vector<CHandle> GetChildrenWithAnyTagRecursive(Args... args)
		{
			std::vector<CHandle> children;

			std::stack<CGameObject*> stack;
			for (CGameObject* child : m_children)
			{
				stack.push(child);
			}

			while (!stack.empty())
			{
				CGameObject* current = stack.top();
				stack.pop();

				if (current)
				{
					if (current->HasAnyTag(std::forward<Args>(args)...))
					{
						children.emplace_back(current);
					}
					for (int i = 0; i < current->GetChildrenCount(); ++i)
					{
						stack.push(current->GetChildByIndex(i));
					}
				}
			}
			return children;
		}

		template<typename T>
		void SendMessage(const T& message, ESendMessageType type = ESendMessageType::NonRecursive)
		{
			if (IsActive() && !IsDestroyed())
			{
				for (CComponent* component : m_components)
				{
					if (component)
					{
						component->SendMessage(message);
					}
				}

				if (type == ESendMessageType::Recursive)
				{
					std::stack<CGameObject*> stack;
					for (CGameObject* child : m_children)
					{
						stack.push(child);
					}

					while (!stack.empty())
					{
						CGameObject* current = stack.top();
						stack.pop();
						current->SendMessage(message, type);
						for (int i = 0; i < current->GetChildrenCount(); ++i)
						{
							stack.push(current->GetChildByIndex(i));
						}
					}
				}
			}
		}

		template<typename T>
		void SendMessageToChildren(const T& message, ESendMessageType type = ESendMessageType::NonRecursive)
		{
			if (IsActive() && !IsDestroyed())
			{
				for (CGameObject* child : m_children)
				{
					if (child)
					{
						child->SendMessage(message, type);
					}
				}
			}
		}

		template<typename T>
		void PostMessage(const T& message, ESendMessageType type = ESendMessageType::NonRecursive);

		template<typename T>
		void PostMessageToChildren(const T& message, ESendMessageType type = ESendMessageType::NonRecursive);

		void Init();
		void Destroy();

		void Activate();
		void Deactivate();

		bool IsInitialized() const { return m_initialized; }
		bool IsActive() const { return m_numDeactivations == 0; }
		bool IsDestroyed() const { return m_destroyed; }

		void SetName(const std::string& name) { m_name = name; }
		const std::string& GetName() const { return m_name; }

		bool GetIsInitiallyActive() const { return m_initiallyActive; }
		void SetIsInitiallyActive(bool initiallyActive) { m_initiallyActive = initiallyActive; }
		void CheckFirstActivation();

		void CloneFrom(CGameObject* gameObject);

	private:
		CGameObject();
		~CGameObject();

		void DestroyInternal();

		void ActivateFromParent();
		void ActivateInternal();
		void CheckFirstActivationInternal();

		CHandle m_parent;
		std::vector<CHandle> m_children;

		std::vector<CComponent*> m_components;

		CComponentFactoryManager& m_componentFactoryManager;
		CGameObjectManager& m_gameObjectManager;
		CTagsManager& m_tagsManager;

		TagsMask m_tags;

		std::string m_name;

		int m_numDeactivations;
		bool m_initialized;
		bool m_destroyed;
		bool m_initiallyActive;
	};

	// -------------------------
	// -- CGameObjectManager
	// -------------------------

	class CGameObjectManager : public CFactory<CGameObject>
	{
		friend class CDonerComponentsSystems;
		friend class CGameObject;
	public:
		~CGameObjectManager() override {}

		template<typename T>
		void BroadcastMessage(const T& message)
		{
			for (SEntry& entry : m_entries)
			{
				if (entry.m_used)
				{
					entry.m_data->SendMessage(message);
				}
			}
		}

		template<typename T>
		void PostMessage(CHandle gameObject, const T& message)
		{
			m_postMsgs.emplace_back(new CPostMessage<T>(gameObject, message));
		}

		CGameObject* CreateGameObject();


		void SendPostMsgs();
		void ExecuteScheduledDestroys();

	private:
		CGameObjectManager();

		bool DestroyGameObject(CHandle handle);
		void ScheduleDestroy(CHandle handle);

		std::vector<CPostMessageBase*> m_postMsgs;
		std::vector<CHandle> m_scheduledDestroys;
	};

	template<typename T>
	void CGameObject::PostMessage(const T& message, ESendMessageType type/* = ESendMessageType::NonRecursive*/)
	{
		if (IsActive() && !IsDestroyed())
		{
			m_gameObjectManager.PostMessage(this, message);

			if (type == ESendMessageType::Recursive)
			{
				std::stack<CGameObject*> stack;
				for (CGameObject* child : m_children)
				{
					stack.push(child);
				}
				while (!stack.empty())
				{
					CGameObject* current = stack.top();
					stack.pop();
					if (current && current->IsActive() && !current->IsDestroyed())
					{
						m_gameObjectManager.PostMessage(current, message);

						for (int i = 0; i < current->GetChildrenCount(); ++i)
						{
							stack.push(current->GetChildByIndex(i));
						}
					}
				}
			}
		}
	}

	template<typename T>
	void CGameObject::PostMessageToChildren(const T& message, ESendMessageType type/* = ESendMessageType::NonRecursive*/)
	{
		if (IsActive() && !IsDestroyed())
		{
			for (CGameObject* child : m_children)
			{
				if (child)
				{
					child->PostMessage(message, type);
				}
			}
		}
	}

	// -------------------------
	// -- CHandle
	// -------------------------

	template<typename T>
	void CHandle::SendMessage(const T& message, ESendMessageType type/* = ESendMessageType::NonRecursive*/)
	{
		if (m_elementType == CHandle::EElementType::GameObject)
		{
			CGameObject* gameObject = *this;
			if (gameObject)
			{
				gameObject->SendMessage(message, type);
			}
		}
		else if (m_elementType == CHandle::EElementType::Component)
		{
			CComponent* component = *this;
			if (component)
			{
				component->SendMessage(message);
			}
		}
	}

	template<typename T>
	void CHandle::SendMessageToChildren(const T& message, ESendMessageType type/* = ESendMessageType::NonRecursive*/)
	{
		if (m_elementType == CHandle::EElementType::GameObject)
		{
			CGameObject* gameObject = *this;
			if (gameObject)
			{
				gameObject->SendMessageToChildren(message, type);
			}
		}
	}

	template<typename T>
	void CHandle::PostMessage(const T& message, ESendMessageType type/* = ESendMessageType::NonRecursive*/)
	{
		if (m_elementType == CHandle::EElementType::GameObject)
		{
			CGameObject* gameObject = *this;
			if (gameObject)
			{
				gameObject->PostMessage(message, type);
			}
		}
	}

	template<typename T>
	void CHandle::PostMessageToChildren(const T& message, ESendMessageType type/* = ESendMessageType::NonRecursive*/)
	{
		if (m_elementType == CHandle::EElementType::GameObject)
		{
			CGameObject* gameObject = *this;
			if (gameObject)
			{
				gameObject->PostMessageToChildren(message, type);
			}
		}
	}
}
