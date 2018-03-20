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

#include <donerecs/CDonerECSSystems.h>
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

#include <string>
#include <vector>

namespace ReflectionTestInternal
{
	const int INT_DEFAULT_VALUE = 0;
	const float FLOAT_DEFAULT_VALUE = 0.f;
	const bool BOOL_DEFAULT_VALUE = false;
	const long long LONG_LONG_DEFAULT_VALUE = 0L;
	const double DOUBLE_DEFAULT_VALUE = 0.0;
	const std::string STRING_DEFAULT_VALUE = "default";

	class CCompFoo : public DonerECS::CComponent
	{
		DECS_DECLARE_COMPONENT_AS_REFLECTABLE(CCompFoo)
	public:
		CCompFoo()
			: m_int(INT_DEFAULT_VALUE)
			, m_float(FLOAT_DEFAULT_VALUE)
			, m_bool(BOOL_DEFAULT_VALUE)
			, m_longLong(LONG_LONG_DEFAULT_VALUE)
			, m_double(DOUBLE_DEFAULT_VALUE)
			, m_string(STRING_DEFAULT_VALUE)
		{}

		int m_int;
		float m_float;
		bool m_bool;
		long long m_longLong;
		double m_double;
		std::string m_string;

		std::vector<int> m_intVector;
		std::vector<std::string> m_stringVector;
	};

	DECS_COMPONENT_REFLECTION_IMPL(ReflectionTestInternal::CCompFoo)

		const char* const FULL_DATA_ENTITY = "{ \"type\": \"scene\", \"root\": {"
		"\"type\": \"entity\", \"name\": \"test1\","
		"\"components\": [{ \"name\": \"foo\", \"int\": 1, \"float\": 16.0, \"bool\": true, \"long_long\": 99999, \"double\": 10.0, \"string\": \"std::string\","
		" \"int_vector\": [1, 20, 300], \"string_vector\": [\"one\", \"two\", \"three\"] }]}}";

	const char* const PARTIALLY_FULL_ENTITY = "{ \"type\": \"scene\", \"root\": {"
		"\"type\": \"entity\", \"name\": \"test1\","
		"\"components\": [{ \"name\": \"foo\", \"bool\": true, \"string\": \"std::string\","
		" \"string_vector\": [\"one\", \"two\", \"three\"] }]}}";

	const char* const EMPTY_ENTITY = "{ \"type\": \"scene\", \"root\": {"
		"\"type\": \"entity\", \"name\": \"test1\","
		"\"components\": [{ \"name\": \"foo\"}]}}";
}

DECS_DEFINE_REFLECTION_DATA(::ReflectionTestInternal::CCompFoo,
							DECS_ADD_VAR_INFO(m_int, "int"),
							DECS_ADD_VAR_INFO(m_float, "float"),
							DECS_ADD_VAR_INFO(m_bool, "bool"),
							DECS_ADD_VAR_INFO(m_longLong, "long_long"),
							DECS_ADD_VAR_INFO(m_double, "double"),
							DECS_ADD_VAR_INFO(m_string, "string"),
							DECS_ADD_VAR_INFO(m_intVector, "int_vector"),
							DECS_ADD_VAR_INFO(m_stringVector, "string_vector")
)

namespace DonerECS
{
	class CReflectionTest : public ::testing::Test
	{
	public:
		CReflectionTest()
			: m_componentFactoryManager(nullptr)
			, m_entityManager(nullptr)
			, m_prefabManager(nullptr)
			, m_tagManager(nullptr)
		{
			CDonerECSSystems& systems = CDonerECSSystems::CreateInstance()->Init();
			m_componentFactoryManager = systems.GetComponentFactoryManager();
			m_entityManager = systems.GetEntityManager();
			m_prefabManager = systems.GetPrefabManager();
			m_tagManager = systems.GetTagsManager();

			ADD_COMPONENT_FACTORY("foo", ::ReflectionTestInternal::CCompFoo, 10);

			m_tagManager->RegisterTag("tag1");
			m_tagManager->RegisterTag("tag2");
			m_tagManager->RegisterTag("tag3");
		}

		~CReflectionTest()
		{
			CDonerECSSystems::DestroyInstance();
		}

		CComponentFactoryManager *m_componentFactoryManager;
		CEntityManager *m_entityManager;
		CPrefabManager *m_prefabManager;
		CTagsManager *m_tagManager;
	};

	TEST_F(CReflectionTest, parse_entity_with_full_data)
	{
		CEntityParser parser;
		CEntity* entity = parser.ParseSceneFromJson(::ReflectionTestInternal::FULL_DATA_ENTITY);
		::ReflectionTestInternal::CCompFoo* component = entity->GetComponent<::ReflectionTestInternal::CCompFoo>();
		
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

	TEST_F(CReflectionTest, parse_entity_with_partial_data)
	{
		CEntityParser parser;
		CEntity* entity = parser.ParseSceneFromJson(::ReflectionTestInternal::PARTIALLY_FULL_ENTITY);
		::ReflectionTestInternal::CCompFoo* component = entity->GetComponent<::ReflectionTestInternal::CCompFoo>();
		
		EXPECT_EQ(::ReflectionTestInternal::INT_DEFAULT_VALUE, component->m_int);
		EXPECT_EQ(::ReflectionTestInternal::FLOAT_DEFAULT_VALUE, component->m_float);
		EXPECT_EQ(true, component->m_bool);
		EXPECT_EQ(::ReflectionTestInternal::LONG_LONG_DEFAULT_VALUE, component->m_longLong);
		EXPECT_EQ(::ReflectionTestInternal::DOUBLE_DEFAULT_VALUE, component->m_double);
		EXPECT_EQ("std::string", component->m_string);

		EXPECT_EQ(0, component->m_intVector.size());

		EXPECT_EQ(3, component->m_stringVector.size());
		EXPECT_EQ("one", component->m_stringVector[0]);
		EXPECT_EQ("two", component->m_stringVector[1]);
		EXPECT_EQ("three", component->m_stringVector[2]);
	}

	TEST_F(CReflectionTest, parse_entity_with_no_data)
	{
		CEntityParser parser;
		CEntity* entity = parser.ParseSceneFromJson(::ReflectionTestInternal::EMPTY_ENTITY);
		::ReflectionTestInternal::CCompFoo* component = entity->GetComponent<::ReflectionTestInternal::CCompFoo>();
		
		EXPECT_EQ(::ReflectionTestInternal::INT_DEFAULT_VALUE, component->m_int);
		EXPECT_EQ(::ReflectionTestInternal::FLOAT_DEFAULT_VALUE, component->m_float);
		EXPECT_EQ(::ReflectionTestInternal::BOOL_DEFAULT_VALUE, component->m_bool);
		EXPECT_EQ(::ReflectionTestInternal::LONG_LONG_DEFAULT_VALUE, component->m_longLong);
		EXPECT_EQ(::ReflectionTestInternal::DOUBLE_DEFAULT_VALUE, component->m_double);
		EXPECT_EQ(::ReflectionTestInternal::STRING_DEFAULT_VALUE, component->m_string);

		EXPECT_EQ(0, component->m_intVector.size());
		EXPECT_EQ(0, component->m_stringVector.size());
	}
}
