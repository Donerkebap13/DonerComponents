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

#include <donerecs/common/CFactory.h>
#include <donerecs/common/CSingleton.h>
#include <donerecs/entity/CEntity.h>
#include <donerecs/messages/CPostMsg.h>

#include <vector>

namespace DonerECS
{
	class CEntityManager : public CSingleton<CEntityManager>, public CFactory<CEntity>
	{
		friend class CEntity;
	public:
		CEntityManager();
		~CEntityManager() override {}

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
		void PostMessage(CHandle entity, const T& message)
		{
			m_postMsgs.emplace_back(new CPostMessage<T>(entity, message));
		}

		CEntity* CreateEntity();

		bool DestroyEntity(CEntity** entity);
		bool DestroyEntity(CHandle handle);

		void SendPostMsgs();

	private:
		std::vector<CPostMessageBase*> m_postMsgs;
	};
}
