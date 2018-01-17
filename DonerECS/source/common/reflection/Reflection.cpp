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
		Optional<std::string> SDataReflector<std::string>::ReflectData(const DonerECS::Json::Value& att)
		{
			if (att.isString())
			{
				return Optional<std::string>(att.asString());
			}
			return Optional<std::string>();
		}

		Optional<bool> SDataReflector<bool>::ReflectData(const DonerECS::Json::Value& att)
		{
			if (att.isBool())
			{
				return Optional<bool>(att.asBool());
			}
			return Optional<bool>();
		}

		Optional<int> SDataReflector<int>::ReflectData(const DonerECS::Json::Value& att)
		{
			if (att.isInt())
			{
				return Optional<int>(att.asInt());
			}
			return Optional<int>();
		}

		Optional<long long> SDataReflector<long long>::ReflectData(const DonerECS::Json::Value& att)
		{
			if (att.isInt64())
			{
				return Optional<long long>(att.asInt64());
			}
			return Optional<long long>();
		}

		Optional<float> SDataReflector<float>::ReflectData(const DonerECS::Json::Value& att)
		{
			if (att.isDouble())
			{
				return Optional<float>(att.asFloat());
			}
			return Optional<float>();
		}

		Optional<double> SDataReflector<double>::ReflectData(const DonerECS::Json::Value& att)
		{
			if (att.isDouble())
			{
				return Optional<double>(att.asDouble());
			}
			return Optional<double>();
		}
	}
}
