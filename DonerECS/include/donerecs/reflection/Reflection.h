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

#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#define DECS_DEFINE_REFLECTION_DATA(base_class, ...)                                                     \
	template <>                                                                                          \
	struct SDonerECSReflectedClassProperties<base_class>                                                 \
	{                                                                                                    \
		using T = base_class;                                                                            \
		constexpr static auto s_properties = std::make_tuple(__VA_ARGS__);                             \
		constexpr static std::size_t s_propertiesCount = std::tuple_size<decltype(s_properties)>::value; \
	};

#define DECS_ADD_VAR_INFO(var, name) \
	DonerECS::Reflection::SProperty<T, decltype(T::var)> { &T::var, name }

#define DECS_DECLARE_COMPONENT_AS_REFLECTABLE(base_class)        \
	friend struct SDonerECSReflectedClassProperties<base_class>; \
public:                                                          \
	void ParseAtts(const DonerECS::Json::Value& atts) override;

#define DECS_COMPONENT_REFLECTION_IMPL(base_class) \
	void base_class::ParseAtts(const DonerECS::Json::Value& atts) { ParseAttsInternal<base_class>(this, atts); }

// Base properties container class (outside any namespace for an easier usage)
template <typename T>
struct SDonerECSReflectedClassProperties
{
	constexpr static auto s_properties = std::tuple<>();
	constexpr static std::size_t s_propertiesCount = std::tuple_size<decltype(s_properties)>::value;
};

namespace DonerECS
{
	namespace Reflection
	{
		template <typename Class, typename T>
		struct SProperty
		{
			constexpr SProperty(T Class::*member, const char* name)
				: m_member(member)
				, m_name(name)
			{
			}

			using Type = T;

			T Class::*m_member;
			const char* m_name;
		};

		template <class T, class Enable = void>
		struct STypeReflector
		{
			static std::experimental::optional<T> DeserializeFromJson(const DonerECS::Json::Value& /*att*/)
			{
				return std::experimental::nullopt;
			}
		};

		template <std::size_t iteration, typename T>
		void DoDeserializeDataFromJson(T& object, const DonerECS::Json::Value& data)
		{
			// get the property
			constexpr auto property =
				std::get<iteration>(std::decay<SDonerECSReflectedClassProperties<T>>::type::s_properties);

			// get the type of the property
			using Type = typename decltype(property)::Type;

			// set the value to the member
			auto op = STypeReflector<Type>::DeserializeFromJson(data[property.m_name]);
			if (op)
			{
				object.*(property.m_member) = op.value();
			}
		}

		template <std::size_t iteration, typename T>
		typename std::enable_if<(iteration == 0)>::type DeserializeDataFromJson(T& object, const DonerECS::Json::Value& data)
		{
			DoDeserializeDataFromJson<iteration>(object, data);
		}

		template <std::size_t iteration, typename T>
		typename std::enable_if<(iteration > 0)>::type DeserializeDataFromJson(T& object, const DonerECS::Json::Value& data)
		{
			DoDeserializeDataFromJson<iteration>(object, data);
			DeserializeDataFromJson<iteration - 1>(object, data);
		}

		template <class T>
		class CReflectionHelper
		{
		public:
			static std::experimental::optional<T> DeserializeFromJson(const DonerECS::Json::Value& att, T* value)
			{
				if (value != nullptr)
				{
					DeserializeDataFromJson<SDonerECSReflectedClassProperties<T>::s_propertiesCount - 1>(*value, att);
				}
				else
				{
					T object;
					DeserializeDataFromJson<SDonerECSReflectedClassProperties<T>::s_propertiesCount - 1>(object, att);
					return std::experimental::make_optional<T>(std::move(object));
				}
				return std::experimental::nullopt;
			}
		};
	}
}

// ===========================
// Basic Types Specializations
// ===========================
namespace DonerECS
{
	namespace Reflection
	{
		template <>
		struct STypeReflector<int>
		{
			static std::experimental::optional<int> DeserializeFromJson(const DonerECS::Json::Value& att);
		};

		template <class T>
		struct STypeReflector<T, typename std::enable_if<std::is_enum<T>::value>::type>
		{
			static std::experimental::optional<T> DeserializeFromJson(const DonerECS::Json::Value& att)
			{
				auto op = STypeReflector<int>::DeserializeFromJson(att);
				if (op)
				{
					T dummy = static_cast<T>(op.value());
					return std::experimental::make_optional<T>(std::move(dummy));
				}
				return std::experimental::nullopt;
			}
		};

		// std::vector
		template<template<typename, typename> class TT, typename T1, typename T2>
		struct STypeReflector<TT<T1, T2>>
		{
			using Vector = TT<T1, T2>;
			static std::experimental::optional<Vector> DeserializeFromJson(const DonerECS::Json::Value& atts)
			{
				if (atts.isArray())
				{
					Vector v;
					for (DonerECS::Json::ArrayIndex i = 0; i < atts.size(); ++i)
					{
						const DonerECS::Json::Value& att = atts[i];
						auto op = STypeReflector<T1>::DeserializeFromJson(att);
						if (op)
						{
							v.emplace_back(op.value());
						}
					}
					return std::experimental::make_optional<Vector>(std::move(v));
				}
				return std::experimental::nullopt;
			}
		};

		template<>
		struct STypeReflector<std::string>
		{
		static std::experimental::optional<std::string> DeserializeFromJson(const DonerECS::Json::Value& att);
		};

		template<>
		struct STypeReflector<bool>
		{
		static std::experimental::optional<bool> DeserializeFromJson(const DonerECS::Json::Value& att);
		};

		template<>
		struct STypeReflector<long long>
		{
		static std::experimental::optional<long long> DeserializeFromJson(const DonerECS::Json::Value& att);
		};

		template<>
		struct STypeReflector<float>
		{
		static std::experimental::optional<float> DeserializeFromJson(const DonerECS::Json::Value& att);
		};

		template<>
		struct STypeReflector<double>
		{
		static std::experimental::optional<double> DeserializeFromJson(const DonerECS::Json::Value& att);
		};
	}
}
