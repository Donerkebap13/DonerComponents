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

#include <donerecs/common/CFactory.h>
#include <donerecs/common/CECSElement.h>

#include <gtest/gtest.h>

namespace DonerECS
{
	namespace VersionableFactoryTestInternal
	{
		class Foo : public CECSElement
		{};
	}

	class CFactoryTest : public ::testing::Test
	{
	public:
		CFactoryTest()
			: m_factory(1)
		{}

		~CFactoryTest()
		{}
		CFactory<VersionableFactoryTestInternal::Foo> m_factory;
	};

	TEST_F(CFactoryTest, init_test)
	{
		EXPECT_TRUE(true);
	}

	TEST_F(CFactoryTest, get_valid_value)
	{
		VersionableFactoryTestInternal::Foo* foo1 = m_factory.GetNewElement();
		EXPECT_NE(nullptr, foo1);
	}

	TEST_F(CFactoryTest, get_invalid_value_if_overflow)
	{
		// googlemock improvement
		VersionableFactoryTestInternal::Foo* foo1 = m_factory.GetNewElement();
		EXPECT_NE(nullptr, foo1);
		VersionableFactoryTestInternal::Foo* foo2 = m_factory.GetNewElement();
		EXPECT_EQ(nullptr, foo2);
	}

	TEST_F(CFactoryTest, check_value_version)
	{
		VersionableFactoryTestInternal::Foo* foo1 = m_factory.GetNewElement();
		EXPECT_EQ(0, foo1->GetVersion());
	}

	TEST_F(CFactoryTest, check_free)
	{
		VersionableFactoryTestInternal::Foo* foo1 = m_factory.GetNewElement();
		EXPECT_NE(nullptr, foo1);
		m_factory.DestroyElement(&foo1);
		EXPECT_EQ(nullptr, foo1);
		VersionableFactoryTestInternal::Foo* foo2 = m_factory.GetNewElement();
		EXPECT_NE(nullptr, foo2);
	}

	TEST_F(CFactoryTest, check_destroy_null_value)
	{
		VersionableFactoryTestInternal::Foo* foo = nullptr;
		m_factory.DestroyElement(&foo);
		EXPECT_TRUE(true);
	}

	TEST_F(CFactoryTest, check_version_increment)
	{
		VersionableFactoryTestInternal::Foo* foo1 = m_factory.GetNewElement();
		EXPECT_NE(nullptr, foo1);
		m_factory.DestroyElement(&foo1);
		EXPECT_EQ(nullptr, foo1);
		VersionableFactoryTestInternal::Foo* foo2 = m_factory.GetNewElement();
		EXPECT_NE(nullptr, foo2);
		EXPECT_EQ(1, foo2->GetVersion());
	}

	TEST_F(CFactoryTest, check_get_position)
	{
		CFactory<VersionableFactoryTestInternal::Foo> factory(2);
		VersionableFactoryTestInternal::Foo* foo1 = factory.GetNewElement();
		EXPECT_NE(nullptr, foo1);
		int pos1 = factory.GetPositionForElement(foo1);
		EXPECT_EQ(pos1, 0);
		VersionableFactoryTestInternal::Foo* foo2 = factory.GetNewElement();
		EXPECT_NE(nullptr, foo2);
		int pos2 = factory.GetPositionForElement(foo2);
		EXPECT_EQ(pos2, 1);
	}

	TEST_F(CFactoryTest, check_get_value_with_position_and_version)
	{
		VersionableFactoryTestInternal::Foo* foo1 = m_factory.GetNewElement();
		EXPECT_NE(nullptr, foo1);
		VersionableFactoryTestInternal::Foo* foo2 = m_factory.GetElementByIdxAndVersion(0, 0);
		EXPECT_EQ(foo2, foo1);
	}

	TEST_F(CFactoryTest, check_get_value_with_position_and_incorrect_version)
	{
		VersionableFactoryTestInternal::Foo* foo1 = m_factory.GetNewElement();
		EXPECT_NE(nullptr, foo1);
		int version = foo1->GetVersion();
		m_factory.DestroyElement(&foo1);
		VersionableFactoryTestInternal::Foo* foo2 = m_factory.GetElementByIdxAndVersion(0, version);
		EXPECT_EQ(nullptr, foo2);
		foo2 = m_factory.GetNewElement();
		EXPECT_NE(nullptr, foo2);
		foo2 = m_factory.GetElementByIdxAndVersion(0, version);
		EXPECT_EQ(nullptr, foo2);
	}
}