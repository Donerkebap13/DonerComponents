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

#include <donerecs/entity/CEntity.h>
#include <donerecs/entity/CPrefabManager.h>
#include <donerecs/component/CComponent.h>
#include <donerecs/component/CComponentFactoryManager.h>
#include <donerecs/handle/CHandle.h>

#include <gtest/gtest.h>

// gmock
// using ::testing::Return;

namespace DonerECS
{
	namespace PrefabManagerTestInternal
	{
		class CCompFoo : public DonerECS::CComponent
		{
		public:
			CCompFoo() : m_a(-1), m_b(-1) {}

			int m_a;
			int m_b;
		};
	}

	class CPrefabManagerTest : public ::testing::Test
	{
	public:
		CPrefabManagerTest()
			: m_componentFactoryManager(CComponentFactoryManager::CreateInstance())
			, m_entityManager(CEntityManager::CreateInstance())
			, m_prefabManager(CPrefabManager::CreateInstance())
		{
			ADD_COMPONENT_FACTORY("foo", PrefabManagerTestInternal::CCompFoo, 10);
		}

		~CPrefabManagerTest()
		{
			CPrefabManager::DestroyInstance();
			CEntityManager::DestroyInstance();
			CComponentFactoryManager::DestroyInstance();
		}

		CComponentFactoryManager *m_componentFactoryManager;
		CEntityManager* m_entityManager;
		CPrefabManager* m_prefabManager;
	};

	TEST_F(CPrefabManagerTest, register_prefab)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		bool success = m_prefabManager->RegisterPrefab("test", entity);
		EXPECT_TRUE(success);
	}

	TEST_F(CPrefabManagerTest, register_existing_prefab_fails)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		m_prefabManager->RegisterPrefab("test", entity);

		bool success = m_prefabManager->RegisterPrefab("test", entity);
		EXPECT_FALSE(success);

		// gmock (TEST THIS)
		/*EXPECT_CALL(m_prefabManager, RegisterPrefab("test", entity))
		.WillOnce(Return(true))
		.WillRepeatedly(Return(false));*/
	}

	TEST_F(CPrefabManagerTest, clone_prefab)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		entity->SetName("test1");

		m_prefabManager->RegisterPrefab("test", entity);

		CEntity* cloned = m_prefabManager->ClonePrefab("test");
		EXPECT_EQ(cloned->GetName(), entity->GetName());
	}

	TEST_F(CPrefabManagerTest, clonen_prefab_is_initialized)
	{
		CEntity* entity = m_entityManager->CreateEntity();

		m_prefabManager->RegisterPrefab("test", entity);

		CEntity* cloned = m_prefabManager->ClonePrefab("test");
		EXPECT_TRUE(cloned->IsInitialized());
	}

	TEST_F(CPrefabManagerTest, clonen_prefab_is_not_active)
	{
		CEntity* entity = m_entityManager->CreateEntity();

		m_prefabManager->RegisterPrefab("test", entity);

		CEntity* cloned = m_prefabManager->ClonePrefab("test");
		EXPECT_FALSE(cloned->IsActive());
	}

	TEST_F(CPrefabManagerTest, cloned_prefab_components_have_correct_owner)
	{
		CEntity* prefabEntity = m_entityManager->CreateEntity();
		CComponent* prefabComponent = prefabEntity->AddComponent("foo");
		CHandle prefabEntityHandle = prefabEntity;
		EXPECT_EQ(prefabComponent->GetOwner(), prefabEntityHandle);

		m_prefabManager->RegisterPrefab("test", prefabEntity);

		CEntity* cloned = m_prefabManager->ClonePrefab("test");

		CComponent* clonedComponent = cloned->GetComponent("foo");
		CHandle clonedHandle = cloned;
		EXPECT_EQ(clonedComponent->GetOwner(), clonedHandle);
	}

	TEST_F(CPrefabManagerTest, clone_invalid_prefab_return_invalid_entity)
	{
		CEntity* cloned = m_prefabManager->ClonePrefab("test");
		EXPECT_EQ(nullptr, cloned);
	}
}
