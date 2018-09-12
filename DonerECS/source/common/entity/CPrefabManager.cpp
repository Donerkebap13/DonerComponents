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
#include <donerecs/CDonerECSSystems.h>
#include <donerecs/entity/CEntity.h>
#include <donerecs/entity/CPrefabManager.h>
#include <donerecs/handle/CHandle.h>

namespace DonerECS
{
	CPrefabManager::CPrefabManager()
		: m_entityManager(*CDonerECSSystems::Get()->GetEntityManager())
	{}

	CPrefabManager::~CPrefabManager()
	{
		for (auto& pair : m_prefabs)
		{
			pair.second->Destroy();
		}
		m_prefabs.clear();
	}

	bool CPrefabManager::RegisterPrefab(CStrID nameId, CEntity* prefab)
	{
		if (m_prefabs.find(nameId) == m_prefabs.end())
		{
			m_prefabs[nameId] = prefab;
			return true;
		}
		DECS_ERROR_MSG(EErrorCode::PrefabAlreadyRegistered, "Prefab with nameId %u already registered", nameId);
		return false;
	}

	CHandle CPrefabManager::ClonePrefab(CStrID nameId)
	{
		return ClonePrefab(nameId, ECloneMode::ActivateAutomaticallyAfterCreation);
	}

	CHandle CPrefabManager::ClonePrefab(CStrID nameId, ECloneMode cloneMode)
	{
		auto prefabIt = m_prefabs.find(nameId);
		if (prefabIt != m_prefabs.end())
		{
			CEntity* prefab = (*prefabIt).second;
			CEntity* entity = m_entityManager.CreateEntity();
			if (entity)
			{
				entity->CloneFrom(prefab);

				if (cloneMode == ECloneMode::ActivateAutomaticallyAfterCreation)
				{
					entity->Init();
					entity->CheckFirstActivation();
					entity->Activate();
				}
			}
			return entity;
		}
		DECS_ERROR_MSG(EErrorCode::PrefabNotRegistered, "Prefab with nameId %u not registered", nameId);
		return CHandle();
	}
}
