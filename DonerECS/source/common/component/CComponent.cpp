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
#include <donerecs/entity/CEntity.h>

namespace DonerECS
{
	CComponent::CComponent()
		: m_numDeactivations(1)
		, m_initialized(false)
		, m_destroyed(false)
		, m_initiallyActive(true)
	{}

	CComponent::~CComponent()
	{
		for (auto& pair : m_messages)
		{
			delete pair.second;
		}
		m_messages.clear();
	}

	CComponent::operator CHandle()
	{
		return CComponentFactoryManager::Get()->SetHandleInfoFromComponent(this);
	}

	const CComponent* CComponent::operator=(const CHandle& rhs)
	{
		if (rhs.m_elementType == CHandle::EElementType::Component)
		{
			*this = CComponentFactoryManager::Get()->GetComponent(rhs.m_componentIdx, rhs.m_elementPosition, rhs.m_version);
			return this;
		}
		return nullptr;
	}
    
    const CComponent& CComponent::operator=(const CComponent& rhs)
    {
        m_owner = CHandle();
        m_numDeactivations = 1;
        m_initialized = false;
        m_destroyed = false;
        m_initiallyActive = true;
        return *this;
    }
    
    void CComponent::SetOwner(CHandle parent) { m_owner = parent; }
    CHandle CComponent::GetOwner() const { return m_owner; }

	void CComponent::Init()
	{
		if (!m_initialized)
		{
			RegisterMessages();
			DoInit();
			m_initialized = true;
		}
	}

	void CComponent::Update(float dt)
	{
		if (IsActive())
		{
			DoUpdate(dt);
		}
	}

	void CComponent::Destroy()
	{
		if (m_initialized && !m_destroyed)
		{
			DoDestroy();
			m_destroyed = true;
		}
	}

	void CComponent::Activate()
	{
		if (m_initialized)
		{
			CEntity* owner = m_owner;
			if (m_numDeactivations > 0)
			{
				if (!owner || owner->IsActive())
				{
					m_numDeactivations = 0;
					DoActivate();
				}
				else
				{
					m_numDeactivations = 1;
				}
			}
		}
	}

	void CComponent::Deactivate()
	{
		if (m_initialized)
		{
			if (m_numDeactivations == 0)
			{
				DoDeactivate();
				++m_numDeactivations;
			}
		}
	}

	void CComponent::ActivateFromParent()
	{
		if (m_numDeactivations > 0)
		{
			--m_numDeactivations;
			if (m_numDeactivations == 0)
			{
				DoActivate();
			}
		}
	}

	void CComponent::CheckFirstActivation()
	{
		if (m_initialized)
		{
			if (GetIsInitiallyActive())
			{
				CheckFirstActivationInternal();
			}
			else
			{
				++m_numDeactivations;
			}
		}
	}

	void CComponent::CheckFirstActivationInternal()
	{
		CEntity* owner = m_owner;
		if ((!owner || owner->IsActive()) && m_numDeactivations > 0)
		{
			--m_numDeactivations;
		}
	}
    
    void CComponent::DoInit() {}
    
    void CComponent::DoUpdate(float dt) {}
    
    void CComponent::DoDestroy() {}
    
    void CComponent::DoActivate() {}
    
    void CComponent::DoDeactivate() {}
}
