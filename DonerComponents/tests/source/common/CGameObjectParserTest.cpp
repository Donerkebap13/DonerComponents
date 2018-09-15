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
#include <donercomponents/gameObject/CGameObjectParser.h>
#include <donercomponents/gameObject/CPrefabManager.h>
#include <donercomponents/component/CComponent.h>
#include <donercomponents/component/CComponentFactoryManager.h>
#include <donercomponents/tags/CTagsManager.h>
#include <donercomponents/handle/CHandle.h>
#include <donercomponents/utils/memory/CMemoryDataProvider.h>

#include <donerserializer/DonerDeserialize.h>

#include <gtest/gtest.h>

namespace GameObjectParserTestInternal
{
	class CCompFoo : public DonerComponents::CComponent
	{
		DC_DECLARE_COMPONENT_AS_SERIALIZABLE(CCompFoo)
	public:
		CCompFoo() : m_a(-1), m_b(-1) {}

		int m_a;
		int m_b;

	};

	const char* const ONE_LEVEL_GAME_OBJECT = "{ \"root\": {"
		"\"name\": \"test1\", \"tags\": [\"tag1\", \"tag3\"],"
		"\"components\": [{ \"name\": \"foo\", \"a\": 1, \"b\": -3 }]}}";

	const char* const ONE_LEVEL_GAME_OBJECT_INITIALLY_DISABLED = "{ \"root\": {"
		"\"name\": \"test1\", \"tags\": [\"tag1\", \"tag3\"], \"initially_active\":false,"
		"\"components\": [{ \"name\": \"foo\", \"a\": 1, \"b\": -3 }]}}";

	const char* const ONE_LEVEL_GAME_OBJECT_COMPONENT_INITIALLY_DISABLED = "{ \"root\": {"
		"\"name\": \"test1\", \"tags\": [\"tag1\", \"tag3\"],"
		"\"components\": [{ \"name\": \"foo\", \"initially_active\":false, \"a\": 1, \"b\": -3 }]}}";

	const char* const ONE_LEVEL_GAME_OBJECT_INVALID_COMPONENT = "{ \"root\": {"
		"\"name\": \"test1\", \"tags\": [\"tag1\", \"tag3\"],"
		"\"components\": [{ \"name\": \"bar\", \"a\": 1, \"b\": -3 }]}}";

	const char* const ONE_LEVEL_GAME_OBJECT_INVALID_TAG = "{ \"root\": {"
		"\"name\": \"test1\", \"tags\": [\"tag1\", \"tag3\", \"tag4\"],"
		"\"components\": [{ \"name\": \"foo\", \"a\": 1, \"b\": -3 }]}}";

	const char* const TWO_LEVEL_GAME_OBJECT = "{ \"root\": {"
		"\"name\": \"test1\", \"tags\": [\"tag1\", \"tag3\"],"
		"\"components\": [{ \"name\": \"foo\", \"a\": 1, \"b\": -3 }],"
		"\"children\": [{ \"name\": \"test11\", \"tags\": [\"tag1\", \"tag3\"], \"components\": [{ \"name\": \"foo\", \"a\": 1, \"b\": -3 }]}]}}";

	const char* const TWO_LEVEL_GAME_OBJECT_INITIALLY_DISABLED = "{ \"root\": {"
		"\"name\": \"test1\", \"tags\": [\"tag1\", \"tag3\"],"
		"\"components\": [{ \"name\": \"foo\", \"a\": 1, \"b\": -3 }],"
		"\"children\": [{ \"name\": \"test11\", \"initially_active\":false, \"tags\": [\"tag1\", \"tag3\"], \"components\": [{ \"name\": \"foo\", \"a\": 1, \"b\": -3 }]}]}}";

	const char* const GAME_OBJECT_BASED_ON_PREFAB = "{ \"root\": {"
		"\"name\": \"test1\", \"prefab\": \"prefabTest\"}}";

	const char* const GAME_OBJECT_BASED_ON_PREFAB_MODIFYING_COMPONENT_DATA = "{ \"root\": {"
		"\"name\": \"test1\", \"prefab\": \"prefabTest\","
		"\"components\": [{ \"name\": \"foo\", \"b\": 1337, \"initially_active\":false }]}}";

