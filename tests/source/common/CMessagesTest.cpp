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

#include <donerecs/component/CComponent.h>
#include <donerecs/component/CComponentFactoryManager.h>
#include <donerecs/entity/CEntityManager.h>
#include <donerecs/handle/CHandle.h>

#include <gtest/gtest.h>

namespace DonerECS
{
	namespace MessagesTestInternal
	{
		const int LOOP_COUNT = 5;
		const int TEST_VALUE = 10;

		struct STestMessage
		{
			STestMessage(int dummy) : m_dummy(dummy) {}
			int m_dummy;
		};

		struct STestMessage2
		{};

		class CCompFoo : public CComponent
		{
		public:
			CCompFoo()
				: m_foo(0)
			{}

			void RegisterMessages() override
			{
				RegisterMessage(&CCompFoo::OnTestMessage);
			}

			void OnTestMessage(const STestMessage& message)
			{
				m_foo = message.m_dummy;
			}

			int m_foo;
		};

		class CCompBar : public CComponent
		{
		public:
			CCompBar()
				: m_bar(0)
			{}

			void RegisterMessages() override
			{
				RegisterMessage(&CCompBar::OnTestMessage);
				RegisterMessage(&CCompBar::OnTestMessage2);
			}

			void OnTestMessage(const STestMessage& message)
			{
				m_bar += message.m_dummy;
			}

			void OnTestMessage2(const STestMessage2& /*message*/)
			{
				m_bar = 1337;
			}

			int m_bar;
		};
	}

	class CMessagesTest : public ::testing::Test
	{
	public:
		CMessagesTest()
			: m_componentFactoryManager(CComponentFactoryManager::CreateInstance())
			, m_entityManager(CEntityManager::CreateInstance())
		{
			ADD_COMPONENT_FACTORY("foo", MessagesTestInternal::CCompFoo, 2);
			ADD_COMPONENT_FACTORY("bar", MessagesTestInternal::CCompBar, 2);
		}

		~CMessagesTest()
		{
			CComponentFactoryManager::DestroyInstance();
			CEntityManager::DestroyInstance();
		}

		CComponentFactoryManager *m_componentFactoryManager;
		CEntityManager *m_entityManager;
	};

	TEST_F(CMessagesTest, component_receives_registered_sendMessage)
	{
		MessagesTestInternal::CCompFoo* compFoo = static_cast<MessagesTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<MessagesTestInternal::CCompFoo>());
		EXPECT_NE(nullptr, compFoo);

