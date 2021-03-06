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
#include <donercomponents/tags/CTagsManager.h>

#include <gtest/gtest.h>

namespace DonerComponents
{
	namespace TagsTestInternal
	{
		const char* const TAGS_JSON_FILE = "{ \"tags\": [\"test\", \"test2\"] }";
	}

	class CTagsTest : public ::testing::Test
	{
	public:
		CTagsTest()
			: m_tagManager(nullptr)
		{
			m_tagManager = CDonerComponentsSystems::CreateInstance()->Init().GetTagsManager();
		}

		~CTagsTest()
		{
			CDonerComponentsSystems::DestroyInstance();
		}

		CTagsManager *m_tagManager;
	};

	TEST_F(CTagsTest, register_new_tag)
	{
		bool success = m_tagManager->RegisterTag("test");
		EXPECT_TRUE(success);
	}

	TEST_F(CTagsTest, register_existing_tag_fails)
	{
		// gmock possible usage instead of
		m_tagManager->RegisterTag("test");
		bool success = m_tagManager->RegisterTag("test");
		EXPECT_FALSE(success);
	}

	TEST_F(CTagsTest, register_tag_correct_value)
	{
		m_tagManager->RegisterTag("test");
		m_tagManager->RegisterTag("test2");
		
		int tagId1 = m_tagManager->GetTagIdx("test");
		EXPECT_EQ(0, tagId1);
		int tagId2 = m_tagManager->GetTagIdx("test2");
		EXPECT_EQ(1, tagId2);
	}

	TEST_F(CTagsTest, add_tag)
	{
		m_tagManager->RegisterTag("test");

		TagsMask mask;
		bool success = m_tagManager->SetTag(mask, "test", true);
		EXPECT_TRUE(success);
		success = m_tagManager->SetTag(mask, "test2", true);
		EXPECT_FALSE(success);

		success = m_tagManager->HasTag(mask, "test");
		EXPECT_TRUE(success);
		success = m_tagManager->HasTag(mask, "test2");
		EXPECT_FALSE(success);
	}

	TEST_F(CTagsTest, has_tag)
	{
		m_tagManager->RegisterTag("test");

		TagsMask mask;
		m_tagManager->SetTag(mask, "test", true);

		bool success = m_tagManager->HasTag(mask, "test");
		EXPECT_TRUE(success);
	}

	TEST_F(CTagsTest, add_invalid_tag_fails)
	{
		m_tagManager->RegisterTag("test");

		TagsMask mask;
		bool success = m_tagManager->SetTag(mask, "test2", true);
		EXPECT_FALSE(success);
	}

	TEST_F(CTagsTest, remove_tag)
	{
		m_tagManager->RegisterTag("test");

		TagsMask mask;
		m_tagManager->SetTag(mask, "test", true);
		bool success = m_tagManager->HasTag(mask, "test");
		EXPECT_TRUE(success);

		m_tagManager->SetTag(mask, "test", false);
		success = m_tagManager->HasTag(mask, "test");
		EXPECT_FALSE(success);
	}

	TEST_F(CTagsTest, add_tags_in_bunch)
	{
		m_tagManager->RegisterTag("test");
		m_tagManager->RegisterTag("test2");

		TagsMask mask;
		bool success = m_tagManager->AddTags(mask, "test", "test2");
		EXPECT_TRUE(success);

		success = m_tagManager->HasTag(mask, "test");
		EXPECT_TRUE(success);
		success = m_tagManager->HasTag(mask, "test2");
		EXPECT_TRUE(success);
	}

	TEST_F(CTagsTest, add_tags_in_bunch_with_incorrect_one)
	{
		m_tagManager->RegisterTag("test");

		TagsMask mask;
		bool success = m_tagManager->AddTags(mask, "test3", "test", "test2");
		EXPECT_FALSE(success);

		success = m_tagManager->HasTag(mask, "test");
		EXPECT_TRUE(success);
		success = m_tagManager->HasTag(mask, "test2");
		EXPECT_FALSE(success);
	}

	TEST_F(CTagsTest, remove_tags_in_bunch)
	{
		m_tagManager->RegisterTag("test");
		m_tagManager->RegisterTag("test2");

		TagsMask mask;
		m_tagManager->AddTags(mask, "test", "test2");

		bool success = m_tagManager->HasTag(mask, "test");
		EXPECT_TRUE(success);
		success = m_tagManager->HasTag(mask, "test2");
		EXPECT_TRUE(success);

		success = m_tagManager->RemoveTags(mask, "test", "test2");
		EXPECT_TRUE(success);

		success = m_tagManager->HasTag(mask, "test");
		EXPECT_FALSE(success);
		success = m_tagManager->HasTag(mask, "test2");
		EXPECT_FALSE(success);
	}

