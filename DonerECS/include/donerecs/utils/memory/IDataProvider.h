////////////////////////////////////////////////////////////
//
// MIT License
//
// DonerECS - A Tweaked GameObject-Component System
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

#include <string>

namespace DonerECS
{
	class IDataProvider
	{
	public:
		enum class ESeekType
		{
			FromStart = SEEK_SET,
			FromCurrent = SEEK_CUR,
			FromEndOfFile = SEEK_END
		};

		IDataProvider() {}

		virtual bool IsValid() const = 0;
		virtual void Read(void *where, size_t nBytes) = 0;

		virtual std::size_t Seek(std::size_t offset, ESeekType from) = 0;
		virtual std::size_t Tell() const = 0;

		const char *GetName() const { return m_name.c_str(); }
		void SetName(const char *newName) { m_name = std::string(newName); }

		template< class T >
		void ReadPOD(T &data)
		{
			Read(&data, sizeof(T));
		}

		int ReadLong()
		{
			int x;
			ReadPOD(x);
			return x;
		}

		// Specialization for bool always read 4 bytes
		void ReadPOD(bool &data)
		{
			int x = ReadLong();
			data = (x != 0);
		}

	protected:
		virtual ~IDataProvider() {}

		std::string m_name;
		FILE *m_file;
	};
}