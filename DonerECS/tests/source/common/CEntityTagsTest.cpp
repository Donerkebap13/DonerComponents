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
#include <donerecs/component/CComponentFactoryManager.h>
#include <donerecs/tags/CTagsManager.h>
#include <donerecs/handle/CHandle.h>

#include <gtest/gtest.h>

namespace DonerECS
{
	namespace EntityTagsTestInternal
	{
		const CStrID TAG1("test");
		const CStrID TAG2("test2");
		const CStrID TAG3("test3");
		const CStrID INVALID_TAG("invalid");
	}

	class CEntityTagsTest : public ::testing::Test
	{
	public:
		CEntityTagsTest()
			: m_componentFactoryManager(CComponentFactoryManager::CreateInstance())
			, m_entityManager(CEntityManager::CreateInstance())
			, m_TagsManager(CTagsManager::CreateInstance())
		{
			m_TagsManager->RegisterTag(EntityTagsTestInternal::TAG1);
			m_TagsManager->RegisterTag(EntityTagsTestInternal::TAG2);
			m_TagsManager->RegisterTag(EntityTagsTestInternal::TAG3);
		}

		~CEntityTagsTest()
		{
			CEntityManager::DestroyInstance();
			CTagsManager::DestroyInstance();
			CComponentFactoryManager::DestroyInstance();
		}

		CComponentFactoryManager *m_componentFactoryManager;
		CEntityManager *m_entityManager;
		CTagsManager *m_TagsManager;
	};

	TEST_F(CEntityTagsTest, add_single_tag)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		bool success = entity->AddTags(EntityTagsTestInternal::TAG1);
		EXPECT_TRUE(success);

