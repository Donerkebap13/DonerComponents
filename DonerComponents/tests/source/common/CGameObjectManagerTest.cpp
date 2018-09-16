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

#include <donercomponents/CDonerComponentsSystems.h>
#include <donercomponents/gameObject/CGameObject.h>
#include <donercomponents/component/CComponent.h>
#include <donercomponents/component/CComponentFactoryManager.h>
#include <donercomponents/handle/CHandle.h>

#include <gtest/gtest.h>

namespace DonerComponents
{
	class CGameObjectManagerTest : public ::testing::Test
	{
	public:
		CGameObjectManagerTest()
			: m_componentFactoryManager(nullptr)
			, m_gameObjectManager(nullptr)
		{
			CDonerComponentsSystems& systems = CDonerComponentsSystems::CreateInstance()->Init();
			m_componentFactoryManager = systems.GetComponentFactoryManager();
			m_gameObjectManager = systems.GetGameObjectManager();
		}

		~CGameObjectManagerTest()
		{
			CDonerComponentsSystems::DestroyInstance();
		}

		CComponentFactoryManager *m_componentFactoryManager;
		CGameObjectManager *m_gameObjectManager;
	};

	TEST_F(CGameObjectManagerTest, create_gameObject)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
	}
}
