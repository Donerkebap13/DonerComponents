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

#include <donerecs/handle/CHandle.h>
#include <donerecs/entity/CEntity.h>
#include <donerecs/component/CComponent.h>
#include <donerecs/component/CComponentFactoryManager.h>

namespace DonerECS
{
    
    CHandle::CHandle()
        : m_elementType(EElementType::None)
        , m_componentIdx(0)
        , m_elementPosition(0)
        , m_version(0)
        {}
    
	CHandle::CHandle(CEntity* rhs)
	{
		*this = rhs ? *rhs : CHandle();
	}

	CHandle::CHandle(CComponent* rhs)
	{
		*this = rhs ? *rhs : CHandle();
	}


	const CHandle& CHandle::operator=(CEntity* rhs)
	{
		*this = rhs ? *rhs : CHandle();
		return *this;
	}

	const CHandle& CHandle::operator=(CComponent* rhs)
	{
		*this = rhs ? *rhs : CHandle();
		return *this;
	}

	CHandle::operator CEntity*()
	{
		return *this ? CEntityManager::Get()->GetElementByIdxAndVersion(m_elementPosition, m_version) : nullptr;
	}

	CHandle::operator bool()
	{
		if (m_elementType == CHandle::EElementType::Entity)
		{
			return CEntityManager::Get()->GetElementByIdxAndVersion(m_elementPosition, m_version) != nullptr;
		}
		else if (m_elementType == CHandle::EElementType::Component)
		{
			return CComponentFactoryManager::Get()->GetComponent(m_componentIdx, m_elementPosition, m_version) != nullptr;
		}
		return false;
	}

	bool CHandle::operator==(const CHandle& rhs) const
	{
		return m_elementType == rhs.m_elementType &&
			m_componentIdx == rhs.m_componentIdx &&
			m_elementPosition == rhs.m_elementPosition &&
			m_version == rhs.m_version;
	}

	bool CHandle::Destroy()
	{
		bool success = false;
		if (*this)
		{
			if (m_elementType == CHandle::EElementType::Entity)
			{
				CEntity* entity = *this;
				success = CEntityManager::Get()->DestroyEntity(&entity);
			}
			else if (m_elementType == CHandle::EElementType::Component)
			{
				CComponent* component = *this;
				success = CComponentFactoryManager::Get()->DestroyComponent(&component);
			}
		}
		if (success)
		{
			*this = CHandle();
		}
		return success;
	}
}
