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

#include <donercomponents/CDonerComponentsSystems.h>
#include <donercomponents/gameObject/CGameObject.h>
#include <donercomponents/component/CComponentFactoryManager.h>
#include <donercomponents/tags/CTagsManager.h>

#include <algorithm>
#include <cassert>

namespace DonerComponents
{
	CGameObject::CGameObject()
		: m_componentFactoryManager(*CDonerComponentsSystems::Get()->GetComponentFactoryManager())
		, m_gameObjectManager(*CDonerComponentsSystems::Get()->GetGameObjectManager())
		, m_tagsManager(*CDonerComponentsSystems::Get()->GetTagsManager())
		, m_numDeactivations(1)
		, m_initialized(false)
		, m_destroyed(false)
		, m_initiallyActive(true)
	{
		m_components.resize(m_componentFactoryManager.GetRegisteredComponentsAmount());
	}

	CGameObject::~CGameObject()
	{}

	CGameObject::operator CHandle()
	{
		CHandle handle;

		handle.m_elementType = CHandle::EElementType::GameObject;
		handle.m_componentIdx = 0;
		handle.m_elementPosition = m_gameObjectManager.GetPositionForElement(this);
		handle.m_version = GetVersion();

		return handle;
	}

	const CGameObject* CGameObject::operator=(const CHandle& rhs)
	{
		if (rhs.m_elementType == CHandle::EElementType::GameObject)
		{
			*this = m_gameObjectManager.GetElementByIdxAndVersion(rhs.m_elementPosition, rhs.m_version);
			return this;
		}
		return nullptr;
	}

	void CGameObject::SetParent(CGameObject* newParent)
	{
		CGameObject* oldParent = m_parent;
		m_parent = newParent;
		if (oldParent)
		{
			oldParent->RemoveChild(this);
		}
	}

	bool CGameObject::AddChild(CHandle newChild)
	{
		if (newChild && !HasChild(newChild))
		{
			CGameObject* gameObject = newChild;
			gameObject->SetParent(this);
			m_children.emplace_back(newChild);
			return true;
		}
		DC_WARNING_MSG(EErrorCode::GameObjectChildAlreadyExists, "Child already added to this gameObject");
		return false;
	}

	bool CGameObject::RemoveChild(CHandle child)
	{
		std::vector<CHandle>::iterator it = std::find(m_children.begin(), m_children.end(), child);
		if (it != m_children.end())
		{
			CGameObject* gameObject = (*it);
			m_children.erase(it);
			if (gameObject->GetParent() == *this)
			{
				gameObject->SetParent(nullptr);
			}
			return true;
		}
		return false;
	}

	bool CGameObject::HasChild(CHandle child) const
	{
		return std::find(m_children.begin(), m_children.end(), child) != m_children.end();
	}

	CHandle CGameObject::GetChildByName(const std::string& name)
	{
		for (CGameObject* child : m_children)
		{
			if (child && child->GetName() == name)
			{
				return child;
			}
		}
		DC_WARNING_MSG(EErrorCode::GameObjectChildNotFound, "Child %s doesn't belong to this gameObject", name.c_str());
		return CHandle();
	}

	CHandle CGameObject::GetChildByIndex(std::size_t index)
	{
		if (index < m_children.size())
		{
			return m_children[index];
		}
		DC_WARNING_MSG(EErrorCode::GameObjectChildNotFound, "Child in position %u doesn't belong to this gameObject", index);
		return CHandle();
	}

	CHandle CGameObject::AddComponent(CStrID nameId)
	{
		if (!HasComponent(nameId))
		{
			CComponent* component = m_componentFactoryManager.AddComponent(nameId, m_components);
			if (component)
			{
				component->SetOwner(this);
			}
			return component;
		}
		else
		{
			DC_WARNING_MSG(EErrorCode::ComponentdAlreadyFoundInGameObject, "Component %u already added", nameId);
			return CHandle();
		}
	}

	bool CGameObject::RemoveComponent(CStrID nameId)
	{
		int componentIdx = m_componentFactoryManager.GetFactoryIndexByName(nameId);
		if (componentIdx >= 0 && m_components[componentIdx])
		{
			m_components[componentIdx]->Destroy();
			m_components[componentIdx] = nullptr;
			return true;
		}
		DC_WARNING_MSG(EErrorCode::ComponentdNotFoundInGameObject, "Component %u hasn't been added to this gameObject", nameId);
		return false;
	}

	CHandle CGameObject::GetComponent(CStrID nameId)
	{
		int componentIdx = m_componentFactoryManager.GetFactoryIndexByName(nameId);
		if (componentIdx >= 0)
		{
			return m_components[componentIdx];
		}
		DC_WARNING_MSG(EErrorCode::ComponentdNotFoundInGameObject, "Component %u hasn't been added to this gameObject", nameId);
		return CHandle();
	}

	bool CGameObject::HasComponent(CStrID nameId) const
	{
		int componentIdx = m_componentFactoryManager.GetFactoryIndexByName(nameId);
		if (componentIdx >= 0)
		{
			return m_components[componentIdx] != nullptr;
		}
		return false;
	}

	void CGameObject::Init()
	{
		if (!m_initialized)
		{
			for (CComponent* component : m_components)
			{
				if (component)
				{
					component->Init();
				}
			}
			for (CGameObject* child : m_children)
			{
				if (child)
				{
					child->Init();
				}
			}

			m_initialized = true;
		}
	}