	const char* const GAME_OBJECT_BASED_ON_PREFAB_ADDING_EXTRA_TAGS = "{ \"root\": {"
		"\"name\": \"test1\", \"prefab\": \"prefabTest\", \"tags\": [\"tag2\", \"tag3\"]}}";

	const char* const GAME_OBJECT_BASED_ON_TWO_LEVEL_PREFAB = "{ \"root\": {"
		"\"name\": \"test1\", \"prefab\": \"2levelPrefab\","
		"\"children\": [{ \"name\": \"test11\", \"components\": [{ \"name\": \"foo\", \"a\": 1337 }]}]}}";

	const char* const BASIC_PREFAB = "{ \"root\": {"
		"\"name\": \"prefabTest\", \"tags\": [\"tag1\", \"tag3\"],"
		"\"components\": [{ \"name\": \"foo\", \"a\": 1, \"b\": -3 }]}}";

	const char* const TWO_LEVEL_PREFAB = "{ \"root\": {"
		"\"name\": \"2levelPrefab\", \"tags\": [\"tag1\", \"tag3\"],"
		"\"children\": [{ \"name\": \"test11\", \"components\": [{ \"name\": \"foo\", \"a\": 1, \"b\": -3 }]}]}}";
}

DONER_DEFINE_REFLECTION_DATA(GameObjectParserTestInternal::CCompFoo,
							DONER_ADD_NAMED_VAR_INFO(m_a, "a"),
							DONER_ADD_NAMED_VAR_INFO(m_b, "b")
)

DC_SERIALIZABLE_COMPONENT_IMPL(::GameObjectParserTestInternal::CCompFoo)

namespace DonerComponents
{
	class CGameObjectParserTest : public ::testing::Test
	{
	public:
		CGameObjectParserTest()
			: m_componentFactoryManager(nullptr)
			, m_gameObjectManager(nullptr)
			, m_prefabManager(nullptr)
			, m_tagManager(nullptr)
		{
			CDonerComponentsSystems& systems = CDonerComponentsSystems::CreateInstance()->Init();
			m_componentFactoryManager = systems.GetComponentFactoryManager();
			m_gameObjectManager = systems.GetGameObjectManager();
			m_prefabManager = systems.GetPrefabManager();
			m_tagManager = systems.GetTagsManager();

			ADD_COMPONENT_FACTORY("foo", ::GameObjectParserTestInternal::CCompFoo, 10);

			m_tagManager->RegisterTag("tag1");
			m_tagManager->RegisterTag("tag2");
			m_tagManager->RegisterTag("tag3");
		}

		~CGameObjectParserTest()
		{
			CDonerComponentsSystems::DestroyInstance();
		}

		CComponentFactoryManager *m_componentFactoryManager;
		CGameObjectManager *m_gameObjectManager;
		CPrefabManager *m_prefabManager;
		CTagsManager *m_tagManager;
	};

	TEST_F(CGameObjectParserTest, parse_gameObject)
	{
		CGameObjectParser parser;
		CGameObject* gameObject = parser.ParseSceneFromJson(::GameObjectParserTestInternal::ONE_LEVEL_GAME_OBJECT);
		EXPECT_NE(nullptr, gameObject);
		EXPECT_EQ(std::string("test1"), gameObject->GetName());
		EXPECT_TRUE(gameObject->GetIsInitiallyActive());
		EXPECT_TRUE(gameObject->IsInitialized());
		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_FALSE(gameObject->IsDestroyed());
	}

	TEST_F(CGameObjectParserTest, parse_gameObject_initiallyActive_false)
	{
		CGameObjectParser parser;
		CGameObject* gameObject = parser.ParseSceneFromJson(::GameObjectParserTestInternal::ONE_LEVEL_GAME_OBJECT_INITIALLY_DISABLED);
		EXPECT_EQ(std::string("test1"), gameObject->GetName());
		EXPECT_FALSE(gameObject->GetIsInitiallyActive());
		EXPECT_TRUE(gameObject->IsInitialized());
		EXPECT_FALSE(gameObject->IsActive());
		EXPECT_FALSE(gameObject->IsDestroyed());
	}

