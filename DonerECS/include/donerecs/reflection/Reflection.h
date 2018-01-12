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

#pragma once

#include <donerecs/json/json.h>

#include <donerecs/akrzemi1_optional.h>

#include <tuple>
#include <type_traits>
#include <utility>

#define DECS_REFLECT_CLASS_DATA(base_class, ...) class base_class##_reflection_data { public: constexpr static auto s_properties = std::make_tuple(__VA_ARGS__); }
#define DECS_REFLECT_VAR(class, type, var, var_name) DonerECS::Reflection::SProperty<class, type>{ &class::var, var_name }

#define DECS_DECLARE_COMPONENT_AS_REFLECTABLE(base_class) friend class base_class##_reflection_data; public: void ParseAtts(const DonerECS::Json::Value& atts) override
#define DECS_COMPONENT_REFLECTION_IMPL(class) void class::ParseAtts(const DonerECS::Json::Value& atts) { ParseAttsInternal<class##_reflection_data, class>(this, atts); }

namespace DonerECS
{
	namespace Reflection
	{
		template<typename T>
		using Optional = std::experimental::optional<T>;

		template<typename Class, typename T>
		struct SProperty
		{
			constexpr SProperty(T Class::* member, const char* name)
				: m_member(member)
				, m_name(name)
			{}

			using Type = T;

			T Class::* m_member;
			const char* m_name;
		};

		template<typename T>
		Optional<T> ReflectData(const DonerECS::Json::Value& att)
		{
			assert(false);
			return Optional<T>();
		}

		template<>
		Optional<std::string> ReflectData<std::string>(const DonerECS::Json::Value& att);
		template<>
		Optional<const char*> ReflectData<const char*>(const DonerECS::Json::Value& att);
		template<>
		Optional<bool> ReflectData<bool>(const DonerECS::Json::Value& att);
		template<>
		Optional<int> ReflectData<int>(const DonerECS::Json::Value& att);
		template<>
		Optional<long long> ReflectData<long long>(const DonerECS::Json::Value& att);
		template<>
		Optional<float> ReflectData<float>(const DonerECS::Json::Value& att);
		template<>
		Optional<double> ReflectData<double>(const DonerECS::Json::Value& att);

		template<typename ReflectionData, std::size_t iteration, typename T>
		void DoSetReflectionData(T* object, const DonerECS::Json::Value& data) {
			// get the property
			constexpr auto property = std::get<iteration>(std::decay<ReflectionData>::type::s_properties);

			// get the type of the property
			using Type = typename decltype(property)::Type;

			// set the value to the member
			Optional<Type> op = DonerECS::Reflection::ReflectData<Type>(data[property.m_name]);
			if (op)
			{
				object->*(property.m_member) = op.value();
			}
		}

		template<typename ReflectionData, std::size_t iteration, typename T>
		typename std::enable_if<(iteration == 0)>::type SetReflectionData(T* object, const DonerECS::Json::Value& data) {
			DonerECS::Reflection::DoSetReflectionData<ReflectionData, iteration>(object, data);
		}

		template<typename ReflectionData, std::size_t iteration, typename T>
		typename std::enable_if<(iteration > 0)>::type SetReflectionData(T* object, const DonerECS::Json::Value& data) {
			DonerECS::Reflection::DoSetReflectionData<ReflectionData, iteration>(object, data);
			DonerECS::Reflection::SetReflectionData<ReflectionData, iteration - 1>(object, data);
		}
	}
}
