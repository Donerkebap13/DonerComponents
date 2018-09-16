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
#include <donercomponents/gameObject/CGameObject.h>
#include <donercomponents/component/CComponent.h>
#include <donercomponents/component/CComponentFactoryManager.h>
#include <donercomponents/handle/CHandle.h>

#include <gtest/gtest.h>

namespace DonerComponents
{
	namespace GameObjectComponentTestInternal
	{
		const int LOOP_COUNT = 5;

		class CCompFoo : public CComponent
		{
		public:
			CCompFoo() : m_x(0) {}

			int m_x;
		};

		class CCompBar : public CComponent
		{};
        
        class CCompUnregistered : public CComponent
        {};
	}

	class CGameObjectComponentTest : public ::testing::Test
	{
	public:
		CGameObjectComponentTest()
			: m_componentFactoryManager(nullptr)
			, m_gameObjectManager(nullptr)
		{
			CDonerComponentsSystems& systems = CDonerComponentsSystems::CreateInstance()->Init();
			m_componentFactoryManager = systems.GetComponentFactoryManager();
			m_gameObjectManager = systems.GetGameObjectManager();

			ADD_COMPONENT_FACTORY("foo", GameObjectComponentTestInternal::CCompFoo, 10);
			ADD_COMPONENT_FACTORY("bar", GameObjectComponentTestInternal::CCompBar, 1);
		}

		~CGameObjectComponentTest()
		{
			CDonerComponentsSystems::DestroyInstance();
		}

		std::tuple<CGameObject*, CGameObject*, CComponent*, CComponent*> GetGameObjectWithChildren()
		{
			CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
			CComponent* component = gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
			CGameObject* gameObject1 = m_gameObjectManager->CreateGameObject();
			CComponent* component1 = gameObject1->AddComponent<GameObjectComponentTestInternal::CCompFoo>();

			gameObject->AddChild(gameObject1);

			return std::make_tuple(gameObject, gameObject1, component, component1);
		}

		std::tuple<CGameObject*, CGameObject*, CGameObject*, CComponent*, CComponent*, CComponent*> GetGameObjectWithChildrenRecursive()
		{
			CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
			CComponent* component = gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
			CGameObject* gameObject1 = m_gameObjectManager->CreateGameObject();
			CComponent* component1 = gameObject1->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
			CGameObject* gameObject11 = m_gameObjectManager->CreateGameObject();
			CComponent* component11 = gameObject11->AddComponent<GameObjectComponentTestInternal::CCompFoo>();

			gameObject->AddChild(gameObject1);
			gameObject1->AddChild(gameObject11);

			return std::make_tuple(gameObject, gameObject1, gameObject11, component, component1, component11);
		}

		CComponentFactoryManager *m_componentFactoryManager;
		CGameObjectManager *m_gameObjectManager;
	};

	TEST_F(CGameObjectComponentTest, add_component_by_class)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		CComponent* component = gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		GameObjectComponentTestInternal::CCompFoo* foo = dynamic_cast<GameObjectComponentTestInternal::CCompFoo*>(component);
		EXPECT_NE(nullptr, foo);
		GameObjectComponentTestInternal::CCompBar* bar = dynamic_cast<GameObjectComponentTestInternal::CCompBar*>(component);
		EXPECT_EQ(nullptr, bar);

