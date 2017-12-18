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
#include <donerecs/entity/CEntityParser.h>
#include <donerecs/entity/CPrefabManager.h>
#include <donerecs/component/CComponent.h>
#include <donerecs/component/CComponentFactoryManager.h>
#include <donerecs/tags/CTagsManager.h>
#include <donerecs/handle/CHandle.h>
#include <donerecs/json/json.h>
#include <donerecs/utils/memory/CMemoryDataProvider.h>

#include <gtest/gtest.h>

namespace DonerECS
{
	namespace EntityParserTestInternal
	{
		class CCompFoo : public CComponent
		{
		public:
			CCompFoo() : m_a(-1), m_b(-1) {}
			void ParseAtts(const Json::Value& atts) override
			{
				m_a = !atts["a"].isNull() ? atts["a"].asInt() : m_a;
				m_b = !atts["b"].isNull() ? atts["b"].asInt() : m_b;
			}
			int m_a;
			int m_b;
		};

		const char* const ONE_LEVEL_ENTITY = "{ \"type\": \"scene\", \"root\": {"
			"\"type\": \"entity\", \"name\": \"test1\", \"tags\": [\"tag1\", \"tag3\"],"
			"\"components\": [{ \"name\": \"foo\", \"a\": 1, \"b\": -3 }]}}";

		const char* const ONE_LEVEL_ENTITY_INITIALLY_DISABLED = "{ \"type\": \"scene\", \"root\": {"
			"\"type\": \"entity\", \"name\": \"test1\", \"tags\": [\"tag1\", \"tag3\"], \"initiallyActive\":false,"
			"\"components\": [{ \"name\": \"foo\", \"a\": 1, \"b\": -3 }]}}";

		const char* const ONE_LEVEL_ENTITY_COMPONENT_INITIALLY_DISABLED = "{ \"type\": \"scene\", \"root\": {"
			"\"type\": \"entity\", \"name\": \"test1\", \"tags\": [\"tag1\", \"tag3\"],"
			"\"components\": [{ \"name\": \"foo\", \"initiallyActive\":false, \"a\": 1, \"b\": -3 }]}}";

		const char* const ONE_LEVEL_ENTITY_INVALID_COMPONENT = "{ \"type\": \"scene\", \"root\": {"
			"\"name\": \"test1\", \"tags\": [\"tag1\", \"tag3\"],"
			"\"components\": [{ \"name\": \"bar\", \"a\": 1, \"b\": -3 }]}}";

		const char* const ONE_LEVEL_ENTITY_INVALID_TAG = "{ \"type\": \"scene\", \"root\": {"
			"\"name\": \"test1\", \"tags\": [\"tag1\", \"tag3\", \"tag4\"],"
			"\"components\": [{ \"name\": \"foo\", \"a\": 1, \"b\": -3 }]}}";

		const char* const TWO_LEVEL_ENTITY = "{ \"type\": \"scene\", \"root\": {"
			"\"name\": \"test1\", \"tags\": [\"tag1\", \"tag3\"],"
			"\"components\": [{ \"name\": \"foo\", \"a\": 1, \"b\": -3 }],"
			"\"children\": [{ \"name\": \"test11\", \"tags\": [\"tag1\", \"tag3\"], \"components\": [{ \"name\": \"foo\", \"a\": 1, \"b\": -3 }]}]}}";

		const char* const TWO_LEVEL_ENTITY_INITIALLY_DISABLED = "{ \"type\": \"scene\", \"root\": {"
			"\"name\": \"test1\", \"tags\": [\"tag1\", \"tag3\"],"
			"\"components\": [{ \"name\": \"foo\", \"a\": 1, \"b\": -3 }],"
			"\"children\": [{ \"name\": \"test11\", \"initiallyActive\":false, \"tags\": [\"tag1\", \"tag3\"], \"components\": [{ \"name\": \"foo\", \"a\": 1, \"b\": -3 }]}]}}";

		const char* const ENTITY_BASED_ON_PREFAB = "{ \"type\": \"scene\", \"root\": {"
			"\"name\": \"test1\", \"prefab\": \"prefabTest\"}}";

		const char* const ENTITY_BASED_ON_PREFAB_MODIFYING_COMPONENT_DATA = "{ \"type\": \"scene\", \"root\": {"
			"\"name\": \"test1\", \"prefab\": \"prefabTest\","
			"\"components\": [{ \"name\": \"foo\", \"b\": 1337, \"initiallyActive\":false }]}}";

		const char* const ENTITY_BASED_ON_PREFAB_ADDING_EXTRA_TAGS = "{ \"type\": \"scene\", \"root\": {"
			"\"name\": \"test1\", \"prefab\": \"prefabTest\", \"tags\": [\"tag2\", \"tag3\"]}}";

