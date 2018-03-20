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

#include <donerecs/component/CComponent.h>
#include <donerecs/component/CComponentFactoryManager.h>
#include <donerecs/handle/CHandle.h>

#include <cassert>

namespace DonerECS
{
	CComponent* CComponentFactoryManager::CreateComponent(CStrID componentNameId)
	{
		IComponentFactory* factory = GetFactoryByName(componentNameId);
		if (factory)
		{
			return factory->CreateComponent();
		}
		else
		{
			DECS_ERROR_MSG(EErrorCode::ComponentFactoryNotRegistered, "There's no factory registered to create component %u", componentNameId);
			return nullptr;
		}
	}

	CComponent* CComponentFactoryManager::CloneComponent(CComponent* component, int componentIdx)
	{
		IComponentFactory* factory = GetFactoryByIndex(componentIdx);
		if (factory)
		{
			return factory->CloneComponent(component);
		}
		else
		{
			DECS_ERROR_MSG(EErrorCode::InvalidComponentFactoryIndex, "There's no factory registered with index %d", componentIdx);
			return nullptr;
		}
	}

	void CComponentFactoryManager::CloneComponents(std::vector<CComponent*>& src, std::vector<CComponent*>& dst)
	{
		assert(src.size() == dst.size());
		for (std::size_t i = 0; i < src.size(); ++i)
		{
			dst[i] = CloneComponent(src[i], i);
		}
	}

	CComponent* CComponentFactoryManager::AddComponent(CStrID componentNameId, std::vector<CComponent*>& components)
	{
		int factoryIdx = GetFactoryIndexByName(componentNameId);
		if (factoryIdx >= 0)
		{
			IComponentFactory* factory = GetFactoryByIndex(factoryIdx);
			if (factory && components[factoryIdx] == nullptr)
			{
				components[factoryIdx] = factory->CreateComponent();
				return components[factoryIdx];
			}
		}
		DECS_ERROR_MSG(EErrorCode::ComponentFactoryNotRegistered, "There's no factory registered to create component %u", componentNameId);
		return nullptr;
	}

	CComponent* CComponentFactoryManager::GetComponent(std::size_t componentTypeIdx, int index, int version)
	{
		if (componentTypeIdx < m_factories.size())
		{
			IComponentFactory* factory = m_factories[componentTypeIdx].m_address;
			if (factory)
			{
				return factory->GetByIdxAndVersion(index, version);
			}
		}
		DECS_ERROR_MSG(EErrorCode::InvalidComponentFactoryIndex, "There's no factory registered with index %d", index);
		return nullptr;
	}

	CHandle CComponentFactoryManager::SetHandleInfoFromComponent(CComponent* component)
	{
		CHandle handle;
		for (std::size_t i = 0; i < m_factories.size(); ++i)
		{
			if (m_factories[i].m_address->SetHandleInfoFromComponent(component, handle))
			{
				handle.m_componentIdx = i;
			}
		}
		return handle;
	}

	int CComponentFactoryManager::GetPositionForElement(CComponent* component)
	{
		for (std::size_t i = 0; i < m_factories.size(); ++i)
		{
			int pos = m_factories[i].m_address->GetComponentPosition(component);
			if (pos >= 0)
			{
				return pos;
			}
		}
		DECS_ERROR_MSG(EErrorCode::ComponentNotRegisteredInFactory, "Trying to get position for component created outside a factory");
		return -1;
	}

	bool CComponentFactoryManager::DestroyComponent(CComponent** component)
	{
		for (std::size_t i = 0; i < m_factories.size(); ++i)
		{
			if (m_factories[i].m_address->GetComponentPosition(*component) != -1)
			{
				(*component)->Destroy();
				if (m_factories[i].m_address->DestroyComponent(*component))
				{
					*component = nullptr;
					return true;
				}
			}
		}
		DECS_ERROR_MSG(EErrorCode::ComponentNotRegisteredInFactory, "Trying to destroy component created outside a factory");
		return false;
	}

	IComponentFactory* CComponentFactoryManager::GetFactoryByName(CStrID nameId)
	{
		for (SFactoryData& data : m_factories)
		{
			if (data.m_nameId == nameId)
			{
				return data.m_address;
			}
		}
		DECS_ERROR_MSG(EErrorCode::ComponentFactoryNotRegistered, "There's no factory registered with id %u", nameId);
		return nullptr;
	}

	IComponentFactory* CComponentFactoryManager::GetFactoryByIndex(std::size_t idx)
	{
		if (idx < m_factories.size())
		{
			return m_factories[idx].m_address;
		}
		DECS_ERROR_MSG(EErrorCode::InvalidComponentFactoryIndex, "There's no factory registered with index %u", idx);
		return nullptr;
	}

	int CComponentFactoryManager::GetFactoryIndexByName(CStrID nameId)
	{
		for (std::size_t i = 0; i < m_factories.size(); ++i)
		{
			if (m_factories[i].m_nameId == nameId)
			{
				return i;
			}
		}
		DECS_ERROR_MSG(EErrorCode::ComponentFactoryNotRegistered, "There's no factory registered with id %u", nameId);
		return -1;
	}

	void CComponentFactoryManager::Update(float dt)
	{
		for (SFactoryData& data : m_factories)
		{
			data.m_address->Update(dt);
		}
	}

	void CComponentFactoryManager::ScheduleDestroyComponent(CComponent* component)
	{
		for (std::size_t i = 0; i < m_factories.size(); ++i)
		{
			if (m_factories[i].m_address->GetComponentPosition(component) != -1)
			{
				m_factories[i].m_address->ScheduleDestroyComponent(component);
			}
		}
		DECS_ERROR_MSG(EErrorCode::ComponentNotRegisteredInFactory, "Trying to destroy component created outside a factory");
	}

	void CComponentFactoryManager::ExecuteScheduledDestroys()
	{
		for (SFactoryData& data : m_factories)
		{
			data.m_address->ExecuteScheduledDestroys();
		}
	}
}
