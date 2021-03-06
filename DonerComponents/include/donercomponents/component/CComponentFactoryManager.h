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

#include <donercomponents/ErrorMessages.h>
#include <donercomponents/component/CComponentFactory.h>
#include <donercomponents/utils/hash/CTypeHasher.h>
#include <donercomponents/utils/hash/CStrID.h>

#include <vector>

#define ADD_COMPONENT_FACTORY(name, T, N) DonerComponents::CDonerComponentsSystems::Get()->GetComponentFactoryManager()->AddFactory(name, new DonerComponents::CComponentFactory<T>(N))

namespace DonerComponents
{
	class CComponent;

	class CComponentFactoryManager
	{
		friend class CDonerComponentsSystems;

		struct SFactoryData
		{
			CTypeHasher::HashId m_id;
			CStrID m_nameId;
			IComponentFactory* m_address;
			SFactoryData() : m_id(0), m_address(nullptr) {}
			SFactoryData(CTypeHasher::HashId id, const char* const nameId, IComponentFactory* address) : m_id(id), m_nameId(nameId), m_address(address) {}
		};
	public:
		template<typename T>
		bool AddFactory(const char* const factoryName, CComponentFactory<T>* factory)
		{
            static_assert(std::is_base_of<CComponent, T>::value, "T must inherits from CComponent");
            
			if (!FactoryExists<T>())
			{
				m_factories.emplace_back(CTypeHasher::Hash<T>(), factoryName, factory);
                return true;
			}
			else
			{
				DC_ERROR_MSG(EErrorCode::ComponentFactoryAlreadyRegistered, "You're trying to register an already registered component factory with name %s", factoryName);
				delete factory;
                return false;
			}
		}

		template<typename T>
		CComponent* CreateComponent()
		{
			IComponentFactory* factory = GetFactory<T>();
			if (factory)
			{
				return factory->CreateComponent();
			}
			else
			{
				DC_ERROR_MSG(EErrorCode::ComponentFactoryNotRegistered, "There's no factory registered to create this component");
				return nullptr;
			}
		}

		template<typename T>
		CComponent* AddComponent(std::vector<CComponent*>& components)
		{
			int factoryIdx = GetFactoryindex<T>();
			if (factoryIdx >= 0)
			{
				IComponentFactory* factory = GetFactory<T>();
				if (factory)
				{
					components[factoryIdx] = factory->CreateComponent();
					return components[factoryIdx];
				}
			}
			DC_ERROR_MSG(EErrorCode::ComponentFactoryNotRegistered, "There's no factory registered to create this component");
			return nullptr;
		}

		template<typename T>
		CComponent* GetByIdxAndVersion(int index, int version)
		{
			IComponentFactory* factory = GetFactory<T>();
			if (factory)
			{
				return factory->GetByIdxAndVersion(index, version);
			}
			else
			{
				DC_WARNING_MSG(EErrorCode::ComponentFactoryNotRegistered, "There's no factory registered to create this component");
				return nullptr;
			}
		}

		template<typename T>
		int GetFactoryindex()
		{
			CTypeHasher::HashId id = CTypeHasher::Hash<T>();
			for (std::size_t i = 0; i < m_factories.size(); ++i)
			{
				if (m_factories[i].m_id == id)
				{
					return i;
				}
			}
			DC_ERROR_MSG(EErrorCode::ComponentFactoryNotRegistered, "There's no factory registered to create this component");
			return -1;
		}

		CComponent* CreateComponent(CStrID componentNameId);
		CComponent* CloneComponent(CComponent* component, int componentIdx);
		void CloneComponents(std::vector<CComponent*>& src, std::vector<CComponent*>& dst);
		CComponent* AddComponent(CStrID componentNameId, std::vector<CComponent*>& components);
		CComponent* GetComponent(std::size_t componentTypeIdx, int index, int version);
		CHandle SetHandleInfoFromComponent(CComponent* component);
		int GetPositionForElement(CComponent* component);
		bool DestroyComponent(CComponent** component);

		int GetFactoryIndexByName(CStrID nameId);

		int GetRegisteredComponentsAmount() const { return m_factories.size(); }

		void Update(float dt);

		void ScheduleDestroyComponent(CComponent* component);
		void ExecuteScheduledDestroys();

	private:
		CComponentFactoryManager() = default;

		template<typename T>
		bool FactoryExists()
		{
			CTypeHasher::HashId id = CTypeHasher::Hash<T>();
			for (SFactoryData& data : m_factories)
			{
				if (data.m_id == id)
				{
					return true;
				}
			}
			return false;
		}

		template<typename T>
		IComponentFactory* GetFactory()
		{
			CTypeHasher::HashId id = CTypeHasher::Hash<T>();
			for (SFactoryData& data : m_factories)
			{
				if (data.m_id == id)
				{
					return data.m_address;
				}
			}
			DC_WARNING_MSG(EErrorCode::ComponentFactoryNotRegistered, "There's no factory registered to create this component");
			return nullptr;
		}

		IComponentFactory* GetFactoryByName(CStrID nameId);
		IComponentFactory* GetFactoryByIndex(std::size_t idx);

		std::vector<SFactoryData> m_factories;
	};
}
