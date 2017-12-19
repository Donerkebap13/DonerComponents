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
#include <donerecs/component/CComponent.h>
#include <donerecs/component/CComponentFactoryManager.h>
#include <donerecs/handle/CHandle.h>

#include <gtest/gtest.h>

namespace DonerECS
{
	namespace EntityComponentTestInternal
	{
		const int LOOP_COUNT = 5;

		class CCompFoo : public CComponent
		{
		public:
			CCompFoo() : m_x(0) {}

			const CCompFoo& operator=(const CCompFoo& rhs)
			{
				m_x = rhs.m_x;
				return *this;
			}
			int m_x;
		};

		class CCompBar : public CComponent
		{};
	}

	class CEntityComponentTest : public ::testing::Test
	{
	public:
		CEntityComponentTest()
			: m_componentFactoryManager(CComponentFactoryManager::CreateInstance())
			, m_entityManager(CEntityManager::CreateInstance())
		{
			ADD_COMPONENT_FACTORY("foo", EntityComponentTestInternal::CCompFoo, 10);
			ADD_COMPONENT_FACTORY("bar", EntityComponentTestInternal::CCompBar, 1);
		}

		~CEntityComponentTest()
		{
			CEntityManager::DestroyInstance();
			CComponentFactoryManager::DestroyInstance();
		}

		std::tuple<CEntity*, CEntity*, CComponent*, CComponent*> GetEntityWithChildren()
		{
			CEntity* entity = m_entityManager->CreateEntity();
			EXPECT_NE(nullptr, entity);
			CComponent* component = entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
			EXPECT_NE(nullptr, component);
			CEntity* entity1 = m_entityManager->CreateEntity();
			EXPECT_NE(nullptr, entity1);
			CComponent* component1 = entity1->AddComponent<EntityComponentTestInternal::CCompFoo>();
			EXPECT_NE(nullptr, component1);

			entity->AddChild(entity1);

			return std::make_tuple(entity, entity1, component, component1);
		}

		std::tuple<CEntity*, CEntity*, CEntity*, CComponent*, CComponent*, CComponent*> GetEntityWithChildrenRecursive()
		{
			CEntity* entity = m_entityManager->CreateEntity();
			EXPECT_NE(nullptr, entity);
			CComponent* component = entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
			EXPECT_NE(nullptr, component);
			CEntity* entity1 = m_entityManager->CreateEntity();
			EXPECT_NE(nullptr, entity1);
			CComponent* component1 = entity1->AddComponent<EntityComponentTestInternal::CCompFoo>();
			EXPECT_NE(nullptr, component1);
			CEntity* entity11 = m_entityManager->CreateEntity();
			EXPECT_NE(nullptr, entity11);
			CComponent* component11 = entity11->AddComponent<EntityComponentTestInternal::CCompFoo>();
			EXPECT_NE(nullptr, component11);

			entity->AddChild(entity1);
			entity1->AddChild(entity11);

			return std::make_tuple(entity, entity1, entity11, component, component1, component11);
		}

		CComponentFactoryManager *m_componentFactoryManager;
		CEntityManager *m_entityManager;
	};

