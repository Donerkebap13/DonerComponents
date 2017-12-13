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

#include <donerecs/ErrorMessages.h>
#include <donerecs/entity/CEntityManager.h>
#include <donerecs/handle/CHandle.h>

namespace DonerECS
{
	CEntityManager::CEntityManager()
		: CFactory(MAX_ENTITIES)
	{}


	CEntity* CEntityManager::CreateEntity()
	{
		CEntity* entity = GetNewElement();
		if (!entity)
		{
			DECS_ERROR_MSG(EErrorCode::NoMoreEntitiesAvailable, "No more entities available for creation at this point");
		}
		return entity;
	}

	bool CEntityManager::DestroyEntity(CEntity** entity)
	{
		if (*entity)
		{
			if (FindElement(*entity))
			{
				(*entity)->Destroy();
				return DestroyElement(entity);
			}
			DECS_WARNING_MSG(EErrorCode::EntityNotRegisteredInFactory, "Trying to destroy an entity which hasn't been created using CEntityManager");
		}
		return false;
	}

	bool CEntityManager::DestroyEntity(CHandle handle)
	{
		CEntity* entity = handle;
		if (entity)
		{
			entity->SetParent(nullptr);
			entity->Destroy();
			return DestroyElement(&entity);
		}
		return false;
	}

	void CEntityManager::SendPostMsgs()
	{
		for (CPostMessageBase* postMsg : m_postMsgs)
		{
			postMsg->SendMessage();
			delete postMsg;
		}
		m_postMsgs.clear();
	}
}
