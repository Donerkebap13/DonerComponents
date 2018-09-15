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
#include <donerecs/gameObject/CGameObject.h>
#include <donerecs/gameObject/CGameObjectParser.h>
#include <donerecs/gameObject/CPrefabManager.h>
#include <donerecs/component/CComponent.h>
#include <donerecs/component/CComponentFactoryManager.h>
#include <donerecs/tags/CTagsManager.h>
#include <donerecs/handle/CHandle.h>
#include <donerecs/json/json.h>
#include <donerecs/utils/memory/CMemoryDataProvider.h>

#include <donerserializer/DonerDeserialize.h>

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace DeserializationTestInternal
{
	const std::int32_t INT_DEFAULT_VALUE = 0;
	const float FLOAT_DEFAULT_VALUE = 0.f;
	const bool BOOL_DEFAULT_VALUE = false;
	const std::int64_t LONG_LONG_DEFAULT_VALUE = 0L;
	const double DOUBLE_DEFAULT_VALUE = 0.0;
	const std::string STRING_DEFAULT_VALUE = "default";

	class CCompFoo : public DonerECS::CComponent
	{
		DECS_DECLARE_COMPONENT_AS_SERIALIZABLE(CCompFoo)
	public:
		CCompFoo()
			: m_int(INT_DEFAULT_VALUE)
			, m_float(FLOAT_DEFAULT_VALUE)
			, m_bool(BOOL_DEFAULT_VALUE)
			, m_longLong(LONG_LONG_DEFAULT_VALUE)
			, m_double(DOUBLE_DEFAULT_VALUE)
			, m_string(STRING_DEFAULT_VALUE)
		{}

		std::int32_t m_int;
		float m_float;
		bool m_bool;
		std::int64_t m_longLong;
		double m_double;
		std::string m_string;

		std::vector<int> m_intVector;
		std::vector<std::string> m_stringVector;
	};

		const char* const FULL_DATA_GAME_OBJECT = "{ \"root\": {"
		"\"name\": \"test1\","
		"\"components\": [{ \"name\": \"foo\", \"int\": 1, \"float\": 16.0, \"bool\": true, \"long_long\": 99999, \"double\": 10.0, \"string\": \"std::string\","
		" \"int_vector\": [1, 20, 300], \"string_vector\": [\"one\", \"two\", \"three\"] }]}}";

	const char* const PARTIALLY_FULL_GAME_OBJECT = "{ \"root\": {"
		"\"name\": \"test1\","
		"\"components\": [{ \"name\": \"foo\", \"bool\": true, \"string\": \"std::string\","
		" \"string_vector\": [\"one\", \"two\", \"three\"] }]}}";

	const char* const EMPTY_GAME_OBJECT = "{ \"root\": {"
		"\"name\": \"test1\","
		"\"components\": [{ \"name\": \"foo\"}]}}";
}

DONER_DEFINE_REFLECTION_DATA(::DeserializationTestInternal::CCompFoo,
							DONER_ADD_NAMED_VAR_INFO(m_int, "int"),
							DONER_ADD_NAMED_VAR_INFO(m_float, "float"),
							DONER_ADD_NAMED_VAR_INFO(m_bool, "bool"),
							DONER_ADD_NAMED_VAR_INFO(m_longLong, "long_long"),
							DONER_ADD_NAMED_VAR_INFO(m_double, "double"),
							DONER_ADD_NAMED_VAR_INFO(m_string, "string"),
							DONER_ADD_NAMED_VAR_INFO(m_intVector, "int_vector"),
							DONER_ADD_NAMED_VAR_INFO(m_stringVector, "string_vector")
)

DECS_SERIALIZABLE_COMPONENT_IMPL(::DeserializationTestInternal::CCompFoo)

namespace DonerECS
{
	class CDeserializationTest : public ::testing::Test
	{
	public:
		CDeserializationTest()
			: m_componentFactoryManager(nullptr)
			, m_gameObjectManager(nullptr)
			, m_prefabManager(nullptr)
			, m_tagManager(nullptr)
		{
			CDonerECSSystems& systems = CDonerECSSystems::CreateInstance()->Init();
			m_componentFactoryManager = systems.GetComponentFactoryManager();
			m_gameObjectManager = systems.GetGameObjectManager();
			m_prefabManager = systems.GetPrefabManager();
			m_tagManager = systems.GetTagsManager();

			ADD_COMPONENT_FACTORY("foo", ::DeserializationTestInternal::CCompFoo, 10);

			m_tagManager->RegisterTag("tag1");
			m_tagManager->RegisterTag("tag2");
			m_tagManager->RegisterTag("tag3");
		}

