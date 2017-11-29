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

#include <donerecs/entity/CEntity.h>
#include <donerecs/entity/CEntityManager.h>
#include <donerecs/component/CComponent.h>
#include <donerecs/component/CComponentFactoryManager.h>
#include <donerecs/handle/CHandle.h>

#include <gtest/gtest.h>

namespace DonerECS
{
	class CEntityManagerTest : public ::testing::Test
	{
	public:
		CEntityManagerTest()
			: m_componentFactoryManager(CComponentFactoryManager::CreateInstance())
			, m_entityManager(CEntityManager::CreateInstance())
		{
		}

		~CEntityManagerTest()
		{
			CComponentFactoryManager::DestroyInstance();
			CEntityManager::DestroyInstance();
		}

		CEntityManager *m_entityManager;
		CComponentFactoryManager *m_componentFactoryManager;
	};

	TEST_F(CEntityManagerTest, destroy_entity_with_entity)
	{
		CEntity* entity = m_entityManager->GetNewElement();
		EXPECT_NE(nullptr, entity);
		m_entityManager->DestroyEntity(&entity);
		EXPECT_EQ(nullptr, entity);
	}

	TEST_F(CEntityManagerTest, destroy_entity_with_handle)
	{
		CHandle entity = m_entityManager->GetNewElement();
		EXPECT_TRUE(static_cast<bool>(entity));
		m_entityManager->DestroyEntity(entity);
		EXPECT_FALSE(static_cast<bool>(entity));
	}
}
