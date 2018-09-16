////////////////////////////////////////////////////////////
//
// MIT License
//
// DonerComponents
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

#include <donercomponents/common/CSingleton.h>
#include <donercomponents/utils/hash/CStrID.h>

#include <bitset>
#include <map>

namespace DonerComponents
{
	using TagsMask = std::bitset<MAX_TAGS>;

	class CTagsManager
	{
		friend class CDonerComponentsSystems;
	public:
		bool RegisterTag(CStrID tag);
		int GetTagIdx(CStrID tag) const;
		bool SetTag(TagsMask& mask, CStrID tag, bool add);

		bool HasTag(const TagsMask& mask, CStrID tag) const;

		template<typename... Args>
		bool AddTags(TagsMask& mask, Args... args)
		{
			return AddTagsInternal(mask, CStrID(args)...);
		}

		template<typename... Args>
		bool RemoveTags(TagsMask& mask, Args... args)
		{
			return RemoveTagsInternal(mask, CStrID(args)...);
		}

		template<typename... Args>
		TagsMask GetTagMask(Args... args)
		{
			TagsMask mask;
			GetTagMaskInternal(mask, CStrID(args)...);
			return mask;
		}

		template<typename... Args>
		bool HasAllTags(const TagsMask& mask, Args... args) const
		{
			return HasAllTagsInternal(mask, CStrID(args)...);
		}

		template<typename... Args>
		bool HasAnyTags(const TagsMask& mask, Args... args) const
		{
			return HasAnyTagsInternal(mask, CStrID(args)...);
		}

		bool ParseTagsFromFile(const char* const path);
		bool ParseTagsFromMemory(const unsigned char* jsonStringBuffer, std::size_t size);
		bool ParseTagsFromJson(const char* const jsonStr);

	private:
		CTagsManager() = default;

		template<typename... Args>
		bool AddTagsInternal(TagsMask& mask, CStrID tag, Args... args)
		{
			bool success = SetTag(mask, tag, true);
			return AddTagsInternal(mask, args...) && success;
		}

		bool AddTagsInternal(TagsMask& mask, CStrID tag)
		{
			return SetTag(mask, tag, true);
		}

		template<typename... Args>
		bool RemoveTagsInternal(TagsMask& mask, CStrID tag, Args... args)
		{
			bool success = SetTag(mask, tag, false);
			return RemoveTagsInternal(mask, args...) && success;
		}

		bool RemoveTagsInternal(TagsMask& mask, CStrID tag)
		{
			return SetTag(mask, tag, false);
		}

		template<typename... Args>
		void GetTagMaskInternal(TagsMask& mask, CStrID tag, Args... args)
		{
			SetTag(mask, tag, true);
			GetTagMaskInternal(mask, args...);
		}

		void GetTagMaskInternal(TagsMask& mask, CStrID tag)
		{
			SetTag(mask, tag, true);
		}

		template<typename... Args>
		bool HasAllTagsInternal(const TagsMask& mask, CStrID tag, Args... args) const
		{
			return HasAllTagsInternal(mask, args...) && mask.test(GetTagIdx(tag));
		}

		bool HasAllTagsInternal(const TagsMask& mask, CStrID tag) const
		{
			return HasTag(mask, tag);
		}

		template<typename... Args>
		bool HasAnyTagsInternal(const TagsMask& mask, CStrID tag, Args... args) const
		{
			return HasAnyTagsInternal(mask, args...) || mask.test(GetTagIdx(tag));
		}

		bool HasAnyTagsInternal(const TagsMask& mask, CStrID tag) const
		{
			return HasTag(mask, tag);
		}

		std::map<CStrID, int> m_registeredTags;
	};
}
