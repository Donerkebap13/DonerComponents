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

#include <donercomponents/utils/memory/CFileDataProvider.h>

#include <cassert>

namespace DonerComponents
{
	namespace CFileDataProviderInternal
	{
		static const std::size_t INFINITE_SIZE = (~(0));
	}

	CFileDataProvider::CFileDataProvider()
		: m_startOffset(0)
		, m_maxBytes(CFileDataProviderInternal::INFINITE_SIZE)
	{}

	CFileDataProvider::CFileDataProvider(const char* fileName)
		: m_startOffset(0)
		, m_maxBytes(CFileDataProviderInternal::INFINITE_SIZE)
	{
		Open(fileName);
	}

	CFileDataProvider::~CFileDataProvider()
	{
		m_startOffset = 0;
		m_maxBytes = CFileDataProviderInternal::INFINITE_SIZE;
		Close();
	}

	std::size_t CFileDataProvider::GetFileSize(const char* fileName)
	{
		CFileDataProvider dataProvider(fileName);
		if (!dataProvider.IsValid())
		{
			return 0;
		}
		std::size_t nBytes = dataProvider.Seek(0, ESeekType::FromEndOfFile);
		return nBytes;
	}

	std::size_t CFileDataProvider::GetSize() const
	{
		if (!m_file)
		{
			return 0;
		}
		std::size_t size = 0;
		fseek(m_file, 0L, SEEK_END);
		size = ftell(m_file);
		fseek(m_file, 0L, SEEK_SET);
		return size;
	}

	bool CFileDataProvider::FileExist(const char* fileName)
	{
		FILE* file = fopen(fileName, "rb");
		if (file)
		{
			fclose(file);
			return true;
		}
		return false;
	}

	bool CFileDataProvider::Open(const char* fileName)
	{
		SetName(fileName);
		m_file = fopen(fileName, "rb");
		if (!IsValid())
		{
			printf("Failed to open read file %s\n", fileName);
		}
		return IsValid();
	}

	bool CFileDataProvider::IsValid() const
	{
		return m_file != nullptr;
	}

	void CFileDataProvider::Close()
	{
		if (m_file)
		{
			fclose(m_file);
			m_file = nullptr;
		}
	}

	void CFileDataProvider::Read(void* where, std::size_t nBytes)
	{
		assert(IsValid());
		size_t nBytesRead = fread(where, 1, nBytes, m_file);
		assert(nBytesRead == nBytes);
	}

	std::size_t CFileDataProvider::Seek(std::size_t offset, ESeekType from)
	{
		assert(IsValid());
		int rc = fseek(m_file, static_cast<long>(offset), static_cast<int>(from));
		assert(rc == 0);
		std::size_t newPosition = static_cast<std::size_t>(ftell(m_file));
		assert(from != ESeekType::FromStart || newPosition == offset);
		return newPosition;
	}

	std::size_t CFileDataProvider::Tell() const
	{
		return ftell(m_file);
	}
}
