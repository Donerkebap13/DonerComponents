////////////////////////////////////////////////////////////
//
// MIT License
//
// DonerECS - A Tweaked GameObject-Component System
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

#include <donerecs/CDonerECSSystems.h>
#include <donerecs/component/CComponent.h>
#include <donerecs/component/CComponentFactoryManager.h>
#include <donerecs/gameObject/CGameObject.h>
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

		struct SPostMessage
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
				RegisterMessage(&CCompFoo::OnPostMessage);
			}

			void OnTestMessage(const STestMessage& message)
			{
				m_foo = message.m_dummy;
			}

			void OnPostMessage(const SPostMessage& message)
			{
				++m_foo;
				m_owner.PostMessage(message);
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
			: m_componentFactoryManager(nullptr)
			, m_gameObjectManager(nullptr)
		{
			CDonerECSSystems& systems = CDonerECSSystems::CreateInstance()->Init();
			m_componentFactoryManager = systems.GetComponentFactoryManager();
			m_gameObjectManager = systems.GetGameObjectManager();

			ADD_COMPONENT_FACTORY("foo", MessagesTestInternal::CCompFoo, 2);
			ADD_COMPONENT_FACTORY("bar", MessagesTestInternal::CCompBar, 2);
		}

		~CMessagesTest()
		{
			CDonerECSSystems::DestroyInstance();
		}

		CComponentFactoryManager *m_componentFactoryManager;
		CGameObjectManager *m_gameObjectManager;
	};

	TEST_F(CMessagesTest, component_receives_registered_sendMessage)
	{
		MessagesTestInternal::CCompFoo* compFoo = static_cast<MessagesTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<MessagesTestInternal::CCompFoo>());

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

		compFoo->Init();
		compFoo->Activate();
		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage2 message;
		compFoo->SendMessage(message);
		EXPECT_EQ(0, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, component_receives_registered_sendMessage_through_handle)
	{
		MessagesTestInternal::CCompFoo* compFoo = static_cast<MessagesTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<MessagesTestInternal::CCompFoo>());

		compFoo->Init();
		compFoo->Activate();
		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		CHandle compFooHandle = compFoo;
		compFooHandle.SendMessage(message);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, gameObject_with_component_receives_message)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		MessagesTestInternal::CCompFoo* compFoo = gameObject->AddComponent<MessagesTestInternal::CCompFoo>();

		gameObject->Init();
		gameObject->Activate();

		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		gameObject->SendMessage(message);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, gameObject_with_component_receives_message_through_handle)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		MessagesTestInternal::CCompFoo* compFoo = gameObject->AddComponent<MessagesTestInternal::CCompFoo>();

		gameObject->Init();
		gameObject->Activate();

		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		CHandle gameObjectHandle = gameObject;
		gameObjectHandle.SendMessage(message);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, not_initialized_gameObject_with_component_dont_receives_message)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		MessagesTestInternal::CCompFoo* compFoo = gameObject->AddComponent<MessagesTestInternal::CCompFoo>();

		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		gameObject->SendMessage(message);
		EXPECT_EQ(0, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, not_active_gameObject_with_component_doesnt_receives_message)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		MessagesTestInternal::CCompFoo* compFoo = gameObject->AddComponent<MessagesTestInternal::CCompFoo>();

		gameObject->Init();

		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		gameObject->SendMessage(message);
		EXPECT_EQ(0, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, two_different_components_in_same_gameObject_receives_same_message)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		MessagesTestInternal::CCompFoo* compFoo = gameObject->AddComponent<MessagesTestInternal::CCompFoo>();
		MessagesTestInternal::CCompBar* compBar = gameObject->AddComponent<MessagesTestInternal::CCompBar>();

		gameObject->Init();
		gameObject->Activate();

		EXPECT_EQ(0, compFoo->m_foo);
		EXPECT_EQ(0, compBar->m_bar);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		gameObject->SendMessage(message);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compBar->m_bar);
	}

	TEST_F(CMessagesTest, component_can_recive_same_message_multiple_times)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		MessagesTestInternal::CCompBar* compBar = gameObject->AddComponent<MessagesTestInternal::CCompBar>();

		gameObject->Init();
		gameObject->Activate();

		EXPECT_EQ(0, compBar->m_bar);
		for (int i = 1; i <= MessagesTestInternal::LOOP_COUNT; ++i)
		{
			MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
			gameObject->SendMessage(message);
			EXPECT_EQ(MessagesTestInternal::TEST_VALUE * i, compBar->m_bar);
		}
	}

	TEST_F(CMessagesTest, child_gameObject_with_component_receives_message_with_SendMessageToChildren)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		CGameObject* gameObject1 = m_gameObjectManager->CreateGameObject();
		gameObject->AddChild(gameObject1);
		MessagesTestInternal::CCompFoo* compFoo = gameObject1->AddComponent<MessagesTestInternal::CCompFoo>();

		gameObject->Init();
		gameObject->Activate();

		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		gameObject->SendMessageToChildren(message);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, parent_gameObject_with_component_doesnt_receive_message_with_SendMessageToChildren)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		CGameObject* gameObject1 = m_gameObjectManager->CreateGameObject();
		MessagesTestInternal::CCompFoo* compFoo = gameObject->AddComponent<MessagesTestInternal::CCompFoo>();
		gameObject->AddChild(gameObject1);
		MessagesTestInternal::CCompFoo* compFoo1 = gameObject1->AddComponent<MessagesTestInternal::CCompFoo>();

		gameObject->Init();
		gameObject->Activate();

		EXPECT_EQ(0, compFoo->m_foo);
		EXPECT_EQ(0, compFoo1->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		gameObject->SendMessageToChildren(message);
		EXPECT_EQ(0, compFoo->m_foo);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo1->m_foo);
	}

	TEST_F(CMessagesTest, child_of_child_gameObject_with_different_components_receives_message_with_SendMessageToChildren_recursive)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		MessagesTestInternal::CCompFoo* compFoo0 = gameObject->AddComponent<MessagesTestInternal::CCompFoo>();
		CGameObject* gameObject1 = m_gameObjectManager->CreateGameObject();
		gameObject->AddChild(gameObject1);
		MessagesTestInternal::CCompFoo* compFoo = gameObject1->AddComponent<MessagesTestInternal::CCompFoo>();
		CGameObject* gameObject11 = m_gameObjectManager->CreateGameObject();
		gameObject1->AddChild(gameObject11);
		MessagesTestInternal::CCompBar* compBar = gameObject11->AddComponent<MessagesTestInternal::CCompBar>();

		gameObject->Init();
		gameObject->Activate();

		EXPECT_EQ(0, compFoo0->m_foo);
		EXPECT_EQ(0, compFoo->m_foo);
		EXPECT_EQ(0, compBar->m_bar);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		gameObject->SendMessageToChildren(message, ESendMessageType::Recursive);
		EXPECT_EQ(0, compFoo0->m_foo);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compBar->m_bar);
	}

	TEST_F(CMessagesTest, child_gameObject_with_component_receives_message_with_SendMessage_recursive_through_handle)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		MessagesTestInternal::CCompFoo* compFoo0 = gameObject->AddComponent<MessagesTestInternal::CCompFoo>();
		CGameObject* gameObject1 = m_gameObjectManager->CreateGameObject();
		gameObject->AddChild(gameObject1);
		MessagesTestInternal::CCompFoo* compFoo = gameObject1->AddComponent<MessagesTestInternal::CCompFoo>();

		gameObject->Init();
		gameObject->Activate();

		EXPECT_EQ(0, compFoo0->m_foo);
		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		CHandle gameObjectHandle = gameObject;
		gameObjectHandle.SendMessage(message, ESendMessageType::Recursive);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo0->m_foo);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, child_gameObject_with_component_receives_message_with_SendMessageToChildren_recursive_through_handle)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		MessagesTestInternal::CCompFoo* compFoo0 = gameObject->AddComponent<MessagesTestInternal::CCompFoo>();
		CGameObject* gameObject1 = m_gameObjectManager->CreateGameObject();
		gameObject->AddChild(gameObject1);
		MessagesTestInternal::CCompFoo* compFoo = gameObject1->AddComponent<MessagesTestInternal::CCompFoo>();

		gameObject->Init();
		gameObject->Activate();

		EXPECT_EQ(0, compFoo0->m_foo);
		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		CHandle gameObjectHandle = gameObject;
		gameObjectHandle.SendMessageToChildren(message, ESendMessageType::Recursive);
		EXPECT_EQ(0, compFoo0->m_foo);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, inactive_child_doesnt_receives_message)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		CGameObject* gameObject1 = m_gameObjectManager->CreateGameObject();
		gameObject->AddChild(gameObject1);
		MessagesTestInternal::CCompFoo* compFoo = gameObject1->AddComponent<MessagesTestInternal::CCompFoo>();

		gameObject->Init();
		gameObject->Activate();
		gameObject1->Deactivate();

		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		gameObject->SendMessage(message, ESendMessageType::Recursive);
		EXPECT_EQ(0, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, two_different_entities_receives_same_message_through_BroadcastMessage)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		CGameObject* gameObject2 = m_gameObjectManager->CreateGameObject();
		MessagesTestInternal::CCompFoo* compFoo = gameObject->AddComponent<MessagesTestInternal::CCompFoo>();
		MessagesTestInternal::CCompBar* compBar = gameObject2->AddComponent<MessagesTestInternal::CCompBar>();

		gameObject->Init();
		gameObject->Activate();
		gameObject2->Init();
		gameObject2->Activate();

		EXPECT_EQ(0, compFoo->m_foo);
		EXPECT_EQ(0, compBar->m_bar);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		m_gameObjectManager->BroadcastMessage(message);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compBar->m_bar);
	}

	TEST_F(CMessagesTest, child_gameObject_receives_message_through_BroadcastMessage_only_once)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		CGameObject* gameObject1 = m_gameObjectManager->CreateGameObject();
		gameObject->AddChild(gameObject1);
		MessagesTestInternal::CCompFoo* compFoo = gameObject->AddComponent<MessagesTestInternal::CCompFoo>();
		MessagesTestInternal::CCompBar* compBar = gameObject1->AddComponent<MessagesTestInternal::CCompBar>();

		gameObject->Init();
		gameObject->Activate();

		EXPECT_EQ(0, compFoo->m_foo);
		EXPECT_EQ(0, compBar->m_bar);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		m_gameObjectManager->BroadcastMessage(message);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compBar->m_bar);

		m_gameObjectManager->BroadcastMessage(message);

		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE * 2, compBar->m_bar);
	}

	TEST_F(CMessagesTest, gameObject_with_component_receives_postMessage)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		MessagesTestInternal::CCompFoo* compFoo = gameObject->AddComponent<MessagesTestInternal::CCompFoo>();

		gameObject->Init();
		gameObject->Activate();

		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		gameObject->PostMessage(message);
		EXPECT_EQ(0, compFoo->m_foo);

		m_gameObjectManager->SendPostMsgs();

		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, gameObject_with_component_receives_postMessage_through_handle)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		MessagesTestInternal::CCompFoo* compFoo = gameObject->AddComponent<MessagesTestInternal::CCompFoo>();

		gameObject->Init();
		gameObject->Activate();

		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		CHandle gameObjectHandle = gameObject;
		gameObjectHandle.PostMessage(message);
		EXPECT_EQ(0, compFoo->m_foo);

		m_gameObjectManager->SendPostMsgs();

		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, child_gameObject_with_component_receives_message_with_PostMessage_recursive)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		MessagesTestInternal::CCompFoo* compFoo0 = gameObject->AddComponent<MessagesTestInternal::CCompFoo>();
		CGameObject* gameObject1 = m_gameObjectManager->CreateGameObject();
		gameObject->AddChild(gameObject1);
		MessagesTestInternal::CCompFoo* compFoo = gameObject1->AddComponent<MessagesTestInternal::CCompFoo>();

		gameObject->Init();
		gameObject->Activate();

		EXPECT_EQ(0, compFoo0->m_foo);
		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		gameObject->PostMessage(message, ESendMessageType::Recursive);
		EXPECT_EQ(0, compFoo0->m_foo);
		EXPECT_EQ(0, compFoo->m_foo);

		m_gameObjectManager->SendPostMsgs();

		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo0->m_foo);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, child_gameObject_with_component_receives_message_with_PostMessage_recursive_through_handle)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		MessagesTestInternal::CCompFoo* compFoo0 = gameObject->AddComponent<MessagesTestInternal::CCompFoo>();
		CGameObject* gameObject1 = m_gameObjectManager->CreateGameObject();
		gameObject->AddChild(gameObject1);
		MessagesTestInternal::CCompFoo* compFoo = gameObject1->AddComponent<MessagesTestInternal::CCompFoo>();

		gameObject->Init();
		gameObject->Activate();

		EXPECT_EQ(0, compFoo0->m_foo);
		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		CHandle gameObjectHandle = gameObject;
		gameObjectHandle.PostMessage(message, ESendMessageType::Recursive);
		EXPECT_EQ(0, compFoo0->m_foo);
		EXPECT_EQ(0, compFoo->m_foo);

		m_gameObjectManager->SendPostMsgs();

		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo0->m_foo);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, child_gameObject_with_component_receives_message_with_PostMessageToChildren_recursive)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		MessagesTestInternal::CCompFoo* compFoo0 = gameObject->AddComponent<MessagesTestInternal::CCompFoo>();
		CGameObject* gameObject1 = m_gameObjectManager->CreateGameObject();
		gameObject->AddChild(gameObject1);
		MessagesTestInternal::CCompFoo* compFoo = gameObject1->AddComponent<MessagesTestInternal::CCompFoo>();

		gameObject->Init();
		gameObject->Activate();

		EXPECT_EQ(0, compFoo0->m_foo);
		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		gameObject->PostMessageToChildren(message, ESendMessageType::Recursive);
		EXPECT_EQ(0, compFoo0->m_foo);
		EXPECT_EQ(0, compFoo->m_foo);

		m_gameObjectManager->SendPostMsgs();

		EXPECT_EQ(0, compFoo0->m_foo);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, child_gameObject_with_component_receives_message_with_PostMessageToChildren_recursive_through_handle)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		MessagesTestInternal::CCompFoo* compFoo0 = gameObject->AddComponent<MessagesTestInternal::CCompFoo>();
		CGameObject* gameObject1 = m_gameObjectManager->CreateGameObject();
		gameObject->AddChild(gameObject1);
		MessagesTestInternal::CCompFoo* compFoo = gameObject1->AddComponent<MessagesTestInternal::CCompFoo>();

		gameObject->Init();
		gameObject->Activate();

		EXPECT_EQ(0, compFoo0->m_foo);
		EXPECT_EQ(0, compFoo->m_foo);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		CHandle gameObjectHandle = gameObject;
		gameObjectHandle.PostMessageToChildren(message, ESendMessageType::Recursive);
		EXPECT_EQ(0, compFoo0->m_foo);
		EXPECT_EQ(0, compFoo->m_foo);

		m_gameObjectManager->SendPostMsgs();

		EXPECT_EQ(0, compFoo0->m_foo);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, child_of_child_gameObject_with_different_components_receives_message_with_PostMessage_recursive)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		CGameObject* gameObject1 = m_gameObjectManager->CreateGameObject();
		gameObject->AddChild(gameObject1);
		MessagesTestInternal::CCompFoo* compFoo = gameObject1->AddComponent<MessagesTestInternal::CCompFoo>();
		CGameObject* gameObject11 = m_gameObjectManager->CreateGameObject();
		gameObject1->AddChild(gameObject11);
		MessagesTestInternal::CCompBar* compBar = gameObject11->AddComponent<MessagesTestInternal::CCompBar>();

		gameObject->Init();
		gameObject->Activate();

		EXPECT_EQ(0, compFoo->m_foo);
		EXPECT_EQ(0, compBar->m_bar);
		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		gameObject->PostMessage(message, ESendMessageType::Recursive);
		EXPECT_EQ(0, compFoo->m_foo);
		EXPECT_EQ(0, compBar->m_bar);

		m_gameObjectManager->SendPostMsgs();

		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compFoo->m_foo);
		EXPECT_EQ(MessagesTestInternal::TEST_VALUE, compBar->m_bar);
	}

	TEST_F(CMessagesTest, send_postMessage_before_destroying_gameObject_doesnt_crash)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();

		gameObject->Init();
		gameObject->Activate();

		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		gameObject->PostMessage(message);

		gameObject->Destroy();

		m_gameObjectManager->ExecuteScheduledDestroys();
		m_gameObjectManager->SendPostMsgs();

		EXPECT_TRUE(true);
	}

	TEST_F(CMessagesTest, sendMsg_to_destroyed_gameObject_not_received)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		MessagesTestInternal::CCompFoo* compFoo = gameObject->AddComponent<MessagesTestInternal::CCompFoo>();

		gameObject->Init();
		gameObject->Activate();

		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		EXPECT_EQ(0, compFoo->m_foo);

		gameObject->Destroy();

		gameObject->SendMessage(message);

		EXPECT_EQ(0, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, sendMsg_to_destroyed_gameObject_with_child_not_received)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		CGameObject* gameObject1 = m_gameObjectManager->CreateGameObject();
		gameObject->AddChild(gameObject1);
		MessagesTestInternal::CCompFoo* compFoo = gameObject1->AddComponent<MessagesTestInternal::CCompFoo>();

		gameObject->Init();
		gameObject->Activate();

		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		EXPECT_EQ(0, compFoo->m_foo);

		gameObject->Destroy();

		gameObject->SendMessageToChildren(message);

		EXPECT_EQ(0, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, postMsg_to_destroyed_gameObject_not_received)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		MessagesTestInternal::CCompFoo* compFoo = gameObject->AddComponent<MessagesTestInternal::CCompFoo>();

		gameObject->Init();
		gameObject->Activate();

		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		EXPECT_EQ(0, compFoo->m_foo);

		gameObject->Destroy();

		gameObject->PostMessage(message);
		m_gameObjectManager->SendPostMsgs();

		EXPECT_EQ(0, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, postMsg_to_destroyed_gameObject_with_child_not_received)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		CGameObject* gameObject1 = m_gameObjectManager->CreateGameObject();
		gameObject->AddChild(gameObject1);
		MessagesTestInternal::CCompFoo* compFoo = gameObject1->AddComponent<MessagesTestInternal::CCompFoo>();

		gameObject->Init();
		gameObject->Activate();

		MessagesTestInternal::STestMessage message(MessagesTestInternal::TEST_VALUE);
		EXPECT_EQ(0, compFoo->m_foo);

		gameObject->Destroy();

		gameObject->PostMessageToChildren(message);
		m_gameObjectManager->SendPostMsgs();

		EXPECT_EQ(0, compFoo->m_foo);
	}

	TEST_F(CMessagesTest, postMsg_recursive_doesnt_crash)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		MessagesTestInternal::CCompFoo* compFoo = gameObject->AddComponent<MessagesTestInternal::CCompFoo>();

		gameObject->Init();
		gameObject->Activate();

		MessagesTestInternal::SPostMessage message;
		EXPECT_EQ(0, compFoo->m_foo);

		gameObject->PostMessage(message);

		for (int i = 1; i < MessagesTestInternal::LOOP_COUNT; ++i)
		{
			m_gameObjectManager->SendPostMsgs();
			EXPECT_EQ(i, compFoo->m_foo);
		}
	}
}
