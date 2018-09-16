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
#include <donercomponents/tags/CTagsManager.h>
#include <donercomponents/handle/CHandle.h>

#include <gtest/gtest.h>

#include <algorithm>

namespace DonerComponents
{
	namespace GameObjectTagsTestInternal
	{
		const CStrID TAG1("test");
		const CStrID TAG2("test2");
		const CStrID TAG3("test3");
		const CStrID INVALID_TAG("invalid");
	}

	class CGameObjectTagsTest : public ::testing::Test
	{
	public:
		CGameObjectTagsTest()
			: m_componentFactoryManager(nullptr)
			, m_gameObjectManager(nullptr)
			, m_TagsManager(nullptr)
		{
			CDonerComponentsSystems& systems = CDonerComponentsSystems::CreateInstance()->Init();
			m_componentFactoryManager = systems.GetComponentFactoryManager();
			m_gameObjectManager = systems.GetGameObjectManager();
			m_TagsManager = systems.GetTagsManager();

			m_TagsManager->RegisterTag(GameObjectTagsTestInternal::TAG1);
			m_TagsManager->RegisterTag(GameObjectTagsTestInternal::TAG2);
			m_TagsManager->RegisterTag(GameObjectTagsTestInternal::TAG3);
		}

		~CGameObjectTagsTest()
		{
			CDonerComponentsSystems::DestroyInstance();
		}

		CComponentFactoryManager *m_componentFactoryManager;
		CGameObjectManager *m_gameObjectManager;
		CTagsManager *m_TagsManager;
	};

