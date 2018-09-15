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

#include <donercomponents/CDonerComponentsSystems.h>

#include <donercomponents/component/CComponentFactoryManager.h>
#include <donercomponents/gameObject/CGameObject.h>
#include <donercomponents/gameObject/CPrefabManager.h>
#include <donercomponents/tags/CTagsManager.h>

#include <cassert>

namespace DonerComponents
{
	CDonerComponentsSystems::CDonerComponentsSystems()
		: m_componentFactoryManager(nullptr)
		, m_gameObjectManager(nullptr)
		, m_tagsManager(nullptr)
		, m_prefabManager(nullptr)
		, m_initialized(false)
	{
	}

	CDonerComponentsSystems::~CDonerComponentsSystems()
	{
		Destroy();
	}

	CDonerComponentsSystems& CDonerComponentsSystems::Init()
	{
		assert(!m_initialized);
		m_initialized = true;

		m_componentFactoryManager = new CComponentFactoryManager();
		m_gameObjectManager = new CGameObjectManager();
		m_tagsManager = new CTagsManager();
		m_prefabManager = new CPrefabManager();

		return *this;
	}

	void CDonerComponentsSystems::Destroy()
	{
		assert(m_initialized);

		DC_DELETE_POINTER(m_prefabManager);
		DC_DELETE_POINTER(m_tagsManager);

		m_componentFactoryManager->ExecuteScheduledDestroys();
		m_gameObjectManager->ExecuteScheduledDestroys();

		DC_DELETE_POINTER(m_gameObjectManager);
		DC_DELETE_POINTER(m_componentFactoryManager);

		m_initialized = false;
	}

	void CDonerComponentsSystems::Update(float dt)
	{
		// Updates all registered components
		m_componentFactoryManager->Update(dt);

		// Destroys pending entities & components
		m_componentFactoryManager->ExecuteScheduledDestroys();
		m_gameObjectManager->ExecuteScheduledDestroys();

		// Sends postMsg
		m_gameObjectManager->SendPostMsgs();
	}

	CComponentFactoryManager* CDonerComponentsSystems::GetComponentFactoryManager() 
	{
		assert(m_initialized);
		return m_componentFactoryManager; 
	}
	CGameObjectManager* CDonerComponentsSystems::GetGameObjectManager() 
	{
		assert(m_initialized);
		return m_gameObjectManager;
	}

	CTagsManager* CDonerComponentsSystems::GetTagsManager() 
	{
		assert(m_initialized);
		return m_tagsManager;
	}

	CPrefabManager* CDonerComponentsSystems::GetPrefabManager() 
	{
		assert(m_initialized);
		return m_prefabManager;
	}
}