		const char* const BASIC_PREFAB = "{ \"type\": \"prefab\", \"root\": {"
			"\"name\": \"prefabTest\", \"tags\": [\"tag1\", \"tag3\"],"
			"\"components\": [{ \"name\": \"foo\", \"a\": 1, \"b\": -3 }]}}";
	}

	class CEntityParserTest : public ::testing::Test
	{
	public:
		CEntityParserTest()
			: m_componentFactoryManager(CComponentFactoryManager::CreateInstance())
			, m_entityManager(CEntityManager::CreateInstance())
			, m_prefabManager(CPrefabManager::CreateInstance())
			, m_tagManager(CTagsManager::CreateInstance())
		{
			ADD_COMPONENT_FACTORY("foo", EntityParserTestInternal::CCompFoo, 10);

			m_tagManager->RegisterTag("tag1");
			m_tagManager->RegisterTag("tag2");
			m_tagManager->RegisterTag("tag3");
		}

		~CEntityParserTest()
		{
			CPrefabManager::DestroyInstance();
			CEntityManager::DestroyInstance();
			CTagsManager::DestroyInstance();
			CComponentFactoryManager::DestroyInstance();
		}

		CComponentFactoryManager *m_componentFactoryManager;
		CEntityManager *m_entityManager;
		CPrefabManager *m_prefabManager;
		CTagsManager *m_tagManager;
	};

	TEST_F(CEntityParserTest, parse_entity)
	{
		CEntityParser parser;
		CEntity* entity = parser.ParseSceneFromJson(EntityParserTestInternal::ONE_LEVEL_ENTITY);
		EXPECT_NE(nullptr, entity);
		EXPECT_EQ(std::string("test1"), entity->GetName());
		EXPECT_TRUE(entity->GetIsInitiallyActive());
		EXPECT_TRUE(entity->IsInitialized());
		EXPECT_TRUE(entity->IsActive());
		EXPECT_FALSE(entity->IsDestroyed());
	}

	TEST_F(CEntityParserTest, parse_entity_initiallyActive_false)
	{
		CEntityParser parser;
		CEntity* entity = parser.ParseSceneFromJson(EntityParserTestInternal::ONE_LEVEL_ENTITY_INITIALLY_DISABLED);
		EXPECT_NE(nullptr, entity);
		EXPECT_EQ(std::string("test1"), entity->GetName());
		EXPECT_FALSE(entity->GetIsInitiallyActive());
		EXPECT_TRUE(entity->IsInitialized());
		EXPECT_FALSE(entity->IsActive());
		EXPECT_FALSE(entity->IsDestroyed());
	}

