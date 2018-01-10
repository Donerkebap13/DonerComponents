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
		template<>
		Optional<std::string> ReflectData<std::string>(const DonerECS::Json::Value& att)
		{
			if (att.isString())
			{
				return Optional<std::string>(att.asString());
			}
			return Optional<std::string>();
		}

		template<>
		Optional<const char*> ReflectData<const char*>(const DonerECS::Json::Value& att)
		{
			if (att.isString())
			{
				return Optional<const char*>(att.asCString());
			}
			return Optional<const char*>();
		}

		template<>
		Optional<bool> ReflectData<bool>(const DonerECS::Json::Value& att)
		{
			if (att.isBool())
			{
				return Optional<bool>(att.asBool());
			}
			return Optional<bool>();
		}

		template<>
		Optional<int> ReflectData<int>(const DonerECS::Json::Value& att)
		{
			if (att.isInt())
			{
				return Optional<int>(att.asInt());
			}
			return Optional<int>();
		}

		template<>
		Optional<long long> ReflectData<long long>(const DonerECS::Json::Value& att)
		{
			if (att.isInt64())
			{
				return Optional<long long>(att.asInt64());
			}
			return Optional<long long>();
		}

		template<>
		Optional<float> ReflectData<float>(const DonerECS::Json::Value& att)
		{
			if (att.isDouble())
			{
				return Optional<float>(att.asFloat());
			}
			return Optional<float>();
		}

		template<>
		Optional<double> ReflectData<double>(const DonerECS::Json::Value& att)
		{
			if (att.isDouble())
			{
				return Optional<double>(att.asDouble());
			}
			return Optional<double>();
		}
	}
}