	TEST_F(CGameObjectTagsTest, add_single_tag)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		bool success = gameObject->AddTags(GameObjectTagsTestInternal::TAG1);
		EXPECT_TRUE(success);

		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG1);
		EXPECT_TRUE(success);
	}

	TEST_F(CGameObjectTagsTest, add_multiple_tags)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		bool success = gameObject->AddTags(GameObjectTagsTestInternal::TAG1, GameObjectTagsTestInternal::TAG2);
		EXPECT_TRUE(success);

		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG1);
		EXPECT_TRUE(success);
		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG2);
		EXPECT_TRUE(success);
	}

	TEST_F(CGameObjectTagsTest, remove_single_tag)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		bool success = gameObject->AddTags(GameObjectTagsTestInternal::TAG1);
		EXPECT_TRUE(success);

		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG1);
		EXPECT_TRUE(success);

		success = gameObject->RemoveTags(GameObjectTagsTestInternal::TAG1);
		EXPECT_TRUE(success);

		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG1);
		EXPECT_FALSE(success);
	}

	TEST_F(CGameObjectTagsTest, remove_multiple_tags)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		bool success = gameObject->AddTags(GameObjectTagsTestInternal::TAG1, GameObjectTagsTestInternal::TAG2);
		EXPECT_TRUE(success);

		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG1);
		EXPECT_TRUE(success);
		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG2);
		EXPECT_TRUE(success);

		success = gameObject->RemoveTags(GameObjectTagsTestInternal::TAG1, GameObjectTagsTestInternal::TAG2);
		EXPECT_TRUE(success);

		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG1);
		EXPECT_FALSE(success);
		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG2);
		EXPECT_FALSE(success);
	}

	TEST_F(CGameObjectTagsTest, remove_single_tag_dont_remove_other_tags)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		bool success = gameObject->AddTags(GameObjectTagsTestInternal::TAG1, GameObjectTagsTestInternal::TAG2);
		EXPECT_TRUE(success);

		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG1);
		EXPECT_TRUE(success);
		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG2);
		EXPECT_TRUE(success);

		success = gameObject->RemoveTags(GameObjectTagsTestInternal::TAG1);
		EXPECT_TRUE(success);

		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG1);
		EXPECT_FALSE(success);
		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG2);
		EXPECT_TRUE(success);
	}

	TEST_F(CGameObjectTagsTest, add_nonexistent_tag_fails)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		bool success = gameObject->AddTags(GameObjectTagsTestInternal::INVALID_TAG);
		EXPECT_FALSE(success);

		success = gameObject->HasTags(GameObjectTagsTestInternal::INVALID_TAG);
		EXPECT_FALSE(success);
	}

	TEST_F(CGameObjectTagsTest, add_nonexistent_tag_dont_prevent_to_add_valid_tags)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		bool success = gameObject->AddTags(GameObjectTagsTestInternal::INVALID_TAG, GameObjectTagsTestInternal::TAG1);
		EXPECT_FALSE(success);

		success = gameObject->HasTags(GameObjectTagsTestInternal::INVALID_TAG);
		EXPECT_FALSE(success);
		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG1);
		EXPECT_TRUE(success);
	}

	TEST_F(CGameObjectTagsTest, remove_nonexistent_tag_fails)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		bool success = gameObject->RemoveTags(GameObjectTagsTestInternal::INVALID_TAG);
		EXPECT_FALSE(success);
	}

	TEST_F(CGameObjectTagsTest, remove_nonexistent_tag_dont_prevent_to_remove_valid_tags)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		bool success = gameObject->AddTags(GameObjectTagsTestInternal::TAG1);
		EXPECT_TRUE(success);

		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG1);
		EXPECT_TRUE(success);

		success = gameObject->RemoveTags(GameObjectTagsTestInternal::INVALID_TAG, GameObjectTagsTestInternal::TAG1);
		EXPECT_FALSE(success);

		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG1);
		EXPECT_FALSE(success);
	}

	TEST_F(CGameObjectTagsTest, gameObject_has_all_tags)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		bool success = gameObject->AddTags(GameObjectTagsTestInternal::TAG1, GameObjectTagsTestInternal::TAG2);
		EXPECT_TRUE(success);

		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG1, GameObjectTagsTestInternal::TAG2);
		EXPECT_TRUE(success);
		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG1, GameObjectTagsTestInternal::TAG2, GameObjectTagsTestInternal::TAG3);
		EXPECT_FALSE(success);
		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG1, GameObjectTagsTestInternal::TAG2, GameObjectTagsTestInternal::INVALID_TAG);
		EXPECT_FALSE(success);
	}

	TEST_F(CGameObjectTagsTest, gameObject_has_any_tag)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		bool success = gameObject->AddTags(GameObjectTagsTestInternal::TAG1, GameObjectTagsTestInternal::TAG2);
		EXPECT_TRUE(success);

		success = gameObject->HasAnyTag(GameObjectTagsTestInternal::TAG1, GameObjectTagsTestInternal::TAG2);
		EXPECT_TRUE(success);
		success = gameObject->HasAnyTag(GameObjectTagsTestInternal::TAG1, GameObjectTagsTestInternal::TAG3);
		EXPECT_TRUE(success);
		success = gameObject->HasAnyTag(GameObjectTagsTestInternal::TAG2, GameObjectTagsTestInternal::INVALID_TAG);
		EXPECT_TRUE(success);
		success = gameObject->HasAnyTag(GameObjectTagsTestInternal::TAG3);
		EXPECT_FALSE(success);
	}

	TEST_F(CGameObjectTagsTest, get_gameObject_children_with_all_tags)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		CGameObject* child1 = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, child1);
		CGameObject* child2 = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, child2);
		CGameObject* child11 = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, child11);

		child1->AddTags(GameObjectTagsTestInternal::TAG1, GameObjectTagsTestInternal::TAG2);
		child2->AddTags(GameObjectTagsTestInternal::TAG1);
		child11->AddTags(GameObjectTagsTestInternal::TAG2, GameObjectTagsTestInternal::TAG3);

		gameObject->AddChild(child1);
		gameObject->AddChild(child2);
		EXPECT_EQ(2, gameObject->GetChildrenCount());
		child1->AddChild(child11);
		EXPECT_EQ(1, child1->GetChildrenCount());

		std::vector<CHandle> childrenWithTags;

		childrenWithTags = gameObject->GetChildrenWithTags(GameObjectTagsTestInternal::TAG1);
		EXPECT_EQ(2, childrenWithTags.size());
		bool child1Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child1)) != childrenWithTags.end();
		EXPECT_TRUE(child1Found);
		bool child2Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child2)) != childrenWithTags.end();
		EXPECT_TRUE(child2Found);

		childrenWithTags.clear();
		childrenWithTags = gameObject->GetChildrenWithTags(GameObjectTagsTestInternal::TAG1, GameObjectTagsTestInternal::TAG2);
		EXPECT_EQ(1, childrenWithTags.size());
		child1Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child1)) != childrenWithTags.end();
		EXPECT_TRUE(child1Found);
		child2Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child2)) != childrenWithTags.end();
		EXPECT_FALSE(child2Found);

		childrenWithTags.clear();
		childrenWithTags = gameObject->GetChildrenWithTags(GameObjectTagsTestInternal::TAG3);
		EXPECT_EQ(0, childrenWithTags.size());
	}

	TEST_F(CGameObjectTagsTest, get_gameObject_children_with_any_tag)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		CGameObject* child1 = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, child1);
		CGameObject* child2 = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, child2);
		CGameObject* child11 = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, child11);

		child1->AddTags(GameObjectTagsTestInternal::TAG1, GameObjectTagsTestInternal::TAG2);
		child2->AddTags(GameObjectTagsTestInternal::TAG1);
		child11->AddTags(GameObjectTagsTestInternal::TAG2, GameObjectTagsTestInternal::TAG3);

		gameObject->AddChild(child1);
		gameObject->AddChild(child2);
		EXPECT_EQ(2, gameObject->GetChildrenCount());
		child1->AddChild(child11);
		EXPECT_EQ(1, child1->GetChildrenCount());

		std::vector<CHandle> childrenWithTags;

		childrenWithTags = gameObject->GetChildrenWithAnyTag(GameObjectTagsTestInternal::TAG1);
		EXPECT_EQ(2, childrenWithTags.size());
		bool child1Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child1)) != childrenWithTags.end();
		EXPECT_TRUE(child1Found);
		bool child2Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child2)) != childrenWithTags.end();
		EXPECT_TRUE(child2Found);

		childrenWithTags.clear();
		childrenWithTags = gameObject->GetChildrenWithAnyTag(GameObjectTagsTestInternal::TAG1, GameObjectTagsTestInternal::TAG2);
		EXPECT_EQ(2, childrenWithTags.size());
		child1Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child1)) != childrenWithTags.end();
		EXPECT_TRUE(child1Found);
		child2Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child2)) != childrenWithTags.end();
		EXPECT_TRUE(child2Found);

		childrenWithTags.clear();
		childrenWithTags = gameObject->GetChildrenWithAnyTag(GameObjectTagsTestInternal::TAG2);
		EXPECT_EQ(1, childrenWithTags.size());
		child1Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child1)) != childrenWithTags.end();
		EXPECT_TRUE(child1Found);
		child2Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child2)) != childrenWithTags.end();
		EXPECT_FALSE(child2Found);

		childrenWithTags.clear();
		childrenWithTags = gameObject->GetChildrenWithAnyTag(GameObjectTagsTestInternal::TAG3);
		EXPECT_EQ(0, childrenWithTags.size());
	}

	TEST_F(CGameObjectTagsTest, get_gameObject_children_with_all_tags_recursive)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		CGameObject* child1 = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, child1);
		CGameObject* child2 = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, child2);
		CGameObject* child11 = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, child11);

		child1->AddTags(GameObjectTagsTestInternal::TAG1, GameObjectTagsTestInternal::TAG2);
		child2->AddTags(GameObjectTagsTestInternal::TAG1);
		child11->AddTags(GameObjectTagsTestInternal::TAG2, GameObjectTagsTestInternal::TAG3);

		gameObject->AddChild(child1);
		gameObject->AddChild(child2);
		EXPECT_EQ(2, gameObject->GetChildrenCount());
		child1->AddChild(child11);
		EXPECT_EQ(1, child1->GetChildrenCount());

		std::vector<CHandle> childrenWithTags;

		childrenWithTags = gameObject->GetChildrenWithTagsRecursive(GameObjectTagsTestInternal::TAG2);
		EXPECT_EQ(2, childrenWithTags.size());
		bool child1Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child1)) != childrenWithTags.end();
		EXPECT_TRUE(child1Found);
		bool child2Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child2)) != childrenWithTags.end();
		EXPECT_FALSE(child2Found);
		bool child11Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child11)) != childrenWithTags.end();
		EXPECT_TRUE(child11Found);

		childrenWithTags.clear();
		childrenWithTags = gameObject->GetChildrenWithTagsRecursive(GameObjectTagsTestInternal::TAG2, GameObjectTagsTestInternal::TAG3);
		EXPECT_EQ(1, childrenWithTags.size());
		child1Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child1)) != childrenWithTags.end();
		EXPECT_FALSE(child1Found);
		child2Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child2)) != childrenWithTags.end();
		EXPECT_FALSE(child2Found);
		child11Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child11)) != childrenWithTags.end();
		EXPECT_TRUE(child11Found);
	}

	TEST_F(CGameObjectTagsTest, get_gameObject_children_with_any_tag_recursive)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		CGameObject* child1 = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, child1);
		CGameObject* child2 = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, child2);
		CGameObject* child11 = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, child11);

		child1->AddTags(GameObjectTagsTestInternal::TAG1, GameObjectTagsTestInternal::TAG2);
		child2->AddTags(GameObjectTagsTestInternal::TAG1);
		child11->AddTags(GameObjectTagsTestInternal::TAG2, GameObjectTagsTestInternal::TAG3);

		gameObject->AddChild(child1);
		gameObject->AddChild(child2);
		EXPECT_EQ(2, gameObject->GetChildrenCount());
		child1->AddChild(child11);
		EXPECT_EQ(1, child1->GetChildrenCount());

		std::vector<CHandle> childrenWithTags;

		childrenWithTags = gameObject->GetChildrenWithAnyTagRecursive(GameObjectTagsTestInternal::TAG2);
		EXPECT_EQ(2, childrenWithTags.size());
		bool child1Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child1)) != childrenWithTags.end();
		EXPECT_TRUE(child1Found);
		bool child2Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child2)) != childrenWithTags.end();
		EXPECT_FALSE(child2Found);
		bool child11Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child11)) != childrenWithTags.end();
		EXPECT_TRUE(child11Found);

		childrenWithTags.clear();
		childrenWithTags = gameObject->GetChildrenWithAnyTagRecursive(GameObjectTagsTestInternal::TAG1, GameObjectTagsTestInternal::TAG3);
		EXPECT_EQ(3, childrenWithTags.size());
		child1Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child1)) != childrenWithTags.end();
		EXPECT_TRUE(child1Found);
		child2Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child2)) != childrenWithTags.end();
		EXPECT_TRUE(child2Found);
		child11Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child11)) != childrenWithTags.end();
		EXPECT_TRUE(child11Found);
	}

	TEST_F(CGameObjectTagsTest, gameObject_clone_tags)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
		bool success = gameObject->AddTags(GameObjectTagsTestInternal::TAG1, GameObjectTagsTestInternal::TAG2);
		EXPECT_TRUE(success);

		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG1);
		EXPECT_TRUE(success);
		success = gameObject->HasTags(GameObjectTagsTestInternal::TAG2);
		EXPECT_TRUE(success);

		CGameObject* clone = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, clone);
		success = clone->HasTags(GameObjectTagsTestInternal::TAG1);
		EXPECT_FALSE(success);
		success = clone->HasTags(GameObjectTagsTestInternal::TAG2);
		EXPECT_FALSE(success);

		clone->CloneFrom(gameObject);
		success = clone->HasTags(GameObjectTagsTestInternal::TAG1);
		EXPECT_TRUE(success);
		success = clone->HasTags(GameObjectTagsTestInternal::TAG2);
		EXPECT_TRUE(success);
	}
}