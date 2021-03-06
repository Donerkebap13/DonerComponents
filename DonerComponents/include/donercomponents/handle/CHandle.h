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

#include <donercomponents/Defines.h>
#include <donercomponents/CDonerComponentsSystems.h>
#include <donercomponents/component/CComponentFactoryManager.h>

#include <functional>

namespace DonerComponents
{
	class CGameObject;
	class CComponent;

	class CHandle
	{
	public:
		static constexpr int MAX_ELEMENT_TYPES = 4;
		static constexpr int MAX_COMPONENT_TYPES = 512;
		static constexpr int MAX_ELEMENTS = 8192;
		static constexpr int MAX_VERSIONS = 256;

		enum EElementType : unsigned { None = 0, GameObject, Component };

        CHandle();
		CHandle(CGameObject* rhs);
		CHandle(CComponent* rhs);

		const CHandle & operator=(CGameObject* rhs);
		const CHandle & operator=(CComponent* rhs);
		operator CGameObject*();
		operator bool();
		operator int() const;

		bool operator==(const CHandle& rhs) const;
		bool operator!=(const CHandle& rhs) const;

		template<typename T>
		operator T*()
		{
			static_assert(
				std::is_base_of<CComponent, T>::value,
				"T must inherits from CComponent"
				);

			if (*this)
			{
				return static_cast<T*>(CDonerComponentsSystems::Get()->GetComponentFactoryManager()->GetComponent(m_componentIdx, m_elementPosition, m_version));
			}
            else
            {
                return nullptr;
            }
		}

		template<typename T>
		void SendMessage(const T& message, ESendMessageType type = ESendMessageType::NonRecursive);
		template<typename T>
		void SendMessageToChildren(const T& message, ESendMessageType type = ESendMessageType::NonRecursive);
		template<typename T>
		void PostMessage(const T& message, ESendMessageType type = ESendMessageType::NonRecursive);
		template<typename T>
		void PostMessageToChildren(const T& message, ESendMessageType type = ESendMessageType::NonRecursive);

		void Destroy();

		unsigned m_elementType : 2; //  4
		unsigned m_componentIdx : 9; // 512
		unsigned m_elementPosition : 13; // 8.192
		unsigned m_version : 8; // 256
	};
}

namespace std
{
	template <>
	struct hash<DonerComponents::CHandle>
	{
		std::size_t operator()(const DonerComponents::CHandle& handle) const 
		{ 
			return std::hash<int>()(handle);
		}
	};
}