	TEST_F(CEntityParserTest, parse_entity_with_component)
	{
		CEntityParser parser;
		CEntity* entity = parser.ParseSceneFromJson(EntityParserTestInternal::ONE_LEVEL_ENTITY);
		EXPECT_NE(nullptr, entity);
		EntityParserTestInternal::CCompFoo* component = entity->GetComponent<EntityParserTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);
		EXPECT_TRUE(component->GetIsInitiallyActive());
		EXPECT_TRUE(component->IsInitialized());
		EXPECT_TRUE(component->IsActive());
		EXPECT_FALSE(component->IsDestroyed());
		EXPECT_EQ(1, component->m_a);
		EXPECT_EQ(-3, component->m_b);
	}

	TEST_F(CEntityParserTest, parse_entity_with_component_initiallyActive_false)
	{
		CEntityParser parser;
		CEntity* entity = parser.ParseSceneFromJson(EntityParserTestInternal::ONE_LEVEL_ENTITY_COMPONENT_INITIALLY_DISABLED);
		EXPECT_NE(nullptr, entity);
		EntityParserTestInternal::CCompFoo* component = entity->GetComponent<EntityParserTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);
		EXPECT_FALSE(component->GetIsInitiallyActive());
		EXPECT_TRUE(component->IsInitialized());
		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component->IsDestroyed());
		EXPECT_EQ(1, component->m_a);
		EXPECT_EQ(-3, component->m_b);
	}

	TEST_F(CEntityParserTest, parse_entity_with_invalid_component)
	{
		CEntityParser parser;
		CEntity* entity = parser.ParseSceneFromJson(EntityParserTestInternal::ONE_LEVEL_ENTITY_INVALID_COMPONENT);
		EXPECT_NE(nullptr, entity);
		EntityParserTestInternal::CCompFoo* component = entity->GetComponent<EntityParserTestInternal::CCompFoo>();
		EXPECT_EQ(nullptr, component);
	}

	TEST_F(CEntityParserTest, parse_entity_with_tags)
	{
		CEntityParser parser;
		CEntity* entity = parser.ParseSceneFromJson(EntityParserTestInternal::ONE_LEVEL_ENTITY);
		EXPECT_NE(nullptr, entity);
		EXPECT_TRUE(entity->HasTags("tag1", "tag3"));
		EXPECT_FALSE(entity->HasTags("tag1", "tag2"));
	}

	TEST_F(CEntityParserTest, parse_entity_with_invalid_tag)
	{
		CEntityParser parser;
		CEntity* entity = parser.ParseSceneFromJson(EntityParserTestInternal::ONE_LEVEL_ENTITY_INVALID_TAG);
		EXPECT_NE(nullptr, entity);
		EXPECT_TRUE(entity->HasTags("tag1", "tag3"));
		EXPECT_FALSE(entity->HasTags("tag4"));
	}

	TEST_F(CEntityParserTest, parse_entity_with_child)
	{
		CEntityParser parser;
		CEntity* entity = parser.ParseSceneFromJson(EntityParserTestInternal::TWO_LEVEL_ENTITY);
		EXPECT_NE(nullptr, entity);
		EXPECT_EQ(1, entity->GetChildrenCount());
		CEntity* child1 = entity->GetChildByName("test11");
		EXPECT_NE(nullptr, child1);

		EXPECT_EQ(std::string("test11"), child1->GetName());
		EXPECT_TRUE(child1->GetIsInitiallyActive());
		EXPECT_TRUE(child1->IsInitialized());
		EXPECT_TRUE(child1->IsActive());
		EXPECT_FALSE(child1->IsDestroyed());

		EntityParserTestInternal::CCompFoo* component = child1->GetComponent<EntityParserTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);
		EXPECT_TRUE(component->GetIsInitiallyActive());
		EXPECT_TRUE(component->IsInitialized());
		EXPECT_TRUE(component->IsActive());
		EXPECT_FALSE(component->IsDestroyed());
		EXPECT_EQ(1, component->m_a);
		EXPECT_EQ(-3, component->m_b);

		EXPECT_TRUE(child1->HasTags("tag1", "tag3"));
		EXPECT_FALSE(child1->HasTags("tag1", "tag2"));
	}

	TEST_F(CEntityParserTest, parse_entity_with_child_initiallyActive_false)
	{
		CEntityParser parser;
		CEntity* entity = parser.ParseSceneFromJson(EntityParserTestInternal::TWO_LEVEL_ENTITY_INITIALLY_DISABLED);
		EXPECT_NE(nullptr, entity);
		EXPECT_EQ(1, entity->GetChildrenCount());
		CEntity* child1 = entity->GetChildByName("test11");
		EXPECT_NE(nullptr, child1);

		EXPECT_FALSE(child1->GetIsInitiallyActive());
		EXPECT_TRUE(child1->IsInitialized());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child1->IsDestroyed());

		EntityParserTestInternal::CCompFoo* component = child1->GetComponent<EntityParserTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);
		EXPECT_TRUE(component->GetIsInitiallyActive());
		EXPECT_TRUE(component->IsInitialized());
		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component->IsDestroyed());
	}

	TEST_F(CEntityParserTest, parse_entity_based_on_prefab)
	{
		CEntityParser parser;
		CEntity* prefabEntity = parser.ParseSceneFromJson(EntityParserTestInternal::ONE_LEVEL_ENTITY);
		EXPECT_NE(nullptr, prefabEntity);

		bool success = m_prefabManager->RegisterPrefab("prefabTest", prefabEntity);
		EXPECT_TRUE(success);


		CEntity* entity = parser.ParseSceneFromJson(EntityParserTestInternal::ENTITY_BASED_ON_PREFAB);
		EXPECT_NE(nullptr, entity);
		EXPECT_EQ(std::string("test1"), entity->GetName());
		EXPECT_TRUE(entity->GetIsInitiallyActive());
		EXPECT_TRUE(entity->IsInitialized());
		EXPECT_TRUE(entity->IsActive());
		EXPECT_FALSE(entity->IsDestroyed());

		EntityParserTestInternal::CCompFoo* component = entity->GetComponent<EntityParserTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);
		EXPECT_TRUE(component->GetIsInitiallyActive());
		EXPECT_TRUE(component->IsInitialized());
		EXPECT_TRUE(component->IsActive());
		EXPECT_FALSE(component->IsDestroyed());
		EXPECT_EQ(1, component->m_a);
		EXPECT_EQ(-3, component->m_b);

		EXPECT_TRUE(entity->HasTags("tag1"));
		EXPECT_FALSE(entity->HasTags("tag2"));
		EXPECT_TRUE(entity->HasTags("tag3"));
	}

	TEST_F(CEntityParserTest, parse_entity_based_on_prefab_modifying_component_data)
	{
		CEntityParser parser;
		CEntity* prefabEntity = parser.ParseSceneFromJson(EntityParserTestInternal::ONE_LEVEL_ENTITY);
		EXPECT_NE(nullptr, prefabEntity);

		bool success = m_prefabManager->RegisterPrefab("prefabTest", prefabEntity);
		EXPECT_TRUE(success);

		CEntity* entity = parser.ParseSceneFromJson(EntityParserTestInternal::ENTITY_BASED_ON_PREFAB_MODIFYING_COMPONENT_DATA);
		EXPECT_NE(nullptr, entity);

		EntityParserTestInternal::CCompFoo* component = entity->GetComponent<EntityParserTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);
		EXPECT_FALSE(component->GetIsInitiallyActive());
		EXPECT_TRUE(component->IsInitialized());
		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component->IsDestroyed());
		EXPECT_EQ(1, component->m_a);
		EXPECT_EQ(1337, component->m_b);
	}

	TEST_F(CEntityParserTest, parse_entity_based_on_prefab_with_extra_tags)
	{
		CEntityParser parser;
		CEntity* prefabEntity = parser.ParseSceneFromJson(EntityParserTestInternal::ONE_LEVEL_ENTITY);
		EXPECT_NE(nullptr, prefabEntity);

		bool success = m_prefabManager->RegisterPrefab("prefabTest", prefabEntity);
		EXPECT_TRUE(success);

		CEntity* entity = parser.ParseSceneFromJson(EntityParserTestInternal::ENTITY_BASED_ON_PREFAB_ADDING_EXTRA_TAGS);
		EXPECT_NE(nullptr, entity);

		EXPECT_TRUE(entity->HasTags("tag1"));
		EXPECT_TRUE(entity->HasTags("tag2"));
		EXPECT_TRUE(entity->HasTags("tag3"));
	}

	TEST_F(CEntityParserTest, parse_entity_based_on_parsed_prefab)
	{
		CEntityParser parser;
		CEntity* prefabEntity = parser.ParseSceneFromJson(EntityParserTestInternal::BASIC_PREFAB);
		EXPECT_NE(nullptr, prefabEntity);

		CEntity* entity = parser.ParseSceneFromJson(EntityParserTestInternal::ENTITY_BASED_ON_PREFAB);
		EXPECT_NE(nullptr, entity);
		EXPECT_EQ(std::string("test1"), entity->GetName());
		EXPECT_TRUE(entity->GetIsInitiallyActive());
		EXPECT_TRUE(entity->IsInitialized());
		EXPECT_TRUE(entity->IsActive());
		EXPECT_FALSE(entity->IsDestroyed());

		EntityParserTestInternal::CCompFoo* component = entity->GetComponent<EntityParserTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);
		EXPECT_TRUE(component->GetIsInitiallyActive());
		EXPECT_TRUE(component->IsInitialized());
		EXPECT_TRUE(component->IsActive());
		EXPECT_FALSE(component->IsDestroyed());
		EXPECT_EQ(1, component->m_a);
		EXPECT_EQ(-3, component->m_b);

		EXPECT_TRUE(entity->HasTags("tag1"));
		EXPECT_FALSE(entity->HasTags("tag2"));
		EXPECT_TRUE(entity->HasTags("tag3"));
	}

	TEST_F(CEntityParserTest, parse_entity_from_memory_buffer)
	{
		std::string entityStr(EntityParserTestInternal::ONE_LEVEL_ENTITY);
		CMemoryDataProvider mdp((unsigned char*)entityStr.c_str(), entityStr.size());

		CEntityParser parser;
		CEntity* entity = parser.ParseSceneFromMemory(mdp.GetBaseData(), mdp.GetSize());

		EXPECT_NE(nullptr, entity);
		EXPECT_EQ(std::string("test1"), entity->GetName());
		EXPECT_TRUE(entity->GetIsInitiallyActive());
		EXPECT_TRUE(entity->IsInitialized());
		EXPECT_TRUE(entity->IsActive());
		EXPECT_FALSE(entity->IsDestroyed());

		EntityParserTestInternal::CCompFoo* component = entity->GetComponent<EntityParserTestInternal::CCompFoo>();
		EXPECT_NE(nullptr, component);
		EXPECT_TRUE(component->GetIsInitiallyActive());
		EXPECT_TRUE(component->IsInitialized());
		EXPECT_TRUE(component->IsActive());
		EXPECT_FALSE(component->IsDestroyed());
		EXPECT_EQ(1, component->m_a);
		EXPECT_EQ(-3, component->m_b);

		EXPECT_TRUE(entity->HasTags("tag1"));
		EXPECT_FALSE(entity->HasTags("tag2"));
		EXPECT_TRUE(entity->HasTags("tag3"));
	}
}