		success = entity->HasTags(EntityTagsTestInternal::TAG1);
		EXPECT_TRUE(success);
	}

	TEST_F(CEntityTagsTest, add_multiple_tags)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		bool success = entity->AddTags(EntityTagsTestInternal::TAG1, EntityTagsTestInternal::TAG2);
		EXPECT_TRUE(success);

		success = entity->HasTags(EntityTagsTestInternal::TAG1);
		EXPECT_TRUE(success);
		success = entity->HasTags(EntityTagsTestInternal::TAG2);
		EXPECT_TRUE(success);
	}

	TEST_F(CEntityTagsTest, remove_single_tag)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		bool success = entity->AddTags(EntityTagsTestInternal::TAG1);
		EXPECT_TRUE(success);

		success = entity->HasTags(EntityTagsTestInternal::TAG1);
		EXPECT_TRUE(success);

		success = entity->RemoveTags(EntityTagsTestInternal::TAG1);
		EXPECT_TRUE(success);

		success = entity->HasTags(EntityTagsTestInternal::TAG1);
		EXPECT_FALSE(success);
	}

	TEST_F(CEntityTagsTest, remove_multiple_tags)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		bool success = entity->AddTags(EntityTagsTestInternal::TAG1, EntityTagsTestInternal::TAG2);
		EXPECT_TRUE(success);

		success = entity->HasTags(EntityTagsTestInternal::TAG1);
		EXPECT_TRUE(success);
		success = entity->HasTags(EntityTagsTestInternal::TAG2);
		EXPECT_TRUE(success);

		success = entity->RemoveTags(EntityTagsTestInternal::TAG1, EntityTagsTestInternal::TAG2);
		EXPECT_TRUE(success);

		success = entity->HasTags(EntityTagsTestInternal::TAG1);
		EXPECT_FALSE(success);
		success = entity->HasTags(EntityTagsTestInternal::TAG2);
		EXPECT_FALSE(success);
	}

	TEST_F(CEntityTagsTest, remove_single_tag_dont_remove_other_tags)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		bool success = entity->AddTags(EntityTagsTestInternal::TAG1, EntityTagsTestInternal::TAG2);
		EXPECT_TRUE(success);

		success = entity->HasTags(EntityTagsTestInternal::TAG1);
		EXPECT_TRUE(success);
		success = entity->HasTags(EntityTagsTestInternal::TAG2);
		EXPECT_TRUE(success);

		success = entity->RemoveTags(EntityTagsTestInternal::TAG1);
		EXPECT_TRUE(success);

		success = entity->HasTags(EntityTagsTestInternal::TAG1);
		EXPECT_FALSE(success);
		success = entity->HasTags(EntityTagsTestInternal::TAG2);
		EXPECT_TRUE(success);
	}

	TEST_F(CEntityTagsTest, add_nonexistent_tag_fails)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		bool success = entity->AddTags(EntityTagsTestInternal::INVALID_TAG);
		EXPECT_FALSE(success);

		success = entity->HasTags(EntityTagsTestInternal::INVALID_TAG);
		EXPECT_FALSE(success);
	}

	TEST_F(CEntityTagsTest, add_nonexistent_tag_dont_prevent_to_add_valid_tags)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		bool success = entity->AddTags(EntityTagsTestInternal::INVALID_TAG, EntityTagsTestInternal::TAG1);
		EXPECT_FALSE(success);

		success = entity->HasTags(EntityTagsTestInternal::INVALID_TAG);
		EXPECT_FALSE(success);
		success = entity->HasTags(EntityTagsTestInternal::TAG1);
		EXPECT_TRUE(success);
	}

	TEST_F(CEntityTagsTest, remove_nonexistent_tag_fails)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		bool success = entity->RemoveTags(EntityTagsTestInternal::INVALID_TAG);
		EXPECT_FALSE(success);
	}

	TEST_F(CEntityTagsTest, remove_nonexistent_tag_dont_prevent_to_remove_valid_tags)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		bool success = entity->AddTags(EntityTagsTestInternal::TAG1);
		EXPECT_TRUE(success);

		success = entity->HasTags(EntityTagsTestInternal::TAG1);
		EXPECT_TRUE(success);

		success = entity->RemoveTags(EntityTagsTestInternal::INVALID_TAG, EntityTagsTestInternal::TAG1);
		EXPECT_FALSE(success);

		success = entity->HasTags(EntityTagsTestInternal::TAG1);
		EXPECT_FALSE(success);
	}

	TEST_F(CEntityTagsTest, entity_has_all_tags)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		bool success = entity->AddTags(EntityTagsTestInternal::TAG1, EntityTagsTestInternal::TAG2);
		EXPECT_TRUE(success);

		success = entity->HasTags(EntityTagsTestInternal::TAG1, EntityTagsTestInternal::TAG2);
		EXPECT_TRUE(success);
		success = entity->HasTags(EntityTagsTestInternal::TAG1, EntityTagsTestInternal::TAG2, EntityTagsTestInternal::TAG3);
		EXPECT_FALSE(success);
		success = entity->HasTags(EntityTagsTestInternal::TAG1, EntityTagsTestInternal::TAG2, EntityTagsTestInternal::INVALID_TAG);
		EXPECT_FALSE(success);
	}

	TEST_F(CEntityTagsTest, entity_has_any_tag)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		bool success = entity->AddTags(EntityTagsTestInternal::TAG1, EntityTagsTestInternal::TAG2);
		EXPECT_TRUE(success);

		success = entity->HasAnyTag(EntityTagsTestInternal::TAG1, EntityTagsTestInternal::TAG2);
		EXPECT_TRUE(success);
		success = entity->HasAnyTag(EntityTagsTestInternal::TAG1, EntityTagsTestInternal::TAG3);
		EXPECT_TRUE(success);
		success = entity->HasAnyTag(EntityTagsTestInternal::TAG2, EntityTagsTestInternal::INVALID_TAG);
		EXPECT_TRUE(success);
		success = entity->HasAnyTag(EntityTagsTestInternal::TAG3);
		EXPECT_FALSE(success);
	}

	TEST_F(CEntityTagsTest, get_entity_children_with_all_tags)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CEntity* child1 = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, child1);
		CEntity* child2 = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, child2);
		CEntity* child11 = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, child11);

		child1->AddTags(EntityTagsTestInternal::TAG1, EntityTagsTestInternal::TAG2);
		child2->AddTags(EntityTagsTestInternal::TAG1);
		child11->AddTags(EntityTagsTestInternal::TAG2, EntityTagsTestInternal::TAG3);

		entity->AddChild(child1);
		entity->AddChild(child2);
		EXPECT_EQ(2, entity->GetChildrenCount());
		child1->AddChild(child11);
		EXPECT_EQ(1, child1->GetChildrenCount());

		std::vector<CHandle> childrenWithTags;

		childrenWithTags = entity->GetChildrenWithTags(EntityTagsTestInternal::TAG1);
		EXPECT_EQ(2, childrenWithTags.size());
		bool child1Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child1)) != childrenWithTags.end();
		EXPECT_TRUE(child1Found);
		bool child2Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child2)) != childrenWithTags.end();
		EXPECT_TRUE(child2Found);

		childrenWithTags.clear();
		childrenWithTags = entity->GetChildrenWithTags(EntityTagsTestInternal::TAG1, EntityTagsTestInternal::TAG2);
		EXPECT_EQ(1, childrenWithTags.size());
		child1Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child1)) != childrenWithTags.end();
		EXPECT_TRUE(child1Found);
		child2Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child2)) != childrenWithTags.end();
		EXPECT_FALSE(child2Found);

		childrenWithTags.clear();
		childrenWithTags = entity->GetChildrenWithTags(EntityTagsTestInternal::TAG3);
		EXPECT_EQ(0, childrenWithTags.size());
	}

	TEST_F(CEntityTagsTest, get_entity_children_with_any_tag)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CEntity* child1 = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, child1);
		CEntity* child2 = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, child2);
		CEntity* child11 = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, child11);

		child1->AddTags(EntityTagsTestInternal::TAG1, EntityTagsTestInternal::TAG2);
		child2->AddTags(EntityTagsTestInternal::TAG1);
		child11->AddTags(EntityTagsTestInternal::TAG2, EntityTagsTestInternal::TAG3);

		entity->AddChild(child1);
		entity->AddChild(child2);
		EXPECT_EQ(2, entity->GetChildrenCount());
		child1->AddChild(child11);
		EXPECT_EQ(1, child1->GetChildrenCount());

		std::vector<CHandle> childrenWithTags;

		childrenWithTags = entity->GetChildrenWithAnyTag(EntityTagsTestInternal::TAG1);
		EXPECT_EQ(2, childrenWithTags.size());
		bool child1Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child1)) != childrenWithTags.end();
		EXPECT_TRUE(child1Found);
		bool child2Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child2)) != childrenWithTags.end();
		EXPECT_TRUE(child2Found);

		childrenWithTags.clear();
		childrenWithTags = entity->GetChildrenWithAnyTag(EntityTagsTestInternal::TAG1, EntityTagsTestInternal::TAG2);
		EXPECT_EQ(2, childrenWithTags.size());
		child1Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child1)) != childrenWithTags.end();
		EXPECT_TRUE(child1Found);
		child2Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child2)) != childrenWithTags.end();
		EXPECT_TRUE(child2Found);

		childrenWithTags.clear();
		childrenWithTags = entity->GetChildrenWithAnyTag(EntityTagsTestInternal::TAG2);
		EXPECT_EQ(1, childrenWithTags.size());
		child1Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child1)) != childrenWithTags.end();
		EXPECT_TRUE(child1Found);
		child2Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child2)) != childrenWithTags.end();
		EXPECT_FALSE(child2Found);

		childrenWithTags.clear();
		childrenWithTags = entity->GetChildrenWithAnyTag(EntityTagsTestInternal::TAG3);
		EXPECT_EQ(0, childrenWithTags.size());
	}

	TEST_F(CEntityTagsTest, get_entity_children_with_all_tags_recursive)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CEntity* child1 = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, child1);
		CEntity* child2 = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, child2);
		CEntity* child11 = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, child11);

		child1->AddTags(EntityTagsTestInternal::TAG1, EntityTagsTestInternal::TAG2);
		child2->AddTags(EntityTagsTestInternal::TAG1);
		child11->AddTags(EntityTagsTestInternal::TAG2, EntityTagsTestInternal::TAG3);

		entity->AddChild(child1);
		entity->AddChild(child2);
		EXPECT_EQ(2, entity->GetChildrenCount());
		child1->AddChild(child11);
		EXPECT_EQ(1, child1->GetChildrenCount());

		std::vector<CHandle> childrenWithTags;

		childrenWithTags = entity->GetChildrenWithTagsRecursive(EntityTagsTestInternal::TAG2);
		EXPECT_EQ(2, childrenWithTags.size());
		bool child1Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child1)) != childrenWithTags.end();
		EXPECT_TRUE(child1Found);
		bool child2Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child2)) != childrenWithTags.end();
		EXPECT_FALSE(child2Found);
		bool child11Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child11)) != childrenWithTags.end();
		EXPECT_TRUE(child11Found);

		childrenWithTags.clear();
		childrenWithTags = entity->GetChildrenWithTagsRecursive(EntityTagsTestInternal::TAG2, EntityTagsTestInternal::TAG3);
		EXPECT_EQ(1, childrenWithTags.size());
		child1Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child1)) != childrenWithTags.end();
		EXPECT_FALSE(child1Found);
		child2Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child2)) != childrenWithTags.end();
		EXPECT_FALSE(child2Found);
		child11Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child11)) != childrenWithTags.end();
		EXPECT_TRUE(child11Found);
	}

	TEST_F(CEntityTagsTest, get_entity_children_with_any_tag_recursive)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CEntity* child1 = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, child1);
		CEntity* child2 = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, child2);
		CEntity* child11 = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, child11);

		child1->AddTags(EntityTagsTestInternal::TAG1, EntityTagsTestInternal::TAG2);
		child2->AddTags(EntityTagsTestInternal::TAG1);
		child11->AddTags(EntityTagsTestInternal::TAG2, EntityTagsTestInternal::TAG3);

		entity->AddChild(child1);
		entity->AddChild(child2);
		EXPECT_EQ(2, entity->GetChildrenCount());
		child1->AddChild(child11);
		EXPECT_EQ(1, child1->GetChildrenCount());

		std::vector<CHandle> childrenWithTags;

		childrenWithTags = entity->GetChildrenWithAnyTagRecursive(EntityTagsTestInternal::TAG2);
		EXPECT_EQ(2, childrenWithTags.size());
		bool child1Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child1)) != childrenWithTags.end();
		EXPECT_TRUE(child1Found);
		bool child2Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child2)) != childrenWithTags.end();
		EXPECT_FALSE(child2Found);
		bool child11Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child11)) != childrenWithTags.end();
		EXPECT_TRUE(child11Found);

		childrenWithTags.clear();
		childrenWithTags = entity->GetChildrenWithAnyTagRecursive(EntityTagsTestInternal::TAG1, EntityTagsTestInternal::TAG3);
		EXPECT_EQ(3, childrenWithTags.size());
		child1Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child1)) != childrenWithTags.end();
		EXPECT_TRUE(child1Found);
		child2Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child2)) != childrenWithTags.end();
		EXPECT_TRUE(child2Found);
		child11Found = std::find(childrenWithTags.begin(), childrenWithTags.end(), CHandle(child11)) != childrenWithTags.end();
		EXPECT_TRUE(child11Found);
	}

	TEST_F(CEntityTagsTest, entity_clone_tags)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		bool success = entity->AddTags(EntityTagsTestInternal::TAG1, EntityTagsTestInternal::TAG2);
		EXPECT_TRUE(success);

		success = entity->HasTags(EntityTagsTestInternal::TAG1);
		EXPECT_TRUE(success);
		success = entity->HasTags(EntityTagsTestInternal::TAG2);
		EXPECT_TRUE(success);

		CEntity* clone = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, clone);
		success = clone->HasTags(EntityTagsTestInternal::TAG1);
		EXPECT_FALSE(success);
		success = clone->HasTags(EntityTagsTestInternal::TAG2);
		EXPECT_FALSE(success);

		clone->CloneFrom(entity);
		success = clone->HasTags(EntityTagsTestInternal::TAG1);
		EXPECT_TRUE(success);
		success = clone->HasTags(EntityTagsTestInternal::TAG2);
		EXPECT_TRUE(success);
	}
}