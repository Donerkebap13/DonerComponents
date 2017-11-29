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

#include <donerecs/utils/hash/elbeno_constexpr_murmur3.h>

namespace DonerECS
{
	class CStrID
	{
	public:
		// Helper to disambiguate
		struct TConstCharWrapper;

		// Null id, equal to CStrID( "" )
		// (internally 0, false when used as boolean)
		CStrID();

		// Construct/assign from a char array
		template <unsigned Len>
		CStrID(const char(&str)[Len]);
		template <unsigned Len>
		CStrID & operator=(const char(&str)[Len]);

		operator unsigned()
		{
			return id;
		}

		// Construct/assign from a char pointer
		// (wrapper disambiguates against compile-time version,
		// but forces you to use the explicit ctor when passed as argument)
		CStrID(const TConstCharWrapper & str);
		CStrID & operator=(const TConstCharWrapper & str);

		CStrID& operator=(const CStrID & s);
		bool operator==(const CStrID & s) const;
		bool operator!=(const CStrID & s) const;
		bool operator<(const CStrID & s) const;
		bool operator>(const CStrID & s) const;

		bool operator==(unsigned int v) const;
		bool operator!=(unsigned int v) const;
		bool operator<(unsigned int v) const;
		bool operator>(unsigned int v) const;

		struct TConstCharWrapper
		{
			inline TConstCharWrapper(const char * s) : str(s) {}
			const char * str;
		};

	private:
		unsigned id;

	};

	inline CStrID::CStrID()
		: id(0)
	{}

	template <unsigned Len>
	inline CStrID::CStrID(const char(&str)[Len])
	{
		*this = str;
	}

	template <unsigned Len>
	inline CStrID & CStrID::operator=(const char(&str)[Len])
	{
		id = cx::murmur3_32(str);
		return *this;
	}

	inline CStrID::CStrID(const TConstCharWrapper & str)
	{
		*this = str;
	}

	inline CStrID & CStrID::operator=(const TConstCharWrapper & str)
	{
		if (!str.str)
		{
			id = 0;
			return *this;
		}
		id = cx::murmur3_32(str.str);
		return *this;
	}

	inline CStrID& CStrID::operator=(const CStrID & s)
	{
		id = s.id;
		return *this;
	}

	inline bool CStrID::operator==(const CStrID & s) const
	{
		return id == s.id;
	}

	inline bool CStrID::operator!=(const CStrID & s) const
	{
		return id != s.id;
	}

	inline bool CStrID::operator<(const CStrID & s) const
	{
		return id < s.id;
	}

	inline bool CStrID::operator>(const CStrID & s) const
	{
		return id > s.id;
	}


	inline bool CStrID::operator==(unsigned int v) const
	{
		return id == v;
	}

	inline bool CStrID::operator!=(unsigned int v) const
	{
		return id != v;
	}

	inline bool CStrID::operator<(unsigned int v) const
	{
		return id < v;
	}

	inline bool CStrID::operator>(unsigned int v) const
	{
		return id > v;
	}
}
