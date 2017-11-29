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

#include <donerecs/common/CECSElement.h>

#include <cstdio>
#include <memory>
#include <vector>

namespace DonerECS
{
	template< typename T>
	class CFactory
	{
		static_assert(
			std::is_base_of<CECSElement, T>::value,
			"T must inherits from CECSElement if you want to use a CFactory"
			);
	public:
		CFactory(std::size_t numElements)
			: m_numElements(numElements)
		{
			m_entries.resize(m_numElements);
			m_buffer = malloc(sizeof(T) * m_numElements);

			T* buffer = static_cast<T*>(m_buffer);
			m_entries[0].m_data = buffer;
			++buffer;
			for (size_t i = 1; i < m_numElements; ++i)
			{
				m_entries[i].m_data = buffer;
				m_entries[i - 1].m_next = &m_entries[i];
				++buffer;
			}
			m_current = &m_entries[0];
		}

		virtual ~CFactory()
		{
			for (SEntry& entry : m_entries)
			{
				if (entry.m_used)
				{
					entry.m_data->~T();
				}
			}
			delete m_buffer;
		}

		T* GetNewElement()
		{
			if (m_current)
			{
				T* data = new(static_cast<void*>(m_current->m_data))T();
				data->SetVersion(m_current->m_version);
				m_current->m_used = true;
				m_current = m_current->m_next;
				return data;
			}
			printf("No room for more!\n");
			return nullptr;
		}

		bool DestroyElement(T** data)
		{
			if (*data)
			{
				SEntry *entry = FindElement(*data);
				if (entry)
				{
					entry->m_data->~T();
					++entry->m_version;
					entry->m_next = m_current;
					entry->m_used = false;
					m_current = entry;
					*data = nullptr;
					return true;
				}
			}
			return false;
		}

		int GetPositionForElement(T* data)
		{
			if (data)
			{
				for (std::size_t i = 0; i < m_numElements; ++i)
				{
					if (m_entries[i].m_data == data && data->GetVersion() == m_entries[i].m_version)
					{
						return i;
					}
				}
			}
			return -1;
		}

		T* GetElementByIdxAndVersion(std::size_t index, int version)
		{
			if (index >= 0 && index < m_numElements && m_entries[index].m_version == version)
			{
				return m_entries[index].m_data;
			}
			return nullptr;
		}

	protected:
		struct SEntry
		{
			T* m_data;
			SEntry *m_next;
			int m_version;
			bool m_used;
			SEntry() : m_data(nullptr), m_next(nullptr), m_version(0), m_used(false) {}
		};

		void* m_buffer;

		std::size_t m_numElements;
		std::vector<SEntry> m_entries;
		SEntry* m_current;

		SEntry* FindElement(T* data)
		{
			for (SEntry& entry : m_entries)
			{
				if (entry.m_data == data)
				{
					return &entry;
				}
			}
			return nullptr;
		}
	};
}
