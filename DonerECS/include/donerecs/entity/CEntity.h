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

#include <donerecs/ErrorMessages.h>
#include <donerecs/handle/CHandle.h>
#include <donerecs/common/CECSElement.h>
#include <donerecs/utils/hash/CStrID.h>
#include <donerecs/tags/CTagsManager.h>

#include <vector>
#include <functional>

namespace DonerECS
{
	class CComponent;
	class CComponentFactoryManager;

	class CEntity : public CECSElement
	{
		template<class CEntity> friend class CFactory;
	public:
		operator CHandle();
		const CEntity* operator=(const CHandle& rhs);

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
				DECS_WARNING_MSG(EErrorCode::ComponentdAlreadyFoundInEntity, "Component already added");
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

		void SetParent(CEntity* newParent);
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
			for (CEntity* child : m_children)
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
			for (CEntity* child : m_children)
			{
				if (child)
				{
					if (child->HasTags(std::forward<Args>(args)...))
					{
						children.emplace_back(child);
					}
					child->GetChildrenWithTagsRecursiveInternal(children, std::forward<Args>(args)...);
				}
			}
			return children;
		}
		template<typename... Args>
		std::vector<CHandle> GetChildrenWithAnyTag(Args... args)
		{
			std::vector<CHandle> children;
			for (CEntity* child : m_children)
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
			for (CEntity* child : m_children)
			{
				if (child)
				{
					if (child->HasAnyTag(std::forward<Args>(args)...))
					{
						children.emplace_back(child);
					}
					child->GetChildrenWithAnyTagRecursiveInternal(children, std::forward<Args>(args)...);
				}
			}
			return children;
		}

		template<typename T>
		void SendMessage(const T& message)
		{
			for (CComponent* component : m_components)
			{
				if (component)
				{
					component->SendMessage(message);
				}
			}
		}

		template<typename T>
		void SendMessageRecursive(const T& message)
		{
			SendMessage(message);

			for (CEntity* child : m_children)
			{
				if (child)
				{
					child->SendMessageRecursive(message);
				}
			}
		}

		template<typename T>
		void PostMessage(const T& message)
		{
			CEntityManager::Get()->PostMessage(this, message);
		}

		template<typename T>
		void PostMessageRecursive(const T& message)
		{
			PostMessage(message);

			for (CEntity* child : m_children)
			{
				if (child)
				{
					child->PostMessageRecursive(message);
				}
			}
		}

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

		void CloneFrom(CEntity* entity);

	private:
		CEntity();
		~CEntity();

		void ActivateFromParent();
		void ActivateInternal();
		void CheckFirstActivationInternal();

		template<typename... Args>
		void GetChildrenWithTagsRecursiveInternal(std::vector<CHandle>& children, Args... args)
		{
			for (CEntity* child : m_children)
			{
				if (child)
				{
					if (child->HasTags(std::forward<Args>(args)...))
					{
						children.emplace_back(child);
					}
					child->GetChildrenWithTagsRecursiveInternal(children, std::forward<Args>(args)...);
				}
			}
		}

		template<typename... Args>
		void GetChildrenWithAnyTagRecursiveInternal(std::vector<CHandle>& children, Args... args)
		{
			for (CEntity* child : m_children)
			{
				if (child)
				{
					if (child->HasAnyTag(std::forward<Args>(args)...))
					{
						children.emplace_back(child);
					}
					child->GetChildrenWithAnyTagRecursiveInternal(children, std::forward<Args>(args)...);
				}
			}
		}

		CHandle m_parent;
		std::vector<CHandle> m_children;

		std::vector<CComponent*> m_components;

		CComponentFactoryManager& m_componentFactoryManager;
		CTagsManager& m_tagsManager;

		TagsMask m_tags;

		std::string m_name;

		int m_numDeactivations;
		bool m_initialized;
		bool m_destroyed;
		bool m_initiallyActive;
	};
}
