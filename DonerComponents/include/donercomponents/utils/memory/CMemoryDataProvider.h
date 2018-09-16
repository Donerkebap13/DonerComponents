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

#include <donercomponents/utils/memory/IDataProvider.h>

#include <cassert>

namespace DonerComponents
{
	class CMemoryDataProvider : public IDataProvider
	{
	public:
		using u8 = unsigned char;

		CMemoryDataProvider();
		explicit CMemoryDataProvider(const char* fileName);
		// Memory not owned by this class
		CMemoryDataProvider(const u8* data, std::size_t nBytes);
		// Memory owned by this class
		explicit CMemoryDataProvider(std::size_t nBytes);
		CMemoryDataProvider(const CMemoryDataProvider& mdp);

		void operator =(const CMemoryDataProvider& mdp);

		~CMemoryDataProvider();
		void Destroy();
		bool Load(const char* fileName);
		bool Load(IDataProvider& dp, std::size_t nBytes);

		std::size_t GetSize() const { return m_nBytes; }
		const u8* GetBaseData() const { return m_base; }

		void Read(void* where, std::size_t nBytes) override;
		bool IsValid() const override;
		bool IsParent() const;

		std::size_t Seek(std::size_t offset, ESeekType from) override;
		std::size_t Tell() const override { return m_currentPosition - m_base; }
		void Rewind();

		std::size_t GetRemainingBytes() const { return GetSize() - (m_currentPosition - m_base); }

		template< class T >
		T* AssignPOD()
		{
			return ConsumeBytes<T>(sizeof(T));
		}

		template< class T >
		T* AssignPODArray(std::size_t nobjects)
		{
			return ConsumeBytes<T>(sizeof(T)*  nobjects);
		}

		const unsigned char* GetTop() const
		{
			return m_currentPosition;
		}

		template< class T >
		T* ConsumeBytes(std::size_t nBytes)
		{
			assert(nBytes <= GetRemainingBytes());
			assert(m_currentPosition + nBytes <= m_base + GetSize());
			T* base = (T*)(m_currentPosition);
			m_currentPosition += nBytes;
			return base;
		}

		const void* GetBufferToWrite() const { return m_base; }

	private:
		u8* m_allocatedMemory;
		const u8* m_base;
		std::size_t m_nBytes;
		const u8* m_currentPosition;
	};
}
