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

#include <functional>

namespace DonerECS
{
	template<typename TypeClass>
	class CSingleton
	{
	public:
		template<typename... Args>
		static TypeClass* CreateInstance(Args... args)
		{
			if (s_instance == nullptr)
			{
				s_instance = new TypeClass(std::forward<Args>(args)...);
			}
			return s_instance;
		}

		static void DestroyInstance()
		{
			delete s_instance;
			s_instance = nullptr;
		}

		static inline TypeClass* Get()
		{
			return s_instance;
		}

	protected:
		CSingleton() {}
		~CSingleton() {}

	private:
		static TypeClass* s_instance;
	};

	template<typename TypeClass>
	TypeClass* CSingleton<TypeClass>::s_instance = nullptr;
}
