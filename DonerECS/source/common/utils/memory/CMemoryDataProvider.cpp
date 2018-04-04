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

#include <donerecs/utils/memory/CMemoryDataProvider.h>
#include <donerecs/utils/memory/CFileDataProvider.h>

#include <cassert>
#include <cstring>

namespace DonerECS
{
	CMemoryDataProvider::CMemoryDataProvider()
		: m_allocatedMemory(nullptr)
		, m_base(nullptr)
		, m_nBytes(0)
		, m_currentPosition(nullptr)
	{}

	CMemoryDataProvider::CMemoryDataProvider(const char *filename)
		: m_allocatedMemory(nullptr)
		, m_base(nullptr)
		, m_nBytes(0)
		, m_currentPosition(nullptr)
	{
		Load(filename);
	}

	CMemoryDataProvider::CMemoryDataProvider(const u8* data, std::size_t anbytes)
		: m_allocatedMemory(nullptr)
		, m_base(data)
		, m_nBytes(anbytes)
		, m_currentPosition(data)
	{}

	CMemoryDataProvider::CMemoryDataProvider(size_t anbytes)
		: m_nBytes(anbytes)
	{
		m_allocatedMemory = static_cast<u8*>(malloc(m_nBytes));
		m_base = m_allocatedMemory;
		m_currentPosition = m_allocatedMemory;
	}

	CMemoryDataProvider::CMemoryDataProvider(const CMemoryDataProvider &mdp)
		: m_allocatedMemory(nullptr)
		, m_base(nullptr)
		, m_nBytes(0)
		, m_currentPosition(nullptr)
	{
		// Use the previous operator to do the checks
		(*this) = mdp;
	}

	void CMemoryDataProvider::operator =(const CMemoryDataProvider &dp)
	{
		// Check sender and receiver is not parent of some buffer, as it's not clear who takes parentship of those
		// buffers. So it's disallowed.
		assert(!dp.m_allocatedMemory);			// Confirm the original is not parent
		assert(!m_allocatedMemory);				// Confirm I'm not parent
		m_allocatedMemory = dp.m_allocatedMemory;
		m_base = dp.m_base;
		m_nBytes = dp.m_nBytes;
		m_currentPosition = dp.m_currentPosition;
		SetName(dp.GetName());
	}

	bool CMemoryDataProvider::Load(const char *filename)
	{

		CFileDataProvider dp(filename);

		if (!dp.IsValid())
		{
			printf("CMemoryDataProvider::load( %s ) : File not found\n", filename);
			return false;
		}

		size_t nbytes = CFileDataProvider::GetFileSize(filename);
		return Load(dp, nbytes);
	}

	bool CMemoryDataProvider::Load(IDataProvider &dp, std::size_t anbytes)
	{

		// Free memory if reloading
		if (m_allocatedMemory)
		{
			Destroy();
		}

		SetName(dp.GetName());
		m_nBytes = anbytes;

		if (m_nBytes > 0)
		{
			m_allocatedMemory = static_cast<u8*>(malloc(m_nBytes));
			if (m_allocatedMemory)
			{
				dp.Read(m_allocatedMemory, m_nBytes);
			}
			m_base = m_allocatedMemory;
			m_currentPosition = m_base;
			assert(m_base);
		}
		return true;
	}

	CMemoryDataProvider::~CMemoryDataProvider()
	{
		Destroy();
	}

	void CMemoryDataProvider::Destroy()
	{
		if (m_allocatedMemory)
		{
			free(m_allocatedMemory);
			m_allocatedMemory = nullptr;
		}
		m_nBytes = 0;
		m_base = nullptr;
		m_currentPosition = nullptr;
	}

	bool CMemoryDataProvider::IsValid() const
	{
		return (m_base != nullptr && GetSize() > 0);
	}

	bool CMemoryDataProvider::IsParent() const
	{
		return m_allocatedMemory != nullptr;
	}

	void CMemoryDataProvider::Read(void *where, std::size_t nbytes)
	{
		if (m_currentPosition + nbytes > m_base + GetSize())
		{
			printf("CMemoryDataProvider::read from dp(%s). Can't read %ld bytes, only %ld/%ld bytes remain at buffer\n",
				   GetName(), nbytes, m_base + GetSize() - m_currentPosition, GetSize());
			assert(m_currentPosition + nbytes <= m_base + GetSize());
		}
		memcpy(where, m_currentPosition, nbytes);
		m_currentPosition += nbytes;
	}

	std::size_t CMemoryDataProvider::Seek(std::size_t offset, ESeekType whence)
	{
		if (whence == ESeekType::FromStart)
		{
			m_currentPosition = m_base + offset;
		}
		else if (whence == ESeekType::FromCurrent)
		{
			m_currentPosition = m_currentPosition + offset;
		}
		else if (whence == ESeekType::FromEndOfFile)
		{
			m_currentPosition = m_base + GetSize() - offset;
		}
		return Tell();
	}

	void CMemoryDataProvider::Rewind()
	{
		m_currentPosition = m_base;
	}
}
