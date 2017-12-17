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

#include <donerecs/utils/hash/CStrID.h>

#include <gtest/gtest.h>

namespace DonerECS
{
	namespace StrIdTestInternal
	{
		constexpr char* const TEST_1_STR = "Test1";
		constexpr int TEST_1_ID = 1875181088;
	}

	class CStrIDTest : public ::testing::Test
	{
	};

	TEST_F(CStrIDTest, check_runtime_hash)
	{
		std::string test1Str = std::string("Test") + std::string("1");
		CStrID test1(test1Str.c_str());
		
		EXPECT_EQ(static_cast<int>(test1), StrIdTestInternal::TEST_1_ID);
	}

	TEST_F(CStrIDTest, check_compilation_time_hash)
	{
		CStrID test1(StrIdTestInternal::TEST_1_STR);
		EXPECT_EQ(static_cast<int>(test1), StrIdTestInternal::TEST_1_ID);
	}

	TEST_F(CStrIDTest, check_runtime_hash_equals_compile_hash)
	{
		std::string test1Str = std::string("Test") + std::string("1");
		CStrID test1(test1Str.c_str());

		CStrID test2(StrIdTestInternal::TEST_1_STR);

		EXPECT_EQ(static_cast<int>(test1), static_cast<int>(test2));
	}
}