		compFoo->Init();
		compFoo->Activate();
		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		compFoo->SendMessage(message);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, component_ignores_unregistered_sendMessage)
	{
		MessagesTestInternal::CCompFoo* compFoo = static_cast<MessagesTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<MessagesTestInternal::CCompFoo>());
		EXPECT_NE(nullptr, compFoo);

		compFoo->Init();
		compFoo->Activate();
		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage2 message;
		compFoo->SendMessage(message);
		EXPECT_EQ(0, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, entity_with_component_receives_message)
	{
		CEntity* entity = m_entityManager->GetNewElement();
		EXPECT_NE(nullptr, entity);
		MessagesTestInternal::CCompFoo* compFoo = entity->AddComponent<MessagesTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, compFoo);

		entity->Init();
		entity->Activate();

		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		entity->SendMessage(message);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, not_initialized_entity_with_component_dont_receives_message)
	{
		CEntity* entity = m_entityManager->GetNewElement();
		EXPECT_NE(nullptr, entity);
		MessagesTestInternal::CCompFoo* compFoo = entity->AddComponent<MessagesTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, compFoo);

		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		entity->SendMessage(message);
		EXPECT_EQ(0, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, not_active_entity_with_component_dont_receives_message)
	{
		CEntity* entity = m_entityManager->GetNewElement();
		EXPECT_NE(nullptr, entity);
		MessagesTestInternal::CCompFoo* compFoo = entity->AddComponent<MessagesTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, compFoo);

		entity->Init();

		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		entity->SendMessage(message);
		EXPECT_EQ(0, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, two_different_components_in_same_entity_receives_same_message)
	{
		CEntity* entity = m_entityManager->GetNewElement();
		EXPECT_NE(nullptr, entity);
		MessagesTestInternal::CCompFoo* compFoo = entity->AddComponent<MessagesTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, compFoo);
		MessagesTestInternal::CCompBar* compBar = entity->AddComponent<MessagesTestInternal::CCompBar>();
		EXPECT_NE(nullptr, compBar);

		entity->Init();
		entity->Activate();

		EXPECT_EQ(0, compFoo->m_foo);
		EXPECT_EQ(0, compBar->m_bar);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		entity->SendMessage(message);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compBar->m_bar);
	}

	TEST_F(CMessagesTest, component_can_recive_same_message_multiple_times)
	{
		CEntity* entity = m_entityManager->GetNewElement();
		EXPECT_NE(nullptr, entity);
		MessagesTestInternal::CCompBar* compBar = entity->AddComponent<MessagesTestInternal::CCompBar>();
		EXPECT_NE(nullptr, compBar);

		entity->Init();
		entity->Activate();

		EXPECT_EQ(0, compBar->m_bar);
		for (int i = 1; i <= MessagesTestInternal::LOOP_COUNT; ++i)
		{
			MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
			entity->SendMessage(message);
			EXPECT_EQ(MessagesTestInternal::TEST_VALUE * i, compBar->m_bar);
		}
	}

	TEST_F(CMessagesTest, child_entity_with_component_receives_message_with_SendMessageRecursive)
	{
		CEntity* entity = m_entityManager->GetNewElement();
		EXPECT_NE(nullptr, entity);
		CEntity* entity1 = m_entityManager->GetNewElement();
		EXPECT_NE(nullptr, entity1);
		entity->AddChild(entity1);
		MessagesTestInternal::CCompFoo* compFoo = entity1->AddComponent<MessagesTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, compFoo);

		entity->Init();
		entity->Activate();

		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		entity->SendMessageRecursive(message);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, child_of_child_entity_with_different_components_receives_message_with_SendMessageRecursive)
	{
		CEntity* entity = m_entityManager->GetNewElement();
		EXPECT_NE(nullptr, entity);
		CEntity* entity1 = m_entityManager->GetNewElement();
		EXPECT_NE(nullptr, entity1);
		entity->AddChild(entity1);
		MessagesTestInternal::CCompFoo* compFoo = entity1->AddComponent<MessagesTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, compFoo);
		CEntity* entity11 = m_entityManager->GetNewElement();
		EXPECT_NE(nullptr, entity11);
		entity1->AddChild(entity11);
		MessagesTestInternal::CCompBar* compBar = entity11->AddComponent<MessagesTestInternal::CCompBar>();
		EXPECT_NE(nullptr, compBar);

		entity->Init();
		entity->Activate();

		EXPECT_EQ(0, compFoo->m_foo);
		EXPECT_EQ(0, compBar->m_bar);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		entity->SendMessageRecursive(message);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compBar->m_bar);
	}

	TEST_F(CMessagesTest, inactive_child_doesnt_receives_message)
	{
		CEntity* entity = m_entityManager->GetNewElement();
		EXPECT_NE(nullptr, entity);
		CEntity* entity1 = m_entityManager->GetNewElement();
		EXPECT_NE(nullptr, entity1);
		entity->AddChild(entity1);
		MessagesTestInternal::CCompFoo* compFoo = entity1->AddComponent<MessagesTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, compFoo);

		entity->Init();
		entity->Activate();
		entity1->Deactivate();

		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		entity->SendMessageRecursive(message);
		EXPECT_EQ(0, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, two_different_entities_receives_same_message_through_BroadcastMessage)
	{
		CEntity* entity = m_entityManager->GetNewElement();
		EXPECT_NE(nullptr, entity);
		CEntity* entity2 = m_entityManager->GetNewElement();
		EXPECT_NE(nullptr, entity2);
		MessagesTestInternal::CCompFoo* compFoo = entity->AddComponent<MessagesTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, compFoo);
		MessagesTestInternal::CCompBar* compBar = entity2->AddComponent<MessagesTestInternal::CCompBar>();
		EXPECT_NE(nullptr, compBar);

		entity->Init();
		entity->Activate();
		entity2->Init();
		entity2->Activate();

		EXPECT_EQ(0, compFoo->m_foo);
		EXPECT_EQ(0, compBar->m_bar);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		CEntityManager::Get()->BroadcastMessage(message);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compBar->m_bar);
	}

	TEST_F(CMessagesTest, child_entity_receives_message_through_BroadcastMessage_only_once)
	{
		CEntity* entity = m_entityManager->GetNewElement();
		EXPECT_NE(nullptr, entity);
		CEntity* entity1 = m_entityManager->GetNewElement();
		EXPECT_NE(nullptr, entity1);
		entity->AddChild(entity1);
		MessagesTestInternal::CCompFoo* compFoo = entity->AddComponent<MessagesTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, compFoo);
		MessagesTestInternal::CCompBar* compBar = entity1->AddComponent<MessagesTestInternal::CCompBar>();
		EXPECT_NE(nullptr, compBar);

		entity->Init();
		entity->Activate();

		EXPECT_EQ(0, compFoo->m_foo);
		EXPECT_EQ(0, compBar->m_bar);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		CEntityManager::Get()->BroadcastMessage(message);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compBar->m_bar);

		CEntityManager::Get()->BroadcastMessage(message);

		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE * 2, compBar->m_bar);
	}
}