	TEST_F(CGameObjectParserTest, parse_gameObject_with_component)
	{
		CGameObjectParser parser;
		CGameObject* gameObject = parser.ParseSceneFromJson(::GameObjectParserTestInternal::ONE_LEVEL_GAME_OBJECT);
		::GameObjectParserTestInternal::CCompFoo* component = gameObject->GetComponent<::GameObjectParserTestInternal::CCompFoo>();
		EXPECT_TRUE(component->GetIsInitiallyActive());
		EXPECT_TRUE(component->IsInitialized());
		EXPECT_TRUE(component->IsActive());
		EXPECT_FALSE(component->IsDestroyed());
		EXPECT_EQ(1, component->m_a);
		EXPECT_EQ(-3, component->m_b);
	}

	TEST_F(CGameObjectParserTest, parse_gameObject_with_component_initiallyActive_false)
	{
		CGameObjectParser parser;
		CGameObject* gameObject = parser.ParseSceneFromJson(::GameObjectParserTestInternal::ONE_LEVEL_GAME_OBJECT_COMPONENT_INITIALLY_DISABLED);
		::GameObjectParserTestInternal::CCompFoo* component = gameObject->GetComponent<::GameObjectParserTestInternal::CCompFoo>();
		EXPECT_FALSE(component->GetIsInitiallyActive());
		EXPECT_TRUE(component->IsInitialized());
		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component->IsDestroyed());
		EXPECT_EQ(1, component->m_a);
		EXPECT_EQ(-3, component->m_b);
	}

	TEST_F(CGameObjectParserTest, parse_gameObject_with_invalid_component)
	{
		CGameObjectParser parser;
		CGameObject* gameObject = parser.ParseSceneFromJson(::GameObjectParserTestInternal::ONE_LEVEL_GAME_OBJECT_INVALID_COMPONENT);
		::GameObjectParserTestInternal::CCompFoo* component = gameObject->GetComponent<::GameObjectParserTestInternal::CCompFoo>();
		EXPECT_EQ(nullptr, component);
	}

	TEST_F(CGameObjectParserTest, parse_gameObject_with_tags)
	{
		CGameObjectParser parser;
		CGameObject* gameObject = parser.ParseSceneFromJson(::GameObjectParserTestInternal::ONE_LEVEL_GAME_OBJECT);
		EXPECT_TRUE(gameObject->HasTags("tag1", "tag3"));
		EXPECT_FALSE(gameObject->HasTags("tag1", "tag2"));
	}

	TEST_F(CGameObjectParserTest, parse_gameObject_with_invalid_tag)
	{
		CGameObjectParser parser;
		CGameObject* gameObject = parser.ParseSceneFromJson(::GameObjectParserTestInternal::ONE_LEVEL_GAME_OBJECT_INVALID_TAG);
		EXPECT_TRUE(gameObject->HasTags("tag1", "tag3"));
		EXPECT_FALSE(gameObject->HasTags("tag4"));
	}

	TEST_F(CGameObjectParserTest, parse_gameObject_with_child)
	{
		CGameObjectParser parser;
		CGameObject* gameObject = parser.ParseSceneFromJson(::GameObjectParserTestInternal::TWO_LEVEL_GAME_OBJECT);
		EXPECT_EQ(1, gameObject->GetChildrenCount());
		CGameObject* child1 = gameObject->GetChildByName("test11");

		EXPECT_EQ(std::string("test11"), child1->GetName());
		EXPECT_TRUE(child1->GetIsInitiallyActive());
		EXPECT_TRUE(child1->IsInitialized());
		EXPECT_TRUE(child1->IsActive());
		EXPECT_FALSE(child1->IsDestroyed());

		::GameObjectParserTestInternal::CCompFoo* component = child1->GetComponent<::GameObjectParserTestInternal::CCompFoo>();
		EXPECT_TRUE(component->GetIsInitiallyActive());
		EXPECT_TRUE(component->IsInitialized());
		EXPECT_TRUE(component->IsActive());
		EXPECT_FALSE(component->IsDestroyed());
		EXPECT_EQ(1, component->m_a);
		EXPECT_EQ(-3, component->m_b);

		EXPECT_TRUE(child1->HasTags("tag1", "tag3"));
		EXPECT_FALSE(child1->HasTags("tag1", "tag2"));
	}

	TEST_F(CGameObjectParserTest, parse_gameObject_with_child_initiallyActive_false)
	{
		CGameObjectParser parser;
		CGameObject* gameObject = parser.ParseSceneFromJson(::GameObjectParserTestInternal::TWO_LEVEL_GAME_OBJECT_INITIALLY_DISABLED);
		EXPECT_EQ(1, gameObject->GetChildrenCount());
		CGameObject* child1 = gameObject->GetChildByName("test11");

		EXPECT_FALSE(child1->GetIsInitiallyActive());
		EXPECT_TRUE(child1->IsInitialized());
		EXPECT_FALSE(child1->IsActive());
		EXPECT_FALSE(child1->IsDestroyed());

		::GameObjectParserTestInternal::CCompFoo* component = child1->GetComponent<::GameObjectParserTestInternal::CCompFoo>();
		EXPECT_TRUE(component->GetIsInitiallyActive());
		EXPECT_TRUE(component->IsInitialized());
		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component->IsDestroyed());
	}

	TEST_F(CGameObjectParserTest, parse_gameObject_based_on_prefab)
	{
		CGameObjectParser parser;
		parser.ParsePrefabFromJson(::GameObjectParserTestInternal::BASIC_PREFAB);

		CGameObject* gameObject = parser.ParseSceneFromJson(::GameObjectParserTestInternal::GAME_OBJECT_BASED_ON_PREFAB);
		EXPECT_EQ(std::string("test1"), gameObject->GetName());
		EXPECT_TRUE(gameObject->GetIsInitiallyActive());
		EXPECT_TRUE(gameObject->IsInitialized());
		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_FALSE(gameObject->IsDestroyed());

		::GameObjectParserTestInternal::CCompFoo* component = gameObject->GetComponent<::GameObjectParserTestInternal::CCompFoo>();
		EXPECT_TRUE(component->GetIsInitiallyActive());
		EXPECT_TRUE(component->IsInitialized());
		EXPECT_TRUE(component->IsActive());
		EXPECT_FALSE(component->IsDestroyed());
		EXPECT_EQ(1, component->m_a);
		EXPECT_EQ(-3, component->m_b);

		EXPECT_TRUE(gameObject->HasTags("tag1"));
		EXPECT_FALSE(gameObject->HasTags("tag2"));
		EXPECT_TRUE(gameObject->HasTags("tag3"));
	}

	TEST_F(CGameObjectParserTest, parse_gameObject_based_on_prefab_modifying_component_data)
	{
		CGameObjectParser parser;
		parser.ParsePrefabFromJson(::GameObjectParserTestInternal::BASIC_PREFAB);

		CGameObject* gameObject = parser.ParseSceneFromJson(::GameObjectParserTestInternal::GAME_OBJECT_BASED_ON_PREFAB_MODIFYING_COMPONENT_DATA);

		::GameObjectParserTestInternal::CCompFoo* component = gameObject->GetComponent<::GameObjectParserTestInternal::CCompFoo>();
		EXPECT_FALSE(component->GetIsInitiallyActive());
		EXPECT_TRUE(component->IsInitialized());
		EXPECT_FALSE(component->IsActive());
		EXPECT_FALSE(component->IsDestroyed());
		EXPECT_EQ(1, component->m_a);
		EXPECT_EQ(1337, component->m_b);
	}

	TEST_F(CGameObjectParserTest, parse_gameObject_based_on_prefab_with_extra_tags)
	{
		CGameObjectParser parser;
		parser.ParsePrefabFromJson(::GameObjectParserTestInternal::BASIC_PREFAB);

		CGameObject* gameObject = parser.ParseSceneFromJson(::GameObjectParserTestInternal::GAME_OBJECT_BASED_ON_PREFAB_ADDING_EXTRA_TAGS);

		EXPECT_TRUE(gameObject->HasTags("tag1"));
		EXPECT_TRUE(gameObject->HasTags("tag2"));
		EXPECT_TRUE(gameObject->HasTags("tag3"));
	}

	TEST_F(CGameObjectParserTest, parse_gameObject_based_on_parsed_prefab)
	{
		CGameObjectParser parser;
		parser.ParsePrefabFromJson(::GameObjectParserTestInternal::BASIC_PREFAB);

		CGameObject* gameObject = parser.ParseSceneFromJson(::GameObjectParserTestInternal::GAME_OBJECT_BASED_ON_PREFAB);
		EXPECT_EQ(std::string("test1"), gameObject->GetName());
		EXPECT_TRUE(gameObject->GetIsInitiallyActive());
		EXPECT_TRUE(gameObject->IsInitialized());
		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_FALSE(gameObject->IsDestroyed());

		::GameObjectParserTestInternal::CCompFoo* component = gameObject->GetComponent<::GameObjectParserTestInternal::CCompFoo>();
		EXPECT_TRUE(component->GetIsInitiallyActive());
		EXPECT_TRUE(component->IsInitialized());
		EXPECT_TRUE(component->IsActive());
		EXPECT_FALSE(component->IsDestroyed());
		EXPECT_EQ(1, component->m_a);
		EXPECT_EQ(-3, component->m_b);

		EXPECT_TRUE(gameObject->HasTags("tag1"));
		EXPECT_FALSE(gameObject->HasTags("tag2"));
		EXPECT_TRUE(gameObject->HasTags("tag3"));
	}

	TEST_F(CGameObjectParserTest, parse_gameObject_from_memory_buffer)
	{
		std::string gameObjectStr(::GameObjectParserTestInternal::ONE_LEVEL_GAME_OBJECT);
		CMemoryDataProvider mdp((unsigned char*)gameObjectStr.c_str(), gameObjectStr.size());

		CGameObjectParser parser;
		CGameObject* gameObject = parser.ParseSceneFromMemory(mdp.GetBaseData(), mdp.GetSize());

		EXPECT_EQ(std::string("test1"), gameObject->GetName());
		EXPECT_TRUE(gameObject->GetIsInitiallyActive());
		EXPECT_TRUE(gameObject->IsInitialized());
		EXPECT_TRUE(gameObject->IsActive());
		EXPECT_FALSE(gameObject->IsDestroyed());

		::GameObjectParserTestInternal::CCompFoo* component = gameObject->GetComponent<::GameObjectParserTestInternal::CCompFoo>();
		EXPECT_TRUE(component->GetIsInitiallyActive());
		EXPECT_TRUE(component->IsInitialized());
		EXPECT_TRUE(component->IsActive());
		EXPECT_FALSE(component->IsDestroyed());
		EXPECT_EQ(1, component->m_a);
		EXPECT_EQ(-3, component->m_b);

		EXPECT_TRUE(gameObject->HasTags("tag1"));
		EXPECT_FALSE(gameObject->HasTags("tag2"));
		EXPECT_TRUE(gameObject->HasTags("tag3"));
	}

	TEST_F(CGameObjectParserTest, parse_gameObject_based_on_prefab_modifying_prefab_children_info)
	{
		CGameObjectParser parser;
		parser.ParsePrefabFromJson(::GameObjectParserTestInternal::TWO_LEVEL_PREFAB);

		CGameObject* gameObject = parser.ParseSceneFromJson(::GameObjectParserTestInternal::GAME_OBJECT_BASED_ON_TWO_LEVEL_PREFAB);
		CGameObject* gameObject1 = gameObject->GetChildByName("test11");
		::GameObjectParserTestInternal::CCompFoo* component = gameObject1->GetComponent<::GameObjectParserTestInternal::CCompFoo>();

		EXPECT_EQ(1337, component->m_a);
	}
}
