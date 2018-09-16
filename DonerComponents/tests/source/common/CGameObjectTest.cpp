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
#include <donercomponents/component/CComponentFactoryManager.h>
#include <donercomponents/handle/CHandle.h>

#include <gtest/gtest.h>

namespace DonerComponents
{
	namespace GameObjectTestInternal
	{
		const int LOOP_COUNT = 5;
	}

	class CGameObjectTest : public ::testing::Test
	{
	public:
		CGameObjectTest()
			: m_componentFactoryManager(nullptr)
			, m_gameObjectManager(nullptr)
		{
			CDonerComponentsSystems& systems = CDonerComponentsSystems::CreateInstance()->Init();
			m_componentFactoryManager = systems.GetComponentFactoryManager();
			m_gameObjectManager = systems.GetGameObjectManager();
		}

		~CGameObjectTest()
		{
			CDonerComponentsSystems::DestroyInstance();
		}

		std::tuple<CGameObject*, CGameObject*, CGameObject*> GetGameObjectWithChildren()
		{
			CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
			CGameObject* child1 = m_gameObjectManager->CreateGameObject();
			CGameObject* child11 = m_gameObjectManager->CreateGameObject();
			gameObject->AddChild(child1);
			child1->AddChild(child11);

			return std::make_tuple(gameObject, child1, child11);
		}

		std::tuple<CGameObject*, CGameObject*, CGameObject*, CGameObject*> GetGameObjectWithChildren2()
		{
			CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
			CGameObject* child1 = m_gameObjectManager->CreateGameObject();
			CGameObject* child11 = m_gameObjectManager->CreateGameObject();
			CGameObject* child111 = m_gameObjectManager->CreateGameObject();
			gameObject->AddChild(child1);
			child1->AddChild(child11);
			child11->AddChild(child111);

			return std::make_tuple(gameObject, child1, child11, child111);
		}

		std::tuple<CGameObject*, CGameObject*> GetGameObjectWithChildrenRecursive()
		{
			CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
			CGameObject* gameObject1 = m_gameObjectManager->CreateGameObject();

			gameObject->AddChild(gameObject1);

			return std::make_tuple(gameObject, gameObject1);
		}

		CComponentFactoryManager *m_componentFactoryManager;
		CGameObjectManager *m_gameObjectManager;
	};

	TEST_F(CGameObjectTest, gameObject_set_name)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		static const std::string gameObjectName("TestName");
		gameObject->SetName(gameObjectName.c_str());
		const std::string& name = gameObject->GetName();