		~CDeserializationTest()
		{
			CDonerECSSystems::DestroyInstance();
		}

		CComponentFactoryManager *m_componentFactoryManager;
		CGameObjectManager *m_gameObjectManager;
		CPrefabManager *m_prefabManager;
		CTagsManager *m_tagManager;
	};

	TEST_F(CDeserializationTest, parse_gameObject_with_full_data)
	{
		CGameObjectParser parser;
		CGameObject* gameObject = parser.ParseSceneFromJson(::DeserializationTestInternal::FULL_DATA_GAME_OBJECT);
		::DeserializationTestInternal::CCompFoo* component = gameObject->GetComponent<::DeserializationTestInternal::CCompFoo>();
		
		EXPECT_EQ(1, component->m_int);
		EXPECT_EQ(16.f, component->m_float);
		EXPECT_EQ(true, component->m_bool);
		EXPECT_EQ(99999L, component->m_longLong);
		EXPECT_EQ(10.0, component->m_double);
		EXPECT_EQ("std::string", component->m_string);

		// TODO_TESTS improve size check
		EXPECT_EQ(3, component->m_intVector.size());
		EXPECT_EQ(1, component->m_intVector[0]);
		EXPECT_EQ(20, component->m_intVector[1]);
		EXPECT_EQ(300, component->m_intVector[2]);

		// TODO_TESTS improve size check
		EXPECT_EQ(3, component->m_stringVector.size());
		EXPECT_EQ("one", component->m_stringVector[0]);
		EXPECT_EQ("two", component->m_stringVector[1]);
		EXPECT_EQ("three", component->m_stringVector[2]);
	}

	TEST_F(CDeserializationTest, parse_gameObject_with_partial_data)
	{
		CGameObjectParser parser;
		CGameObject* gameObject = parser.ParseSceneFromJson(::DeserializationTestInternal::PARTIALLY_FULL_GAME_OBJECT);
		::DeserializationTestInternal::CCompFoo* component = gameObject->GetComponent<::DeserializationTestInternal::CCompFoo>();
		
		EXPECT_EQ(::DeserializationTestInternal::INT_DEFAULT_VALUE, component->m_int);
		EXPECT_EQ(::DeserializationTestInternal::FLOAT_DEFAULT_VALUE, component->m_float);
		EXPECT_EQ(true, component->m_bool);
		EXPECT_EQ(::DeserializationTestInternal::LONG_LONG_DEFAULT_VALUE, component->m_longLong);
		EXPECT_EQ(::DeserializationTestInternal::DOUBLE_DEFAULT_VALUE, component->m_double);
		EXPECT_EQ("std::string", component->m_string);

		EXPECT_EQ(0, component->m_intVector.size());

		EXPECT_EQ(3, component->m_stringVector.size());
		EXPECT_EQ("one", component->m_stringVector[0]);
		EXPECT_EQ("two", component->m_stringVector[1]);
		EXPECT_EQ("three", component->m_stringVector[2]);
	}

	TEST_F(CDeserializationTest, parse_gameObject_with_no_data)
	{
		CGameObjectParser parser;
		CGameObject* gameObject = parser.ParseSceneFromJson(::DeserializationTestInternal::EMPTY_GAME_OBJECT);
		::DeserializationTestInternal::CCompFoo* component = gameObject->GetComponent<::DeserializationTestInternal::CCompFoo>();
		
		EXPECT_EQ(::DeserializationTestInternal::INT_DEFAULT_VALUE, component->m_int);
		EXPECT_EQ(::DeserializationTestInternal::FLOAT_DEFAULT_VALUE, component->m_float);
		EXPECT_EQ(::DeserializationTestInternal::BOOL_DEFAULT_VALUE, component->m_bool);
		EXPECT_EQ(::DeserializationTestInternal::LONG_LONG_DEFAULT_VALUE, component->m_longLong);
		EXPECT_EQ(::DeserializationTestInternal::DOUBLE_DEFAULT_VALUE, component->m_double);
		EXPECT_EQ(::DeserializationTestInternal::STRING_DEFAULT_VALUE, component->m_string);

		EXPECT_EQ(0, component->m_intVector.size());
		EXPECT_EQ(0, component->m_stringVector.size());
	}
}
