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

#include <donerecs/reflection/Reflection.h>

namespace DonerECS
{
	namespace Reflection
	{
		std::experimental::optional<int> STypeReflector<int>::DeserializeFromJson(const DonerECS::Json::Value& att)
		{
			if (att.isInt())
			{
				return std::experimental::make_optional<int>(att.asInt());
			}
			return std::experimental::nullopt;
		}

		std::experimental::optional<std::string> STypeReflector<std::string>::DeserializeFromJson(const DonerECS::Json::Value& att)
		{
			if (att.isString())
			{
				return std::experimental::optional<std::string>(att.asString());
			}
			return std::experimental::nullopt;
		}

		std::experimental::optional<bool> STypeReflector<bool>::DeserializeFromJson(const DonerECS::Json::Value& att)
		{
			if (att.isBool())
			{
				return std::experimental::optional<bool>(att.asBool());
			}
			return std::experimental::optional<bool>();
		}

		

		std::experimental::optional<long long> STypeReflector<long long>::DeserializeFromJson(const DonerECS::Json::Value& att)
		{
			if (att.isInt64())
			{
				return std::experimental::optional<long long>(att.asInt64());
			}
			return std::experimental::nullopt;
		}

		std::experimental::optional<float> STypeReflector<float>::DeserializeFromJson(const DonerECS::Json::Value& att)
		{
			if (att.isDouble())
			{
				return std::experimental::optional<float>(att.asFloat());
			}
			return std::experimental::nullopt;
		}

		std::experimental::optional<double> STypeReflector<double>::DeserializeFromJson(const DonerECS::Json::Value& att)
		{
			if (att.isDouble())
			{
				return std::experimental::optional<double>(att.asDouble());
			}
			return std::experimental::nullopt;
		}
	}
}