	TEST_F(CEntityComponentTest, add_component_by_class)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CComponent* component = entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);
		EntityComponentTestInternal::CCompFoo* foo = dynamic_cast<EntityComponentTestInternal::CCompFoo*>(component);
		EXPECT_NE(nullptr, foo);
		EntityComponentTestInternal::CCompBar* bar = dynamic_cast<EntityComponentTestInternal::CCompBar*>(component);
		EXPECT_EQ(nullptr, bar);

		CHandle handle = entity->AddComponent("bar");
		EXPECT_TRUE(static_cast<bool>(handle));
		component = handle;
		bar = dynamic_cast<EntityComponentTestInternal::CCompBar*>(component);
		EXPECT_NE(nullptr, bar);
		foo = dynamic_cast<EntityComponentTestInternal::CCompFoo*>(component);
		EXPECT_EQ(nullptr, foo);
	}

	TEST_F(CEntityComponentTest, add_component_by_name)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CComponent* component = entity->AddComponent("foo");
		EXPECT_NE(nullptr, component);
		EntityComponentTestInternal::CCompFoo* foo = dynamic_cast<EntityComponentTestInternal::CCompFoo*>(component);
		EXPECT_NE(nullptr, foo);
		EntityComponentTestInternal::CCompBar* bar = dynamic_cast<EntityComponentTestInternal::CCompBar*>(component);
		EXPECT_EQ(nullptr, bar);

		CHandle handle = entity->AddComponent("bar");
		EXPECT_TRUE(static_cast<bool>(handle));
		component = handle;
		bar = dynamic_cast<EntityComponentTestInternal::CCompBar*>(component);
		EXPECT_NE(nullptr, bar);
		foo = dynamic_cast<EntityComponentTestInternal::CCompFoo*>(component);
		EXPECT_EQ(nullptr, foo);
	}

	TEST_F(CEntityComponentTest, get_component_by_class)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		CComponent* component = entity->GetComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);
		EntityComponentTestInternal::CCompFoo* foo = dynamic_cast<EntityComponentTestInternal::CCompFoo*>(component);
		EXPECT_NE(nullptr, foo);
		EntityComponentTestInternal::CCompBar* bar = dynamic_cast<EntityComponentTestInternal::CCompBar*>(component);
		EXPECT_EQ(nullptr, bar);
	}

	TEST_F(CEntityComponentTest, has_component_by_class)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		bool hasComponent = entity->HasComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_TRUE(hasComponent);
	}

	TEST_F(CEntityComponentTest, get_component_by_name)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		CComponent* component = entity->GetComponent("foo");
		EXPECT_NE(nullptr, component);
		EntityComponentTestInternal::CCompFoo* foo = dynamic_cast<EntityComponentTestInternal::CCompFoo*>(component);
		EXPECT_NE(nullptr, foo);
		EntityComponentTestInternal::CCompBar* bar = dynamic_cast<EntityComponentTestInternal::CCompBar*>(component);
		EXPECT_EQ(nullptr, bar);
	}

	TEST_F(CEntityComponentTest, has_component_by_name)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		bool hasComponent = entity->HasComponent("foo");
		EXPECT_TRUE(hasComponent);
	}

	TEST_F(CEntityComponentTest, remove_component_by_class)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		CHandle handle = entity->GetComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_TRUE(static_cast<bool>(handle));

		bool success = entity->RemoveComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_TRUE(success);

		CComponent* component = entity->GetComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_EQ(nullptr, component);
		EXPECT_FALSE(static_cast<bool>(handle));
	}

	TEST_F(CEntityComponentTest, remove_component_by_name)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		CHandle handle = entity->GetComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_TRUE(static_cast<bool>(handle));

		bool success = entity->RemoveComponent("foo");
		EXPECT_TRUE(success);

		CComponent* component = entity->GetComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_EQ(nullptr, component);
		EXPECT_FALSE(static_cast<bool>(handle));
	}

	TEST_F(CEntityComponentTest, check_cant_add_component_twice)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CHandle handle = entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_TRUE(static_cast<bool>(handle));
		handle = entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_FALSE(static_cast<bool>(handle));
	}

	TEST_F(CEntityComponentTest, check_cant_remove_component_twice)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CHandle handle = entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_TRUE(static_cast<bool>(handle));

		bool success = entity->RemoveComponent("foo");
		EXPECT_TRUE(success);
		success = entity->RemoveComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_FALSE(success);
	}

	TEST_F(CEntityComponentTest, check_component_invalid_after_deleting_entity)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CHandle handle = entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_TRUE(static_cast<bool>(handle));

		m_entityManager->DestroyEntity(&entity);
		EXPECT_EQ(nullptr, entity);

		EXPECT_FALSE(static_cast<bool>(handle));
		CComponent* component = handle;
		EXPECT_EQ(nullptr, component);
	}

	TEST_F(CEntityComponentTest, check_component_parent)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CComponent* component = entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);

		CHandle entityHandle = entity;
		EXPECT_TRUE(static_cast<bool>(entityHandle));
		CHandle ownerHandle = component->GetOwner();

		EXPECT_EQ(ownerHandle, entityHandle);
	}

	TEST_F(CEntityComponentTest, init_entity_initialize_components)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CComponent* component = entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);

		EXPECT_FALSE(entity->IsInitialized());
		EXPECT_FALSE(component->IsInitialized());
		entity->Init();
		EXPECT_TRUE(entity->IsInitialized());
		EXPECT_TRUE(component->IsInitialized());
	}

	TEST_F(CEntityComponentTest, destroy_entity_destroys_components)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CHandle compHandle = entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_TRUE(static_cast<bool>(compHandle));

		EXPECT_FALSE(entity->IsDestroyed());
		entity->Destroy();
		EXPECT_TRUE(entity->IsDestroyed());
		EXPECT_FALSE(static_cast<bool>(compHandle));
	}

	TEST_F(CEntityComponentTest, activate_entity_activates_components)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CComponent* component = entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);
		entity->Init();
		EXPECT_FALSE(entity->IsActive());
		EXPECT_FALSE(component->IsActive());
		entity->Activate();
		EXPECT_TRUE(entity->IsActive());
		EXPECT_TRUE(component->IsActive());
	}

	TEST_F(CEntityComponentTest, entity_activate_activates_components_children)
	{
		CEntity* entity, *entity1 = nullptr;
		CComponent* component, *component1 = nullptr;
		std::tie(entity, entity1, component, component1) = GetEntityWithChildren();

		entity->Init();
		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component1->IsActive());
		entity->Activate();
		EXPECT_TRUE(component->IsActive());
		EXPECT_TRUE(component1->IsActive());
	}

	TEST_F(CEntityComponentTest, entity_activate_activates_components_children_recursive)
	{
		CEntity* entity, *entity1, *entity11 = nullptr;
		CComponent* component, *component1, *component11 = nullptr;
		std::tie(entity, entity1, entity11, component, component1, component11) = GetEntityWithChildrenRecursive();

		entity->Init();
		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component1->IsActive());
		EXPECT_FALSE(component11->IsActive());
		entity->Activate();
		EXPECT_TRUE(component->IsActive());
		EXPECT_TRUE(component1->IsActive());
		EXPECT_TRUE(component11->IsActive());
	}

	TEST_F(CEntityComponentTest, deactivate_entity_deactivates_components)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CComponent* component = entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);

		entity->Init();

		EXPECT_FALSE(entity->IsActive());
		EXPECT_FALSE(component->IsActive());
		entity->Activate();
		EXPECT_TRUE(entity->IsActive());
		EXPECT_TRUE(component->IsActive());

		entity->Deactivate();
		EXPECT_FALSE(entity->IsActive());
		EXPECT_FALSE(component->IsActive());
	}

	TEST_F(CEntityComponentTest, deactivate_entity_deactivates_components_children)
	{
		CEntity* entity, *entity1 = nullptr;
		CComponent* component, *component1 = nullptr;
		std::tie(entity, entity1, component, component1) = GetEntityWithChildren();

		entity->Init();

		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component1->IsActive());
		entity->Activate();
		EXPECT_TRUE(component->IsActive());
		EXPECT_TRUE(component1->IsActive());

		entity->Deactivate();
		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component1->IsActive());
	}

	TEST_F(CEntityComponentTest, deactivate_entity_deactivates_components_children_recursive)
	{
		CEntity* entity, *entity1, *entity11 = nullptr;
		CComponent* component, *component1, *component11 = nullptr;
		std::tie(entity, entity1, entity11, component, component1, component11) = GetEntityWithChildrenRecursive();

		entity->Init();

		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component1->IsActive());
		EXPECT_FALSE(component11->IsActive());
		entity->Activate();
		EXPECT_TRUE(component->IsActive());
		EXPECT_TRUE(component1->IsActive());
		EXPECT_TRUE(component11->IsActive());

		entity->Deactivate();
		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component1->IsActive());
		EXPECT_FALSE(component11->IsActive());
	}

	TEST_F(CEntityComponentTest, entity_and_component_initiallyActive)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CComponent* component = entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);

		entity->Init();

		entity->SetIsInitiallyActive(true);
		component->SetIsInitiallyActive(true);
		entity->CheckFirstActivation();
		EXPECT_TRUE(entity->IsActive());
		EXPECT_TRUE(component->IsActive());
	}

	TEST_F(CEntityComponentTest, entity_and_component_initiallyActive_children)
	{
		CEntity* entity, *entity1 = nullptr;
		CComponent* component, *component1 = nullptr;
		std::tie(entity, entity1, component, component1) = GetEntityWithChildren();

		entity->Init();

		entity->SetIsInitiallyActive(true);
		entity1->SetIsInitiallyActive(true);
		component->SetIsInitiallyActive(true);
		component1->SetIsInitiallyActive(true);
		entity->CheckFirstActivation();
		EXPECT_TRUE(entity->IsActive());
		EXPECT_TRUE(entity1->IsActive());
		EXPECT_TRUE(component->IsActive());
		EXPECT_TRUE(component1->IsActive());
	}

	TEST_F(CEntityComponentTest, entity_and_component_initiallyActive_children_recursive)
	{
		CEntity* entity, *entity1, *entity11 = nullptr;
		CComponent* component, *component1, *component11 = nullptr;
		std::tie(entity, entity1, entity11, component, component1, component11) = GetEntityWithChildrenRecursive();

		entity->Init();

		entity->SetIsInitiallyActive(true);
		entity1->SetIsInitiallyActive(true);
		entity11->SetIsInitiallyActive(true);
		component->SetIsInitiallyActive(true);
		component1->SetIsInitiallyActive(true);
		component11->SetIsInitiallyActive(true);
		entity->CheckFirstActivation();
		EXPECT_TRUE(entity->IsActive());
		EXPECT_TRUE(entity1->IsActive());
		EXPECT_TRUE(entity11->IsActive());
		EXPECT_TRUE(component->IsActive());
		EXPECT_TRUE(component1->IsActive());
		EXPECT_TRUE(component11->IsActive());
	}

	TEST_F(CEntityComponentTest, entity_active_and_component_deactivated_initiallyActive)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CComponent* component = entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);

		entity->Init();

		entity->SetIsInitiallyActive(true);
		component->SetIsInitiallyActive(false);
		entity->CheckFirstActivation();
		EXPECT_TRUE(entity->IsActive());
		EXPECT_FALSE(component->IsActive());
	}

	TEST_F(CEntityComponentTest, entity_active_and_component_deactivated_initiallyActive_children)
	{
		CEntity* entity, *entity1 = nullptr;
		CComponent* component, *component1 = nullptr;
		std::tie(entity, entity1, component, component1) = GetEntityWithChildren();

		entity->Init();

		component->SetIsInitiallyActive(false);
		component1->SetIsInitiallyActive(false);
		entity->CheckFirstActivation();
		EXPECT_TRUE(entity->IsActive());
		EXPECT_TRUE(entity1->IsActive());
		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component1->IsActive());

		for (int i = 0; i < EntityComponentTestInternal::LOOP_COUNT; ++i)
		{
			entity->Activate();
			EXPECT_FALSE(component->IsActive());
			EXPECT_FALSE(component1->IsActive());
		}
		for (int i = 0; i < EntityComponentTestInternal::LOOP_COUNT; ++i)
		{
			entity1->Activate();
			EXPECT_FALSE(component->IsActive());
			EXPECT_FALSE(component1->IsActive());
		}
	}

	TEST_F(CEntityComponentTest, entity_active_and_component_deactivated_initiallyActive_children_recursive)
	{
		CEntity* entity, *entity1, *entity11 = nullptr;
		CComponent* component, *component1, *component11 = nullptr;
		std::tie(entity, entity1, entity11, component, component1, component11) = GetEntityWithChildrenRecursive();

		entity->Init();

		component->SetIsInitiallyActive(false);
		component1->SetIsInitiallyActive(false);
		component11->SetIsInitiallyActive(false);
		entity->CheckFirstActivation();
		EXPECT_TRUE(entity->IsActive());
		EXPECT_TRUE(entity1->IsActive());
		EXPECT_TRUE(entity11->IsActive());
		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component1->IsActive());
		EXPECT_FALSE(component11->IsActive());

		for (int i = 0; i < EntityComponentTestInternal::LOOP_COUNT; ++i)
		{
			entity->Activate();
			EXPECT_FALSE(component->IsActive());
			EXPECT_FALSE(component1->IsActive());
			EXPECT_FALSE(component11->IsActive());
		}
		for (int i = 0; i < EntityComponentTestInternal::LOOP_COUNT; ++i)
		{
			entity1->Activate();
			EXPECT_FALSE(component->IsActive());
			EXPECT_FALSE(component1->IsActive());
			EXPECT_FALSE(component11->IsActive());
		}
	}

	TEST_F(CEntityComponentTest, component_no_active_despite_initiallyActive_if_entity_not_initiallyActive)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CComponent* component = entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);

		entity->Init();

		entity->SetIsInitiallyActive(false);
		component->SetIsInitiallyActive(true);
		entity->CheckFirstActivation();
		EXPECT_FALSE(entity->IsActive());
		EXPECT_FALSE(component->IsActive());
	}

	TEST_F(CEntityComponentTest, component_no_active_despite_initiallyActive_if_entity_not_initiallyActive_children)
	{
		CEntity* entity, *entity1 = nullptr;
		CComponent* component, *component1 = nullptr;
		std::tie(entity, entity1, component, component1) = GetEntityWithChildren();

		entity->Init();

		entity->SetIsInitiallyActive(false);
		component->SetIsInitiallyActive(true);
		component1->SetIsInitiallyActive(true);
		entity->CheckFirstActivation();
		EXPECT_FALSE(entity->IsActive());
		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component1->IsActive());
		entity->Activate();
		EXPECT_TRUE(entity->IsActive());
		EXPECT_TRUE(component->IsActive());
		EXPECT_TRUE(component1->IsActive());
	}

	TEST_F(CEntityComponentTest, component_no_active_despite_initiallyActive_if_entity_not_initiallyActive_children_recursive)
	{
		CEntity* entity, *entity1, *entity11 = nullptr;
		CComponent* component, *component1, *component11 = nullptr;
		std::tie(entity, entity1, entity11, component, component1, component11) = GetEntityWithChildrenRecursive();

		entity->Init();

		entity->SetIsInitiallyActive(true);
		entity1->SetIsInitiallyActive(false);
		entity11->SetIsInitiallyActive(false);
		entity->CheckFirstActivation();
		EXPECT_TRUE(entity->IsActive());
		EXPECT_FALSE(entity1->IsActive());
		EXPECT_FALSE(entity11->IsActive());
		EXPECT_TRUE(component->IsActive());
		EXPECT_FALSE(component1->IsActive());
		EXPECT_FALSE(component11->IsActive());
		for (int i = 0; i < EntityComponentTestInternal::LOOP_COUNT; ++i)
		{
			entity1->Activate();
			EXPECT_TRUE(entity1->IsActive());
			EXPECT_FALSE(entity11->IsActive());
			EXPECT_TRUE(component1->IsActive());
			EXPECT_FALSE(component11->IsActive());
		}
		for (int i = 0; i < EntityComponentTestInternal::LOOP_COUNT; ++i)
		{
			entity11->Activate();
			EXPECT_TRUE(entity11->IsActive());
			EXPECT_TRUE(component11->IsActive());
		}
	}

	TEST_F(CEntityComponentTest, inactive_entity_activates_component_initiallyActive)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CComponent* component = entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);

		entity->Init();

		entity->SetIsInitiallyActive(false);
		component->SetIsInitiallyActive(true);
		entity->CheckFirstActivation();
		EXPECT_FALSE(entity->IsActive());
		EXPECT_FALSE(component->IsActive());
		entity->Activate();
		EXPECT_TRUE(entity->IsActive());
		EXPECT_TRUE(component->IsActive());
	}

	TEST_F(CEntityComponentTest, inactive_entity_dont_activates_inactive_component_initiallyActive)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CComponent* component = entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);

		entity->Init();

		entity->SetIsInitiallyActive(false);
		component->SetIsInitiallyActive(false);
		entity->CheckFirstActivation();
		EXPECT_FALSE(entity->IsActive());
		EXPECT_FALSE(component->IsActive());
		entity->Activate();
		EXPECT_TRUE(entity->IsActive());
		EXPECT_FALSE(component->IsActive());
	}

	TEST_F(CEntityComponentTest, component_inactive_after_activation_if_entity_inactive)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CComponent* component = entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);

		entity->Init();

		entity->SetIsInitiallyActive(false);
		component->SetIsInitiallyActive(false);
		entity->CheckFirstActivation();
		EXPECT_FALSE(entity->IsActive());
		EXPECT_FALSE(component->IsActive());
		component->Activate();
		EXPECT_FALSE(entity->IsActive());
		EXPECT_FALSE(component->IsActive());
	}

	TEST_F(CEntityComponentTest, component_inactive_after_activation_if_entity_inactive_2)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CComponent* component = entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);

		entity->Init();

		entity->SetIsInitiallyActive(false);
		component->SetIsInitiallyActive(false);
		entity->CheckFirstActivation();
		EXPECT_FALSE(entity->IsActive());
		EXPECT_FALSE(component->IsActive());
		component->Activate();
		EXPECT_FALSE(entity->IsActive());
		EXPECT_FALSE(component->IsActive());
		entity->Activate();
		EXPECT_TRUE(entity->IsActive());
		EXPECT_TRUE(component->IsActive());
	}

	TEST_F(CEntityComponentTest, clone_entity_clone_component)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		EntityComponentTestInternal::CCompFoo* component = entity->AddComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);

		static constexpr int VALUE = 13;

		component->m_x = VALUE;

		CEntity* clone = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, clone);
		clone->CloneFrom(entity);
		EntityComponentTestInternal::CCompFoo* componentClone = clone->GetComponent<EntityComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, componentClone);
		EXPECT_EQ(VALUE, componentClone->m_x);
	}
}
