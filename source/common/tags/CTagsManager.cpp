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

#include <donerecs/tags/CTagsManager.h>

namespace DonerECS
{
	bool CTagsManager::RegisterTag(CStrID tag)
	{
		if (m_registeredTags.find(tag) == m_registeredTags.end())
		{
			m_registeredTags[tag] = m_registeredTags.size();
			return true;
		}
		return false;
	}

	int CTagsManager::GetTagIdx(CStrID tag) const
	{
		auto tagItr = m_registeredTags.find(tag);
		if (tagItr != m_registeredTags.end())
		{
			return tagItr->second;
		}
		return -1;
	}

	bool CTagsManager::SetTag(TagsMask& mask, CStrID tag, bool add)
	{
		int tagIdx = GetTagIdx(tag);
		if (tagIdx >= 0)
		{
			mask[tagIdx] = add;
			return true;
		}
		return false;
	}


	bool CTagsManager::HasTag(const TagsMask& mask, CStrID tag) const
	{
		int tagIdx = GetTagIdx(tag);
		return tagIdx >= 0 ? mask.test(tagIdx) : false;
	}
}