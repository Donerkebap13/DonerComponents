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

namespace DonerECS
{
	class CMsgHandlerBase
	{
	public:
		template<typename C, typename T>
		void Execute(C* caller, const T& param)
		{
			CMsgHandler<C, T>* myself = static_cast<CMsgHandler<C, T>*>(this);
			myself->ExecuteFunction(caller, param);
		}
	};

	template<typename C, typename T>
	class CMsgHandler : public CMsgHandlerBase
	{
	public:
		CMsgHandler(void(C::*function)(const T&))
			: m_function(function)
		{}

		void ExecuteFunction(C* caller, const T& param)
		{
			(caller->*m_function)(param);
		}

	private:
		void (C::*m_function)(const T&);
	};
}