	void CGameObject::Destroy()
	{
		SetParent(nullptr);
		DestroyInternal();
	}

	void CGameObject::DestroyInternal()
	{
		if (!m_destroyed)
		{
			for (CComponent* component : m_components)
			{
				if (component)
				{
					component->Destroy();
				}
			}

			for (CGameObject* child : m_children)
			{
				if (child)
				{
					child->DestroyInternal();
				}
			}

			m_destroyed = true;
			m_gameObjectManager.ScheduleDestroy(this);
		}
	}

	void CGameObject::Activate()
	{
		if (m_initialized && m_numDeactivations > 0)
		{
			m_numDeactivations = 0;
			ActivateInternal();
		}
	}

	void CGameObject::Deactivate()
	{
		if (m_initialized)
		{
			if (m_numDeactivations == 0)
			{
				for (CComponent* component : m_components)
				{
					if (component)
					{
						component->Deactivate();
					}
				}
				++m_numDeactivations;
			}
			for (CGameObject* child : m_children)
			{
				if (child)
				{
					child->Deactivate();
				}
			}
		}
	}

	void CGameObject::ActivateFromParent()
	{
		if (m_numDeactivations > 0)
		{
			--m_numDeactivations;
			if (m_numDeactivations == 0)
			{
				ActivateInternal();
			}
		}
	}

	void CGameObject::ActivateInternal()
	{
		for (CComponent* component : m_components)
		{
			if (component)
			{
				component->ActivateFromParent();
			}
		}
		for (CGameObject* child : m_children)
		{
			if (child)
			{
				child->ActivateFromParent();
			}
		}
	}

	void CGameObject::CheckFirstActivation()
	{
		if (m_initialized)
		{
			for (CGameObject* child : m_children)
			{
				if (child)
				{
					child->CheckFirstActivationInternal();
				}
			}

			for (CComponent* component : m_components)
			{
				if (component)
				{
					component->CheckFirstActivation();
				}
			}

			if (GetIsInitiallyActive())
			{
				m_numDeactivations = 1;
				ActivateFromParent();
			}
			else
			{
				m_numDeactivations = 0;
				Deactivate();
			}
		}
	}

	void CGameObject::CheckFirstActivationInternal()
	{
		if (m_initialized)
		{
			if (!GetIsInitiallyActive())
			{
				++m_numDeactivations;
			}

			for (CComponent* component : m_components)
			{
				if (component)
				{
					component->CheckFirstActivation();
				}
			}

			for (CGameObject* child : m_children)
			{
				if (child)
				{
					child->CheckFirstActivationInternal();
				}
			}
		}
	}

	void CGameObject::CloneFrom(CGameObject* gameObject)
	{
		if (gameObject)
		{
			SetParent(nullptr);
			SetName(gameObject->GetName());

			m_tags.reset();
			m_tags |= gameObject->m_tags;

			m_numDeactivations = gameObject->m_numDeactivations;
			m_initialized = gameObject->m_initialized;
			m_destroyed = gameObject->m_destroyed;
			m_initiallyActive =gameObject->m_initiallyActive;

			m_componentFactoryManager.CloneComponents(gameObject->m_components, m_components);
			for (CComponent* component : m_components)
			{
				if (component)
				{
					component->SetOwner(this);
				}
			}

			for (CGameObject *child : gameObject->m_children)
			{
				if (child)
				{
					CGameObject* newChild = m_gameObjectManager.CreateGameObject();
					if (newChild)
					{
						newChild->CloneFrom(child);
						AddChild(newChild);
					}
					else
					{
						assert(false || "CGameObjectManager doesn't have enough free entities to perform a full copy");
					}
				}
			}
		}
	}

	// -------------------------
	// -- CGameObjectManager
	// -------------------------

	CGameObjectManager::CGameObjectManager()
		: CFactory(MAX_ENTITIES)
	{}


	CGameObject* CGameObjectManager::CreateGameObject()
	{
		CGameObject* gameObject = GetNewElement();
		if (!gameObject)
		{
			DC_ERROR_MSG(EErrorCode::NoMoreEntitiesAvailable, "No more entities available for creation at this point");
		}
		return gameObject;
	}

	bool CGameObjectManager::DestroyGameObject(CHandle handle)
	{
		CGameObject* gameObject = handle;
		if (FindElement(gameObject))
		{
			return DestroyElement(&gameObject);
		}
		DC_WARNING_MSG(EErrorCode::GameObjectNotRegisteredInFactory, "Trying to destroy an gameObject which hasn't been created using CGameObjectManager");
		return false;
	}

	void CGameObjectManager::SendPostMsgs()
	{
		std::vector<CPostMessageBase*> currentMsgs = m_postMsgs;
		m_postMsgs.clear();
		for (CPostMessageBase* postMsg : currentMsgs)
		{
			postMsg->SendMessage();
			delete postMsg;
		}
	}

	void CGameObjectManager::ScheduleDestroy(CHandle handle)
	{
		if (std::find(m_scheduledDestroys.begin(), m_scheduledDestroys.end(), handle) == m_scheduledDestroys.end())
		{
			m_scheduledDestroys.emplace_back(handle);
		}
	}

	void CGameObjectManager::ExecuteScheduledDestroys()
	{
		for (CHandle handle : m_scheduledDestroys)
		{
			DestroyGameObject(handle);
		}
		m_scheduledDestroys.clear();
	}
}