	TEST_F(CTagsTest, remove_tags_in_bunch_with_incorrect_one)
	{
		m_tagManager->RegisterTag("test");

		TagsMask mask;
		m_tagManager->AddTags(mask, "test");

		bool success = m_tagManager->HasTag(mask, "test");
		EXPECT_TRUE(success);

		success = m_tagManager->RemoveTags(mask, "test3", "test", "test2");
		EXPECT_FALSE(success);

		success = m_tagManager->HasTag(mask, "test");
		EXPECT_FALSE(success);
	}

	TEST_F(CTagsTest, generate_tag_mask)
	{
		m_tagManager->RegisterTag("test");
		m_tagManager->RegisterTag("test2");

		int tagId1 = m_tagManager->GetTagIdx("test");
		int tagId2 = m_tagManager->GetTagIdx("test2");

		TagsMask mask;
		mask[tagId1] = true;
		TagsMask newMask = m_tagManager->GetTagMask("test");
		EXPECT_EQ(mask, newMask);
		newMask = m_tagManager->GetTagMask("test", "test2");
		EXPECT_NE(mask, newMask);
		mask[tagId2] = true;
		EXPECT_EQ(mask, newMask);
	}

	TEST_F(CTagsTest, check_nonexistent_tag_behavior)
	{
		m_tagManager->RegisterTag("test");

		int tagId1 = m_tagManager->GetTagIdx("test");
		int tagId2 = m_tagManager->GetTagIdx("test2");
		EXPECT_EQ(-1, tagId2);

		TagsMask mask;
		mask[tagId1].flip();
		EXPECT_TRUE(m_tagManager->HasTag(mask, "test"));
		EXPECT_FALSE(m_tagManager->HasTag(mask, "test2"));
	}

	TEST_F(CTagsTest, generate_mask_with_nonexistent_tag)
	{
		m_tagManager->RegisterTag("test");

		TagsMask mask = m_tagManager->GetTagMask("test", "test2");
		EXPECT_TRUE(m_tagManager->HasTag(mask, "test"));
		EXPECT_FALSE(m_tagManager->HasTag(mask, "test2"));
	}

	TEST_F(CTagsTest, has_all_tags)
	{
		m_tagManager->RegisterTag("test");
		m_tagManager->RegisterTag("test2");

		int tagId1 = m_tagManager->GetTagIdx("test");
		int tagId2 = m_tagManager->GetTagIdx("test2");

		TagsMask mask;
		mask[tagId1] = true;
		bool success = m_tagManager->HasAllTags(mask, "test");
		EXPECT_TRUE(success);
		success = m_tagManager->HasAllTags(mask, "test", "test2");
		EXPECT_FALSE(success);
		mask[tagId2] = true;
		success = m_tagManager->HasAllTags(mask, "test");
		EXPECT_TRUE(success);
		success = m_tagManager->HasAllTags(mask, "test", "test2");
		EXPECT_TRUE(success);
	}

	TEST_F(CTagsTest, check_has_any_tags)
	{
		m_tagManager->RegisterTag("test");
		m_tagManager->RegisterTag("test2");

		int tagId1 = m_tagManager->GetTagIdx("test");
		int tagId2 = m_tagManager->GetTagIdx("test2");

		TagsMask mask;
		mask[tagId1] = true;
		bool success = m_tagManager->HasAnyTags(mask, "test");
		EXPECT_TRUE(success);
		success = m_tagManager->HasAnyTags(mask, "test", "test2");
		EXPECT_TRUE(success);
		TagsMask mask2;
		mask2[tagId2] = true;
		success = m_tagManager->HasAnyTags(mask2, "test");
		EXPECT_FALSE(success);
		success = m_tagManager->HasAnyTags(mask2, "test", "test2");
		EXPECT_TRUE(success);
	}

	TEST_F(CTagsTest, parse_tags_from_json)
	{
		bool success = m_tagManager->ParseTagsFromJson(TagsTestInternal::TAGS_JSON_FILE);
		EXPECT_TRUE(success);

		int tagId1 = m_tagManager->GetTagIdx("test");
		int tagId2 = m_tagManager->GetTagIdx("test2");

		TagsMask mask;
		mask[tagId1] = true;
		success = m_tagManager->HasAnyTags(mask, "test");
		EXPECT_TRUE(success);
		success = m_tagManager->HasAnyTags(mask, "test", "test2");
		EXPECT_TRUE(success);
		TagsMask mask2;
		mask2[tagId2] = true;
		success = m_tagManager->HasAnyTags(mask2, "test");
		EXPECT_FALSE(success);
		success = m_tagManager->HasAnyTags(mask2, "test", "test2");
		EXPECT_TRUE(success);
	}
}
