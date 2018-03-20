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
#include <donerecs/component/CComponentFactory.h>
#include <donerecs/handle/CHandle.h>

#include <algorithm>

namespace DonerECS
{
	bool IComponentFactory::SetHandleInfoFromComponent(CComponent* component, CHandle& handle)
	{
		int pos = GetComponentPosition(component);
		if (pos != -1)
		{
			handle.m_elementType = CHandle::EElementType::Component;
			handle.m_elementPosition = pos;
			handle.m_version = component->GetVersion();
			return true;
		}
		return false;
	}

	void IComponentFactory::ScheduleDestroyComponent(CHandle component)
	{
		if (std::find(m_scheduledDestroys.begin(), m_scheduledDestroys.end(), component) == m_scheduledDestroys.end())
		{
			m_scheduledDestroys.emplace_back(component);
		}
	}

	void IComponentFactory::ExecuteScheduledDestroys()
	{
		for (CComponent* component : m_scheduledDestroys)
		{
			if (component)
			{
				DestroyComponent(component);
			}
		}
		m_scheduledDestroys.clear();
	}
}