		CHandle handle = gameObject->AddComponent("bar");
		EXPECT_TRUE(static_cast<bool>(handle));
		component = handle;
		bar = dynamic_cast<GameObjectComponentTestInternal::CCompBar*>(component);
		EXPECT_NE(nullptr, bar);
		foo = dynamic_cast<GameObjectComponentTestInternal::CCompFoo*>(component);
		EXPECT_EQ(nullptr, foo);
	}

	TEST_F(CGameObjectComponentTest, add_component_by_name)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		CComponent* component = gameObject->AddComponent("foo");
		GameObjectComponentTestInternal::CCompFoo* foo = dynamic_cast<GameObjectComponentTestInternal::CCompFoo*>(component);
		EXPECT_NE(nullptr, foo);
		GameObjectComponentTestInternal::CCompBar* bar = dynamic_cast<GameObjectComponentTestInternal::CCompBar*>(component);
		EXPECT_EQ(nullptr, bar);

		CHandle handle = gameObject->AddComponent("bar");
		EXPECT_TRUE(static_cast<bool>(handle));
		component = handle;
		bar = dynamic_cast<GameObjectComponentTestInternal::CCompBar*>(component);
		EXPECT_NE(nullptr, bar);
		foo = dynamic_cast<GameObjectComponentTestInternal::CCompFoo*>(component);
		EXPECT_EQ(nullptr, foo);
	}

	TEST_F(CGameObjectComponentTest, get_component_by_class)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		CComponent* component = gameObject->GetComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);
		GameObjectComponentTestInternal::CCompFoo* foo = dynamic_cast<GameObjectComponentTestInternal::CCompFoo*>(component);
		EXPECT_NE(nullptr, foo);
		GameObjectComponentTestInternal::CCompBar* bar = dynamic_cast<GameObjectComponentTestInternal::CCompBar*>(component);
		EXPECT_EQ(nullptr, bar);
	}
    
    TEST_F(CGameObjectComponentTest, get_invalid_component_by_class_fails)
    {
        CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
        CComponent* component = gameObject->GetComponent<GameObjectComponentTestInternal::CCompFoo>();
        EXPECT_EQ(nullptr, component);
    }

	TEST_F(CGameObjectComponentTest, has_component_by_class)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		bool hasComponent = gameObject->HasComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_TRUE(hasComponent);
	}
    
    TEST_F(CGameObjectComponentTest, has_invalid_component_by_class_fails)
    {
        CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
        bool hasComponent = gameObject->HasComponent<GameObjectComponentTestInternal::CCompFoo>();
        EXPECT_FALSE(hasComponent);
    }

	TEST_F(CGameObjectComponentTest, get_component_by_name)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		CComponent* component = gameObject->GetComponent("foo");
		EXPECT_NE(nullptr, component);
		GameObjectComponentTestInternal::CCompFoo* foo = dynamic_cast<GameObjectComponentTestInternal::CCompFoo*>(component);
		EXPECT_NE(nullptr, foo);
		GameObjectComponentTestInternal::CCompBar* bar = dynamic_cast<GameObjectComponentTestInternal::CCompBar*>(component);
		EXPECT_EQ(nullptr, bar);
	}
    
    TEST_F(CGameObjectComponentTest, get_invalid_component_by_name_fails)
    {
        CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
        CComponent* omponent = gameObject->GetComponent("foo");
        EXPECT_EQ(nullptr, omponent);
    }

	TEST_F(CGameObjectComponentTest, has_component_by_name)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		bool hasComponent = gameObject->HasComponent("foo");
		EXPECT_TRUE(hasComponent);
	}
    
    TEST_F(CGameObjectComponentTest, has_invalid_component_by_name_fails)
    {
        CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
        bool hasComponent = gameObject->HasComponent("foo");
        EXPECT_FALSE(hasComponent);
    }

	TEST_F(CGameObjectComponentTest, remove_component_by_class)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		CHandle handle = gameObject->GetComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_TRUE(static_cast<bool>(handle));

		bool success = gameObject->RemoveComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_TRUE(success);

		CComponent* component = gameObject->GetComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_EQ(nullptr, component);
		EXPECT_FALSE(static_cast<bool>(handle));
	}

	TEST_F(CGameObjectComponentTest, remove_component_by_name)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		CHandle handle = gameObject->GetComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_TRUE(static_cast<bool>(handle));

		bool success = gameObject->RemoveComponent("foo");
		EXPECT_TRUE(success);

		CComponent* component = gameObject->GetComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_EQ(nullptr, component);

		m_componentFactoryManager->ExecuteScheduledDestroys();
		EXPECT_FALSE(static_cast<bool>(handle));
	}

	TEST_F(CGameObjectComponentTest, check_cant_add_component_twice)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		CHandle handle = gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_TRUE(static_cast<bool>(handle));
		handle = gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_FALSE(static_cast<bool>(handle));
	}

	TEST_F(CGameObjectComponentTest, check_cant_remove_component_twice)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		CHandle handle = gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_TRUE(static_cast<bool>(handle));

		bool success = gameObject->RemoveComponent("foo");
		EXPECT_TRUE(success);
		success = gameObject->RemoveComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_FALSE(success);
	}

	TEST_F(CGameObjectComponentTest, check_component_invalid_after_deleting_gameObject)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		CHandle handle = gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_TRUE(static_cast<bool>(handle));

		gameObject->Destroy();
		EXPECT_TRUE(gameObject->IsDestroyed());

		m_componentFactoryManager->ExecuteScheduledDestroys();

		EXPECT_FALSE(static_cast<bool>(handle));
		CComponent* component = handle;
		EXPECT_EQ(nullptr, component);
	}

	TEST_F(CGameObjectComponentTest, check_component_parent)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		CComponent* component = gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();

		CHandle gameObjectHandle = gameObject;
		EXPECT_TRUE(static_cast<bool>(gameObjectHandle));
		CHandle ownerHandle = component->GetOwner();

		EXPECT_EQ(ownerHandle, gameObjectHandle);
	}

	TEST_F(CGameObjectComponentTest, init_gameObject_initialize_components)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		CComponent* component = gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();

		EXPECT_FALSE(gameObject->IsInitialized());
		EXPECT_FALSE(component->IsInitialized());
		gameObject->Init();
		EXPECT_TRUE(gameObject->IsInitialized());
		EXPECT_TRUE(component->IsInitialized());
	}

	TEST_F(CGameObjectComponentTest, destroy_gameObject_destroys_components)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		CHandle compHandle = gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_TRUE(static_cast<bool>(compHandle));

		EXPECT_FALSE(gameObject->IsDestroyed());
		gameObject->Destroy();
		EXPECT_TRUE(gameObject->IsDestroyed());

		m_componentFactoryManager->ExecuteScheduledDestroys();

		EXPECT_FALSE(static_cast<bool>(compHandle));
	}

	TEST_F(CGameObjectComponentTest, activate_gameObject_activates_components)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		CComponent* component = gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		gameObject->Init();
		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(component->IsActive());
		gameObject->Activate();
		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_TRUE(component->IsActive());
	}

	TEST_F(CGameObjectComponentTest, gameObject_activate_activates_components_children)
	{
		CGameObject* gameObject, *gameObject1 = nullptr;
		CComponent* component, *component1 = nullptr;
		std::tie(gameObject, gameObject1, component, component1) = GetGameObjectWithChildren();

		gameObject->Init();
		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component1->IsActive());
		gameObject->Activate();
		EXPECT_TRUE(component->IsActive());
		EXPECT_TRUE(component1->IsActive());
	}

	TEST_F(CGameObjectComponentTest, gameObject_activate_activates_components_children_recursive)
	{
		CGameObject* gameObject, *gameObject1, *gameObject11 = nullptr;
		CComponent* component, *component1, *component11 = nullptr;
		std::tie(gameObject, gameObject1, gameObject11, component, component1, component11) = GetGameObjectWithChildrenRecursive();

		gameObject->Init();
		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component1->IsActive());
		EXPECT_FALSE(component11->IsActive());
		gameObject->Activate();
		EXPECT_TRUE(component->IsActive());
		EXPECT_TRUE(component1->IsActive());
		EXPECT_TRUE(component11->IsActive());
	}

	TEST_F(CGameObjectComponentTest, deactivate_gameObject_deactivates_components)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		CComponent* component = gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);

		gameObject->Init();

		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(component->IsActive());
		gameObject->Activate();
		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_TRUE(component->IsActive());

		gameObject->Deactivate();
		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(component->IsActive());
	}

	TEST_F(CGameObjectComponentTest, deactivate_gameObject_deactivates_components_children)
	{
		CGameObject* gameObject, *gameObject1 = nullptr;
		CComponent* component, *component1 = nullptr;
		std::tie(gameObject, gameObject1, component, component1) = GetGameObjectWithChildren();

		gameObject->Init();

		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component1->IsActive());
		gameObject->Activate();
		EXPECT_TRUE(component->IsActive());
		EXPECT_TRUE(component1->IsActive());

		gameObject->Deactivate();
		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component1->IsActive());
	}

	TEST_F(CGameObjectComponentTest, deactivate_gameObject_deactivates_components_children_recursive)
	{
		CGameObject* gameObject, *gameObject1, *gameObject11 = nullptr;
		CComponent* component, *component1, *component11 = nullptr;
		std::tie(gameObject, gameObject1, gameObject11, component, component1, component11) = GetGameObjectWithChildrenRecursive();

		gameObject->Init();

		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component1->IsActive());
		EXPECT_FALSE(component11->IsActive());
		gameObject->Activate();
		EXPECT_TRUE(component->IsActive());
		EXPECT_TRUE(component1->IsActive());
		EXPECT_TRUE(component11->IsActive());

		gameObject->Deactivate();
		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component1->IsActive());
		EXPECT_FALSE(component11->IsActive());
	}

	TEST_F(CGameObjectComponentTest, gameObject_and_component_initiallyActive)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		CComponent* component = gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);

		gameObject->Init();

		gameObject->SetIsInitiallyActive(true);
		component->SetIsInitiallyActive(true);
		gameObject->CheckFirstActivation();
		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_TRUE(component->IsActive());
	}

	TEST_F(CGameObjectComponentTest, gameObject_and_component_initiallyActive_children)
	{
		CGameObject* gameObject, *gameObject1 = nullptr;
		CComponent* component, *component1 = nullptr;
		std::tie(gameObject, gameObject1, component, component1) = GetGameObjectWithChildren();

		gameObject->Init();

		gameObject->SetIsInitiallyActive(true);
		gameObject1->SetIsInitiallyActive(true);
		component->SetIsInitiallyActive(true);
		component1->SetIsInitiallyActive(true);
		gameObject->CheckFirstActivation();
		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_TRUE(gameObject1->IsActive());
		EXPECT_TRUE(component->IsActive());
		EXPECT_TRUE(component1->IsActive());
	}

	TEST_F(CGameObjectComponentTest, gameObject_and_component_initiallyActive_children_recursive)
	{
		CGameObject* gameObject, *gameObject1, *gameObject11 = nullptr;
		CComponent* component, *component1, *component11 = nullptr;
		std::tie(gameObject, gameObject1, gameObject11, component, component1, component11) = GetGameObjectWithChildrenRecursive();

		gameObject->Init();

		gameObject->SetIsInitiallyActive(true);
		gameObject1->SetIsInitiallyActive(true);
		gameObject11->SetIsInitiallyActive(true);
		component->SetIsInitiallyActive(true);
		component1->SetIsInitiallyActive(true);
		component11->SetIsInitiallyActive(true);
		gameObject->CheckFirstActivation();
		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_TRUE(gameObject1->IsActive());
		EXPECT_TRUE(gameObject11->IsActive());
		EXPECT_TRUE(component->IsActive());
		EXPECT_TRUE(component1->IsActive());
		EXPECT_TRUE(component11->IsActive());
	}

	TEST_F(CGameObjectComponentTest, gameObject_active_and_component_deactivated_initiallyActive)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		CComponent* component = gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);

		gameObject->Init();

		gameObject->SetIsInitiallyActive(true);
		component->SetIsInitiallyActive(false);
		gameObject->CheckFirstActivation();
		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_FALSE(component->IsActive());
	}

	TEST_F(CGameObjectComponentTest, gameObject_active_and_component_deactivated_initiallyActive_children)
	{
		CGameObject* gameObject, *gameObject1 = nullptr;
		CComponent* component, *component1 = nullptr;
		std::tie(gameObject, gameObject1, component, component1) = GetGameObjectWithChildren();

		gameObject->Init();

		component->SetIsInitiallyActive(false);
		component1->SetIsInitiallyActive(false);
		gameObject->CheckFirstActivation();
		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_TRUE(gameObject1->IsActive());
		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component1->IsActive());

		for (int i = 0; i < GameObjectComponentTestInternal::LOOP_COUNT; ++i)
		{
			gameObject->Activate();
			EXPECT_FALSE(component->IsActive());
			EXPECT_FALSE(component1->IsActive());
		}
		for (int i = 0; i < GameObjectComponentTestInternal::LOOP_COUNT; ++i)
		{
			gameObject1->Activate();
			EXPECT_FALSE(component->IsActive());
			EXPECT_FALSE(component1->IsActive());
		}
	}

	TEST_F(CGameObjectComponentTest, gameObject_active_and_component_deactivated_initiallyActive_children_recursive)
	{
		CGameObject* gameObject, *gameObject1, *gameObject11 = nullptr;
		CComponent* component, *component1, *component11 = nullptr;
		std::tie(gameObject, gameObject1, gameObject11, component, component1, component11) = GetGameObjectWithChildrenRecursive();

		gameObject->Init();

		component->SetIsInitiallyActive(false);
		component1->SetIsInitiallyActive(false);
		component11->SetIsInitiallyActive(false);
		gameObject->CheckFirstActivation();
		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_TRUE(gameObject1->IsActive());
		EXPECT_TRUE(gameObject11->IsActive());
		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component1->IsActive());
		EXPECT_FALSE(component11->IsActive());

		for (int i = 0; i < GameObjectComponentTestInternal::LOOP_COUNT; ++i)
		{
			gameObject->Activate();
			EXPECT_FALSE(component->IsActive());
			EXPECT_FALSE(component1->IsActive());
			EXPECT_FALSE(component11->IsActive());
		}
		for (int i = 0; i < GameObjectComponentTestInternal::LOOP_COUNT; ++i)
		{
			gameObject1->Activate();
			EXPECT_FALSE(component->IsActive());
			EXPECT_FALSE(component1->IsActive());
			EXPECT_FALSE(component11->IsActive());
		}
	}

	TEST_F(CGameObjectComponentTest, component_no_active_despite_initiallyActive_if_gameObject_not_initiallyActive)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		CComponent* component = gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);

		gameObject->Init();

		gameObject->SetIsInitiallyActive(false);
		component->SetIsInitiallyActive(true);
		gameObject->CheckFirstActivation();
		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(component->IsActive());
	}

	TEST_F(CGameObjectComponentTest, component_no_active_despite_initiallyActive_if_gameObject_not_initiallyActive_children)
	{
		CGameObject* gameObject, *gameObject1 = nullptr;
		CComponent* component, *component1 = nullptr;
		std::tie(gameObject, gameObject1, component, component1) = GetGameObjectWithChildren();

		gameObject->Init();

		gameObject->SetIsInitiallyActive(false);
		component->SetIsInitiallyActive(true);
		component1->SetIsInitiallyActive(true);
		gameObject->CheckFirstActivation();
		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component1->IsActive());
		gameObject->Activate();
		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_TRUE(component->IsActive());
		EXPECT_TRUE(component1->IsActive());
	}

	TEST_F(CGameObjectComponentTest, component_no_active_despite_initiallyActive_if_gameObject_not_initiallyActive_children_recursive)
	{
		CGameObject* gameObject, *gameObject1, *gameObject11 = nullptr;
		CComponent* component, *component1, *component11 = nullptr;
		std::tie(gameObject, gameObject1, gameObject11, component, component1, component11) = GetGameObjectWithChildrenRecursive();

		gameObject->Init();

		gameObject->SetIsInitiallyActive(true);
		gameObject1->SetIsInitiallyActive(false);
		gameObject11->SetIsInitiallyActive(false);
		gameObject->CheckFirstActivation();
		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_FALSE(gameObject1->IsActive());
		EXPECT_FALSE(gameObject11->IsActive());
		EXPECT_TRUE(component->IsActive());
		EXPECT_FALSE(component1->IsActive());
		EXPECT_FALSE(component11->IsActive());
		for (int i = 0; i < GameObjectComponentTestInternal::LOOP_COUNT; ++i)
		{
			gameObject1->Activate();
			EXPECT_TRUE(gameObject1->IsActive());
			EXPECT_FALSE(gameObject11->IsActive());
			EXPECT_TRUE(component1->IsActive());
			EXPECT_FALSE(component11->IsActive());
		}
		for (int i = 0; i < GameObjectComponentTestInternal::LOOP_COUNT; ++i)
		{
			gameObject11->Activate();
			EXPECT_TRUE(gameObject11->IsActive());
			EXPECT_TRUE(component11->IsActive());
		}
	}

	TEST_F(CGameObjectComponentTest, inactive_gameObject_activates_component_initiallyActive)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		CComponent* component = gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);

		gameObject->Init();

		gameObject->SetIsInitiallyActive(false);
		component->SetIsInitiallyActive(true);
		gameObject->CheckFirstActivation();
		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(component->IsActive());
		gameObject->Activate();
		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_TRUE(component->IsActive());
	}

	TEST_F(CGameObjectComponentTest, inactive_gameObject_dont_activates_inactive_component_initiallyActive)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		CComponent* component = gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);

		gameObject->Init();

		gameObject->SetIsInitiallyActive(false);
		component->SetIsInitiallyActive(false);
		gameObject->CheckFirstActivation();
		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(component->IsActive());
		gameObject->Activate();
		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_FALSE(component->IsActive());
	}

	TEST_F(CGameObjectComponentTest, component_inactive_after_activation_if_gameObject_inactive)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		CComponent* component = gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);

		gameObject->Init();

		gameObject->SetIsInitiallyActive(false);
		component->SetIsInitiallyActive(false);
		gameObject->CheckFirstActivation();
		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(component->IsActive());
		component->Activate();
		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(component->IsActive());
	}

	TEST_F(CGameObjectComponentTest, component_inactive_after_activation_if_gameObject_inactive_2)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		CComponent* component = gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);

		gameObject->Init();

		gameObject->SetIsInitiallyActive(false);
		component->SetIsInitiallyActive(false);
		gameObject->CheckFirstActivation();
		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(component->IsActive());
		component->Activate();
		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(component->IsActive());
		gameObject->Activate();
		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_TRUE(component->IsActive());
	}

	TEST_F(CGameObjectComponentTest, clone_gameObject_clone_component)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		GameObjectComponentTestInternal::CCompFoo* component = gameObject->AddComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);

		static constexpr int VALUE = 13;

		component->m_x = VALUE;

		CGameObject* clone = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, clone);
		clone->CloneFrom(gameObject);
		GameObjectComponentTestInternal::CCompFoo* componentClone = clone->GetComponent<GameObjectComponentTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, componentClone);
		EXPECT_EQ(VALUE, componentClone->m_x);
	}
    
    TEST_F(CGameObjectComponentTest, get_unregistered_component_by_class_fails)
    {
        CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
        EXPECT_NE(nullptr, gameObject);
        CComponent* component = gameObject->GetComponent<GameObjectComponentTestInternal::CCompUnregistered>();
        EXPECT_EQ(nullptr, component);
    }
    
    TEST_F(CGameObjectComponentTest, get_unregistered_component_by_name_fails)
    {
        CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
        EXPECT_NE(nullptr, gameObject);
        CComponent* component = gameObject->GetComponent("CompUnregistered");
        EXPECT_EQ(nullptr, component);
    }
}