		EXPECT_EQ(name, gameObjectName);
	}

	TEST_F(CGameObjectTest, gameObject_created_uninitalized_and_deactivated)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();

		EXPECT_FALSE(gameObject->IsInitialized());
		EXPECT_FALSE(gameObject->IsActive());
	}

	TEST_F(CGameObjectTest, gameObject_add_child_as_gameObject)
	{
		CGameObject* parent = m_gameObjectManager->CreateGameObject();
		CGameObject* child = m_gameObjectManager->CreateGameObject();

		bool success = parent->AddChild(child);
		EXPECT_TRUE(success);
		EXPECT_EQ(1, parent->GetChildrenCount());
		EXPECT_TRUE(parent->HasChild(child));

		CGameObject* childParent = child->GetParent();
		EXPECT_EQ(childParent, parent);
	}

	TEST_F(CGameObjectTest, gameObject_add_child_as_handle)
	{
		CGameObject* parent = m_gameObjectManager->CreateGameObject();
		CHandle child = m_gameObjectManager->CreateGameObject();

		bool success = parent->AddChild(child);
		EXPECT_TRUE(success);
		EXPECT_EQ(1, parent->GetChildrenCount());
		EXPECT_TRUE(parent->HasChild(child));

		CGameObject* childGameObject = child;
		CGameObject* childParent = childGameObject->GetParent();
		EXPECT_EQ(childParent, parent);
	}

	TEST_F(CGameObjectTest, gameObject_add_child_twice_fails)
	{
		CGameObject* parent = m_gameObjectManager->CreateGameObject();
		CHandle child = m_gameObjectManager->CreateGameObject();

		bool success = parent->AddChild(child);
		EXPECT_TRUE(success);
		EXPECT_EQ(1, parent->GetChildrenCount());
		EXPECT_TRUE(parent->HasChild(child));
		success = parent->AddChild(child);
		EXPECT_FALSE(success);
		EXPECT_EQ(1, parent->GetChildrenCount());
	}

	TEST_F(CGameObjectTest, gameObject_remove_child_as_gameObject)
	{
		CGameObject* parent = m_gameObjectManager->CreateGameObject();
		CGameObject* child = m_gameObjectManager->CreateGameObject();

		bool success = parent->AddChild(child);
		EXPECT_TRUE(success);
		success = parent->RemoveChild(child);
		EXPECT_TRUE(success);
		EXPECT_EQ(0, parent->GetChildrenCount());
		EXPECT_FALSE(parent->HasChild(child));

		CGameObject* childParent = child->GetParent();
		EXPECT_EQ(nullptr, childParent);
	}

	TEST_F(CGameObjectTest, gameObject_remove_child_as_handle)
	{
		CGameObject* parent = m_gameObjectManager->CreateGameObject();
		CHandle child = m_gameObjectManager->CreateGameObject();

		bool success = parent->AddChild(child);
		EXPECT_TRUE(success);
		success = parent->RemoveChild(child);
		EXPECT_TRUE(success);
		EXPECT_EQ(0, parent->GetChildrenCount());
		EXPECT_FALSE(parent->HasChild(child));

		CGameObject* childGameObject = child;
		CHandle childParent = childGameObject->GetParent();
		EXPECT_FALSE(static_cast<bool>(childParent));
	}

	TEST_F(CGameObjectTest, gameObject_remove_child_twice_fails)
	{
		CGameObject* parent = m_gameObjectManager->CreateGameObject();
		CGameObject* child = m_gameObjectManager->CreateGameObject();

		bool success = parent->AddChild(child);
		EXPECT_TRUE(success);
		success = parent->RemoveChild(child);
		EXPECT_TRUE(success);
		EXPECT_EQ(0, parent->GetChildrenCount());
		EXPECT_FALSE(parent->HasChild(child));
		success = parent->RemoveChild(child);
		EXPECT_FALSE(success);
	}

	TEST_F(CGameObjectTest, get_children_by_name)
	{
		static const char* const CHILD_NAME("Test");
		CGameObject* parent = m_gameObjectManager->CreateGameObject();
		CGameObject* child = m_gameObjectManager->CreateGameObject();
		child->SetName(CHILD_NAME);

		bool success = parent->AddChild(child);
		EXPECT_TRUE(success);
		EXPECT_EQ(1, parent->GetChildrenCount());
		EXPECT_TRUE(parent->HasChild(child));

		CHandle childHandle = parent->GetChildByName(CHILD_NAME);
		EXPECT_TRUE(static_cast<bool>(childHandle));
		child = childHandle;
		EXPECT_EQ(child->GetName(), std::string(CHILD_NAME));
	}

	TEST_F(CGameObjectTest, get_children_by_invalid_name_return_invalid_handle)
	{
		static const char* const CHILD_NAME("Test");
		static const char* const WRONG_NAME("Test2");
		CGameObject* parent = m_gameObjectManager->CreateGameObject();
		CGameObject* child = m_gameObjectManager->CreateGameObject();
		child->SetName(CHILD_NAME);

		bool success = parent->AddChild(child);
		EXPECT_TRUE(success);
		EXPECT_EQ(1, parent->GetChildrenCount());
		EXPECT_TRUE(parent->HasChild(child));

		CHandle childHandle = parent->GetChildByName(WRONG_NAME);
		EXPECT_FALSE(static_cast<bool>(childHandle));
	}

	TEST_F(CGameObjectTest, get_children_by_index)
	{
		static const char* const CHILD_NAME("Test");
		CGameObject* parent = m_gameObjectManager->CreateGameObject();
		CGameObject* child = m_gameObjectManager->CreateGameObject();
		child->SetName(CHILD_NAME);

		bool success = parent->AddChild(child);
		EXPECT_TRUE(success);
		EXPECT_EQ(1, parent->GetChildrenCount());
		EXPECT_TRUE(parent->HasChild(child));

		CHandle childHandle = parent->GetChildByIndex(0);
		EXPECT_TRUE(static_cast<bool>(childHandle));
		child = childHandle;
		EXPECT_EQ(child->GetName(), std::string(CHILD_NAME));
	}

	TEST_F(CGameObjectTest, get_children_by_invalid_index_return_invalid_handle)
	{
		static const char* const CHILD_NAME("Test");
		CGameObject* parent = m_gameObjectManager->CreateGameObject();
		CGameObject* child = m_gameObjectManager->CreateGameObject();
		child->SetName(CHILD_NAME);

		bool success = parent->AddChild(child);
		EXPECT_TRUE(success);
		EXPECT_EQ(1, parent->GetChildrenCount());
		EXPECT_TRUE(parent->HasChild(child));

		CHandle childHandle = parent->GetChildByIndex(1);
		EXPECT_FALSE(static_cast<bool>(childHandle));
	}

	TEST_F(CGameObjectTest, get_children_by_index_with_no_children)
	{
		CGameObject* parent = m_gameObjectManager->CreateGameObject();

		CHandle childHandle = parent->GetChildByIndex(1);
		EXPECT_FALSE(static_cast<bool>(childHandle));
	}

	TEST_F(CGameObjectTest, gameObject_change_gameObject_parent)
	{
		CGameObject* parent1 = m_gameObjectManager->CreateGameObject();
		CGameObject* parent2 = m_gameObjectManager->CreateGameObject();
		CGameObject* child = m_gameObjectManager->CreateGameObject();

		bool success = parent1->AddChild(child);
		EXPECT_TRUE(success);
		EXPECT_EQ(1, parent1->GetChildrenCount());
		EXPECT_TRUE(parent1->HasChild(child));
		CGameObject* childParent = child->GetParent();
		EXPECT_EQ(parent1, childParent);

		success = parent2->AddChild(child);
		EXPECT_TRUE(success);
		EXPECT_EQ(1, parent2->GetChildrenCount());
		EXPECT_TRUE(parent2->HasChild(child));
		childParent = child->GetParent();
		EXPECT_EQ(parent2, childParent);

		EXPECT_EQ(0, parent1->GetChildrenCount());
		EXPECT_FALSE(parent1->HasChild(child));
	}

	TEST_F(CGameObjectTest, gameObject_delete_also_deletes_children)
	{
		CHandle parent = m_gameObjectManager->CreateGameObject();
		CHandle child = m_gameObjectManager->CreateGameObject();

		CGameObject* parentGameObject = parent;
		bool success = parentGameObject->AddChild(child);
		EXPECT_TRUE(success);
		EXPECT_EQ(1, parentGameObject->GetChildrenCount());
		EXPECT_TRUE(parentGameObject->HasChild(child));

		parentGameObject->Destroy();
		
		m_gameObjectManager->ExecuteScheduledDestroys();

		EXPECT_FALSE(static_cast<bool>(parent));
		EXPECT_FALSE(static_cast<bool>(child));
	}

	TEST_F(CGameObjectTest, gameObject_delete_removes_itself_from_its_parent)
	{
		CHandle parent = m_gameObjectManager->CreateGameObject();
		CHandle child = m_gameObjectManager->CreateGameObject();

		CGameObject* parentGameObject = parent;
		bool success = parentGameObject->AddChild(child);
		EXPECT_TRUE(success);
		EXPECT_EQ(1, parentGameObject->GetChildrenCount());
		EXPECT_TRUE(parentGameObject->HasChild(child));

		CGameObject* childGameObject = child;
		childGameObject->Destroy();

		EXPECT_EQ(0, parentGameObject->GetChildrenCount());
		EXPECT_FALSE(parentGameObject->HasChild(child));

		m_gameObjectManager->ExecuteScheduledDestroys();

		EXPECT_FALSE(static_cast<bool>(child));
	}

	TEST_F(CGameObjectTest, gameObject_not_activated_on_initialization)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();

		EXPECT_FALSE(gameObject->IsInitialized());
		EXPECT_FALSE(gameObject->IsActive());
		gameObject->Init();
		EXPECT_TRUE(gameObject->IsInitialized());
		EXPECT_FALSE(gameObject->IsActive());
	}

	TEST_F(CGameObjectTest, gameObject_CheckFirstActivation_with_initiallyActive_true)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		gameObject->SetIsInitiallyActive(true);
		gameObject->Init();
		gameObject->CheckFirstActivation();
		EXPECT_TRUE(gameObject->IsActive());
	}

	TEST_F(CGameObjectTest, gameObject_CheckFirstActivation_with_initiallyActive_false)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		gameObject->SetIsInitiallyActive(false);
		gameObject->Init();
		gameObject->CheckFirstActivation();
		EXPECT_FALSE(gameObject->IsActive());
	}

	TEST_F(CGameObjectTest, gameObject_CheckFirstActivation_not_working_without_init_called_first)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		gameObject->SetIsInitiallyActive(true);
		gameObject->CheckFirstActivation();
		EXPECT_FALSE(gameObject->IsActive());
	}

	TEST_F(CGameObjectTest, gameObject_activate)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		gameObject->SetIsInitiallyActive(false);
		gameObject->CheckFirstActivation();
		gameObject->Init();
		EXPECT_FALSE(gameObject->IsActive());
		gameObject->Activate();
		EXPECT_TRUE(gameObject->IsActive());
	}

	TEST_F(CGameObjectTest, gameObject_deactivate)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		gameObject->Init();
		gameObject->SetIsInitiallyActive(true);
		gameObject->CheckFirstActivation();
		EXPECT_TRUE(gameObject->IsActive());
		gameObject->Deactivate();
		EXPECT_FALSE(gameObject->IsActive());
	}

	TEST_F(CGameObjectTest, gameObject_deactivate_not_accumulative)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		gameObject->SetIsInitiallyActive(false);
		gameObject->CheckFirstActivation();
		gameObject->Init();
		EXPECT_FALSE(gameObject->IsActive());
		gameObject->Activate();
		EXPECT_TRUE(gameObject->IsActive());

		for (int i = 0; i < GameObjectTestInternal::LOOP_COUNT; ++i)
		{
			gameObject->Deactivate();
			EXPECT_FALSE(gameObject->IsActive());
		}
		gameObject->Activate();
		EXPECT_TRUE(gameObject->IsActive());
	}

	TEST_F(CGameObjectTest, gameObject_init_initalizes_children)
	{
		CGameObject* gameObject, *child1, *child11 = nullptr;
		std::tie(gameObject, child1, child11) = GetGameObjectWithChildren();

		EXPECT_FALSE(gameObject->IsInitialized());
		EXPECT_FALSE(child1->IsInitialized());
		EXPECT_FALSE(child11->IsInitialized());

		gameObject->Init();

		EXPECT_TRUE(gameObject->IsInitialized());
		EXPECT_TRUE(child1->IsInitialized());
		EXPECT_TRUE(child11->IsInitialized());
	}

	TEST_F(CGameObjectTest, gameObject_activate_activates_children)
	{
		CGameObject* gameObject, *child1, *child11 = nullptr;
		std::tie(gameObject, child1, child11) = GetGameObjectWithChildren();

		gameObject->Init();

		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());

		gameObject->Activate();

		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_TRUE(child1->IsActive());
		EXPECT_TRUE(child11->IsActive());
	}

	TEST_F(CGameObjectTest, gameObject_activate_deactivates_children)
	{
		CGameObject* gameObject, *child1, *child11 = nullptr;
		std::tie(gameObject, child1, child11) = GetGameObjectWithChildren();

		gameObject->Init();

		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());

		gameObject->Activate();

		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_TRUE(child1->IsActive());
		EXPECT_TRUE(child11->IsActive());

		gameObject->Deactivate();

		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
	}

	TEST_F(CGameObjectTest, gameObject_intial_activate_true_activates_children)
	{
		CGameObject* gameObject, *child1, *child11 = nullptr;
		std::tie(gameObject, child1, child11) = GetGameObjectWithChildren();

		gameObject->SetIsInitiallyActive(true);
		gameObject->Init();

		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());

		gameObject->CheckFirstActivation();

		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_TRUE(child1->IsActive());
		EXPECT_TRUE(child11->IsActive());
	}

	TEST_F(CGameObjectTest, gameObject_initialllyActive_false_dont_activates_children)
	{
		CGameObject* gameObject, *child1, *child11 = nullptr;
		std::tie(gameObject, child1, child11) = GetGameObjectWithChildren();

		gameObject->SetIsInitiallyActive(false);
		gameObject->Init();

		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());

		gameObject->CheckFirstActivation();

		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
	}

	TEST_F(CGameObjectTest, gameObject_child_initialllyActive_false_breaks_activation_chain)
	{
		CGameObject* gameObject, *child1, *child11, *child111 = nullptr;
		std::tie(gameObject, child1, child11, child111) = GetGameObjectWithChildren2();

		gameObject->SetIsInitiallyActive(true);
		child1->SetIsInitiallyActive(false);
		gameObject->Init();

		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
		EXPECT_FALSE(child111->IsActive());

		gameObject->CheckFirstActivation();

		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
		EXPECT_FALSE(child111->IsActive());
	}

	TEST_F(CGameObjectTest, parent_activate_doesnt_activate_inactive_child)
	{
		CGameObject* gameObject, *child1, *child11, *child111 = nullptr;
		std::tie(gameObject, child1, child11, child111) = GetGameObjectWithChildren2();

		gameObject->SetIsInitiallyActive(true);
		child1->SetIsInitiallyActive(false);
		gameObject->Init();

		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
		EXPECT_FALSE(child111->IsActive());

		gameObject->CheckFirstActivation();

		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
		EXPECT_FALSE(child111->IsActive());

		for (int i = 0; i < GameObjectTestInternal::LOOP_COUNT; ++i)
		{
			gameObject->Activate();

			EXPECT_TRUE(gameObject->IsActive());
			EXPECT_FALSE(child1->IsActive());
			EXPECT_FALSE(child11->IsActive());
			EXPECT_FALSE(child111->IsActive());
		}
	}

	TEST_F(CGameObjectTest, child_activate_after_being_deactivated_activate_children)
	{
		CGameObject* gameObject, *child1, *child11, *child111 = nullptr;
		std::tie(gameObject, child1, child11, child111) = GetGameObjectWithChildren2();

		gameObject->SetIsInitiallyActive(true);
		child1->SetIsInitiallyActive(false);
		gameObject->Init();

		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
		EXPECT_FALSE(child111->IsActive());

		gameObject->CheckFirstActivation();

		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
		EXPECT_FALSE(child111->IsActive());

		child1->Activate();

		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_TRUE(child1->IsActive());
		EXPECT_TRUE(child11->IsActive());
		EXPECT_TRUE(child111->IsActive());
	}

	TEST_F(CGameObjectTest, child_activate_after_being_deactivated_activate_children_but_no_parent)
	{
		CGameObject* gameObject, *child1, *child11, *child111 = nullptr;
		std::tie(gameObject, child1, child11, child111) = GetGameObjectWithChildren2();

		gameObject->SetIsInitiallyActive(true);
		child1->SetIsInitiallyActive(false);
		gameObject->Init();

		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
		EXPECT_FALSE(child111->IsActive());

		gameObject->CheckFirstActivation();

		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
		EXPECT_FALSE(child111->IsActive());

		child11->Activate();

		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_TRUE(child11->IsActive());
		EXPECT_TRUE(child111->IsActive());
	}

	TEST_F(CGameObjectTest, deactivating_deactivated_gameObject_deactivates_its_children)
	{
		CGameObject* gameObject, *child1, *child11, *child111 = nullptr;
		std::tie(gameObject, child1, child11, child111) = GetGameObjectWithChildren2();

		gameObject->SetIsInitiallyActive(true);
		child1->SetIsInitiallyActive(false);
		gameObject->Init();

		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
		EXPECT_FALSE(child111->IsActive());

		gameObject->CheckFirstActivation();

		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
		EXPECT_FALSE(child111->IsActive());

		child11->Activate();

		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_TRUE(child11->IsActive());
		EXPECT_TRUE(child111->IsActive());

		child1->Deactivate();

		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
		EXPECT_FALSE(child111->IsActive());
	}

	TEST_F(CGameObjectTest, deactivating_deactivated_gameObject_deactivates_its_children_cascade)
	{
		CGameObject* gameObject, *child1, *child11, *child111 = nullptr;
		std::tie(gameObject, child1, child11, child111) = GetGameObjectWithChildren2();

		gameObject->SetIsInitiallyActive(true);
		child1->SetIsInitiallyActive(false);
		child11->SetIsInitiallyActive(false);
		child111->SetIsInitiallyActive(false);
		gameObject->Init();

		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
		EXPECT_FALSE(child111->IsActive());

		gameObject->CheckFirstActivation();

		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
		EXPECT_FALSE(child111->IsActive());

		for (int i = 0; i < GameObjectTestInternal::LOOP_COUNT; ++i)
		{
			child1->Activate();

			EXPECT_TRUE(gameObject->IsActive());
			EXPECT_TRUE(child1->IsActive());
			EXPECT_FALSE(child11->IsActive());
			EXPECT_FALSE(child111->IsActive());
		}

		for (int i = 0; i < GameObjectTestInternal::LOOP_COUNT; ++i)
		{
			child11->Activate();

			EXPECT_TRUE(gameObject->IsActive());
			EXPECT_TRUE(child1->IsActive());
			EXPECT_TRUE(child11->IsActive());
			EXPECT_FALSE(child111->IsActive());
		}
	}

	TEST_F(CGameObjectTest, deactivating_deactivated_gameObject_deactivates_its_children_cascade_2)
	{
		CGameObject* gameObject, *child1, *child11, *child111 = nullptr;
		std::tie(gameObject, child1, child11, child111) = GetGameObjectWithChildren2();

		gameObject->SetIsInitiallyActive(true);
		child1->SetIsInitiallyActive(false);
		child11->SetIsInitiallyActive(false);
		child111->SetIsInitiallyActive(false);
		gameObject->Init();

		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
		EXPECT_FALSE(child111->IsActive());

		gameObject->CheckFirstActivation();

		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
		EXPECT_FALSE(child111->IsActive());

		for (int i = 0; i < GameObjectTestInternal::LOOP_COUNT; ++i)
		{
			child1->Activate();

			EXPECT_TRUE(gameObject->IsActive());
			EXPECT_TRUE(child1->IsActive());
			EXPECT_FALSE(child11->IsActive());
			EXPECT_FALSE(child111->IsActive());
		}

		for (int i = 0; i < GameObjectTestInternal::LOOP_COUNT; ++i)
		{
			child1->Deactivate();

			EXPECT_TRUE(gameObject->IsActive());
			EXPECT_FALSE(child1->IsActive());
			EXPECT_FALSE(child11->IsActive());
			EXPECT_FALSE(child111->IsActive());
		}

		for (int i = 0; i < GameObjectTestInternal::LOOP_COUNT; ++i)
		{
			child11->Activate();

			EXPECT_TRUE(gameObject->IsActive());
			EXPECT_FALSE(child1->IsActive());
			EXPECT_TRUE(child11->IsActive());
			EXPECT_FALSE(child111->IsActive());
		}
	}

	TEST_F(CGameObjectTest, accumulated_deactivation_on_initialization)
	{
		CGameObject* gameObject, *child1, *child11, *child111 = nullptr;
		std::tie(gameObject, child1, child11, child111) = GetGameObjectWithChildren2();

		gameObject->SetIsInitiallyActive(true);
		child1->SetIsInitiallyActive(false);
		child11->SetIsInitiallyActive(false);
		gameObject->Init();

		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
		EXPECT_FALSE(child111->IsActive());

		gameObject->CheckFirstActivation();

		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
		EXPECT_FALSE(child111->IsActive());

		for (int i = 0; i < GameObjectTestInternal::LOOP_COUNT; ++i)
		{
			child1->Activate();

			EXPECT_TRUE(gameObject->IsActive());
			EXPECT_TRUE(child1->IsActive());
			EXPECT_FALSE(child11->IsActive());
			EXPECT_FALSE(child111->IsActive());
		}
	}

	TEST_F(CGameObjectTest, accumulated_deactivation_on_initialization_two_levels)
	{
		CGameObject* gameObject, *child1, *child11, *child111 = nullptr;
		std::tie(gameObject, child1, child11, child111) = GetGameObjectWithChildren2();

		gameObject->SetIsInitiallyActive(true);
		child1->SetIsInitiallyActive(false);
		child11->SetIsInitiallyActive(false);
		child111->SetIsInitiallyActive(false);
		gameObject->Init();

		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
		EXPECT_FALSE(child111->IsActive());

		gameObject->CheckFirstActivation();

		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child11->IsActive());
		EXPECT_FALSE(child111->IsActive());

		for (int i = 0; i < GameObjectTestInternal::LOOP_COUNT; ++i)
		{
			child1->Activate();

			EXPECT_TRUE(gameObject->IsActive());
			EXPECT_TRUE(child1->IsActive());
			EXPECT_FALSE(child11->IsActive());
			EXPECT_FALSE(child111->IsActive());
		}

		for (int i = 0; i < GameObjectTestInternal::LOOP_COUNT; ++i)
		{
			child11->Activate();

			EXPECT_TRUE(gameObject->IsActive());
			EXPECT_TRUE(child1->IsActive());
			EXPECT_TRUE(child11->IsActive());
			EXPECT_FALSE(child111->IsActive());
		}
	}

	TEST_F(CGameObjectTest, clone_gameObject)
	{
		CGameObject* gameObject, *child1, *child11 = nullptr;
		std::tie(gameObject, child1, child11) = GetGameObjectWithChildren();

		CGameObject* clonedGameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, clonedGameObject);
		clonedGameObject->CloneFrom(gameObject);
		EXPECT_EQ(gameObject->GetName(), clonedGameObject->GetName());
		EXPECT_FALSE(static_cast<bool>(clonedGameObject->GetParent()));
		EXPECT_EQ(1, clonedGameObject->GetChildrenCount());
		EXPECT_TRUE(clonedGameObject->GetIsInitiallyActive());
		EXPECT_FALSE(clonedGameObject->IsInitialized());
		EXPECT_FALSE(clonedGameObject->IsActive());
		EXPECT_FALSE(clonedGameObject->IsDestroyed());
	}

	TEST_F(CGameObjectTest, clone_gameObject_from_child_dont_inherit_parent)
	{
		CGameObject* gameObject, *child1, *child11 = nullptr;
		std::tie(gameObject, child1, child11) = GetGameObjectWithChildren();

		CGameObject* clonedGameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, clonedGameObject);
		clonedGameObject->CloneFrom(child1);
		EXPECT_EQ(gameObject->GetName(), child1->GetName());
		EXPECT_FALSE(static_cast<bool>(clonedGameObject->GetParent()));
		EXPECT_EQ(1, clonedGameObject->GetChildrenCount());
	}
}
