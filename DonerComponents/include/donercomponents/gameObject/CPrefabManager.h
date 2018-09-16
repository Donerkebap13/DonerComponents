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

#include <donercomponents/common/CSingleton.h>
#include <donercomponents/utils/hash/CStrID.h>
#include <donercomponents/utils/hash/hash_cstrid.h>
#include <donercomponents/handle/CHandle.h>

#include <unordered_map>

namespace DonerComponents
{
	class CGameObject;
	class CGameObjectManager;

	class CPrefabManager
	{
		friend class CDonerComponentsSystems;
		friend class CGameObject;
		friend class CGameObjectParser;
	public:
		enum class ECloneMode { ActivateAutomaticallyAfterCreation, KeepUninitialized };

		~CPrefabManager();

		CHandle ClonePrefab(CStrID nameId);
		CHandle ClonePrefab(CStrID nameId, ECloneMode cloneMode);
	private:
		CPrefabManager();

		bool RegisterPrefab(CStrID nameId, CGameObject* prefab);

		std::unordered_map<CStrID, CGameObject*, std::hash<CStrID>> m_prefabs;

		CGameObjectManager& m_gameObjectManager;
	};
}
