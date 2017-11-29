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

#include <donerecs/component/CComponent.h>
#include <donerecs/common/CFactory.h>

namespace DonerECS
{
	class IComponentFactory
	{
	public:
		virtual CComponent* CreateComponent() = 0;
		virtual CComponent* CloneComponent(CComponent* component) = 0;
		virtual CComponent* GetByIdxAndVersion(int index, int version) = 0;
		virtual bool SetHandleInfoFromComponent(CComponent* component, CHandle& handle) = 0;
		virtual int GetComponentPosition(CComponent* component) = 0;
		virtual bool DestroyComponent(CComponent* component) = 0;
		virtual void Update(float dt) = 0;
	};

	template <typename T>
	class CComponentFactory : public IComponentFactory, public CFactory<T>
	{
		static_assert(
			std::is_base_of<CComponent, T>::value,
			"T must inherits from CComponent"
			);
	public:
		CComponentFactory(int nElements)
			: CFactory(nElements)
		{}

		CComponent* CreateComponent() override
		{
			return GetNewElement();
		}

		CComponent* CloneComponent(CComponent* component) override
		{
			T* newComponent = static_cast<T*>(CreateComponent());
			if (newComponent)
			{
				*newComponent = *(static_cast<T*>(component));
				return newComponent;
			}
			return nullptr;
		}

		CComponent* GetByIdxAndVersion(int index, int version) override
		{
			return GetElementByIdxAndVersion(index, version);
		}

		bool SetHandleInfoFromComponent(CComponent* component, CHandle& handle) override
		{
			int pos = GetPositionForElement(static_cast<T*>(component));
			if (pos != -1)
			{
				handle.m_elementType = CHandle::EElementType::Component;
				handle.m_elementPosition = pos;
				handle.m_version = component->GetVersion();
				return true;
			}
			return false;
		}

		int GetComponentPosition(CComponent* component) override
		{
			return GetPositionForElement(static_cast<T*>(component));
		}

		bool DestroyComponent(CComponent* component) override
		{
			T* tmp = static_cast<T*>(component);
			return DestroyElement(&tmp);
		}

		void Update(float dt) override
		{
			for (SEntry& entry : m_entries)
			{
				if (entry.m_used)
				{
					entry.m_data->Update(dt);
				}
			}
		}
	};
}