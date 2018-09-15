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

#include <donercomponents/ErrorMessages.h>
#include <donercomponents/CDonerComponentsSystems.h>
#include <donercomponents/gameObject/CGameObject.h>
#include <donercomponents/gameObject/CPrefabManager.h>
#include <donercomponents/handle/CHandle.h>

namespace DonerComponents
{
	CPrefabManager::CPrefabManager()
		: m_gameObjectManager(*CDonerComponentsSystems::Get()->GetGameObjectManager())
	{}

	CPrefabManager::~CPrefabManager()
	{
		for (auto& pair : m_prefabs)
		{
			pair.second->Destroy();
		}
		m_prefabs.clear();
	}

	bool CPrefabManager::RegisterPrefab(CStrID nameId, CGameObject* prefab)
	{
		if (m_prefabs.find(nameId) == m_prefabs.end())
		{
			m_prefabs[nameId] = prefab;
			return true;
		}
		DC_ERROR_MSG(EErrorCode::PrefabAlreadyRegistered, "Prefab with nameId %u already registered", nameId);
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
			CGameObject* prefab = (*prefabIt).second;
			CGameObject* gameObject = m_gameObjectManager.CreateGameObject();
			if (gameObject)
			{
				gameObject->CloneFrom(prefab);

				if (cloneMode == ECloneMode::ActivateAutomaticallyAfterCreation)
				{
					gameObject->Init();
					gameObject->CheckFirstActivation();
					gameObject->Activate();
				}
			}
			return gameObject;
		}
		DC_ERROR_MSG(EErrorCode::PrefabNotRegistered, "Prefab with nameId %u not registered", nameId);
		return CHandle();
	}
}
