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
#include <donerecs/handle/CHandle.h>
#include <donerecs/component/CComponentFactoryManager.h>

#include <gtest/gtest.h>

namespace DonerECS
{
	namespace VersionableFactoryTestInternal
	{
		class Foo : public CECSElement
		{};
	}

	class CEntityHandleTest : public ::testing::Test
	{
	public:
		CEntityHandleTest()
			: m_entityManager(CEntityManager::CreateInstance())
			, m_componentFactoryManager(CComponentFactoryManager::CreateInstance())
		{
		}

		~CEntityHandleTest()
		{
			CEntityManager::DestroyInstance();
			CComponentFactoryManager::DestroyInstance();
		}

		CEntityManager *m_entityManager;
		CComponentFactoryManager *m_componentFactoryManager;
	};

	TEST_F(CEntityHandleTest, new_handle_is_invalid)
	{
		CHandle handle;
		EXPECT_FALSE(handle);
		EXPECT_EQ(0, handle.m_componentIdx);
		EXPECT_EQ(0, handle.m_elementPosition);
		EXPECT_EQ(0, handle.m_elementType);
		EXPECT_EQ(0, handle.m_version);

		CEntity* entity = handle;
		EXPECT_EQ(nullptr, entity);
		CComponent* component = handle;
		EXPECT_EQ(nullptr, component);
	}

	TEST_F(CEntityHandleTest, get_new_entity_pointer)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
	}

	TEST_F(CEntityHandleTest, get_new_entity_as_handle)
	{
		CHandle entityHandle = m_entityManager->CreateEntity();
		EXPECT_EQ(CHandle::EElementType::Entity, entityHandle.m_elementType);
		EXPECT_EQ(0, entityHandle.m_componentIdx);
		EXPECT_EQ(0, entityHandle.m_elementPosition);
		EXPECT_EQ(0, entityHandle.m_version);
		EXPECT_TRUE(static_cast<bool>(entityHandle));

		CHandle entityHandle2 = m_entityManager->CreateEntity();
		EXPECT_EQ(CHandle::EElementType::Entity, entityHandle2.m_elementType);
		EXPECT_EQ(0, entityHandle2.m_componentIdx);
		EXPECT_EQ(1, entityHandle2.m_elementPosition);
		EXPECT_EQ(0, entityHandle2.m_version);
		EXPECT_TRUE(static_cast<bool>(entityHandle));
	}

	TEST_F(CEntityHandleTest, cast_handle_to_entity)
	{
		CHandle entityHandle = m_entityManager->CreateEntity();

		CEntity* entity = entityHandle;
		EXPECT_NE(nullptr, entity);

		int pos = m_entityManager->GetPositionForElement(entity);
		EXPECT_EQ(pos, entityHandle.m_elementPosition);
		EXPECT_EQ(entity->GetVersion(), entityHandle.m_version);
		CEntity* entity2 = m_entityManager->GetElementByIdxAndVersion(pos, entity->GetVersion());
		EXPECT_EQ(entity2, entity);
	}

	TEST_F(CEntityHandleTest, cast_entity_to_handle)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);

		CHandle entityHandle = entity;
		EXPECT_EQ(CHandle::EElementType::Entity, entityHandle.m_elementType);
		EXPECT_EQ(0, entityHandle.m_componentIdx);
		EXPECT_EQ(0, entityHandle.m_elementPosition);
		EXPECT_EQ(0, entityHandle.m_version);
		EXPECT_TRUE(static_cast<bool>(entityHandle));
	}

	TEST_F(CEntityHandleTest, invalidate_handle)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);

		CHandle entityHandle = entity;
		EXPECT_TRUE(static_cast<bool>(entityHandle));
		m_entityManager->DestroyEntity(&entity);
		EXPECT_FALSE(static_cast<bool>(entityHandle));
	}

	TEST_F(CEntityHandleTest, null_entity_from_invalid_handle)
	{
		CHandle handle;
		EXPECT_FALSE(static_cast<bool>(handle));
		CEntity* entity = handle;
		EXPECT_EQ(nullptr, entity);
	}

	TEST_F(CEntityHandleTest, invalid_handle_from_null_entity)
	{
		CEntity* entity = nullptr;
		CHandle handle = entity;
		EXPECT_FALSE(static_cast<bool>(handle));
	}

	TEST_F(CEntityHandleTest, invalid_cast_to_handle_after_destroy)
	{
		CEntity* entity = m_entityManager->CreateEntity();
		EXPECT_NE(nullptr, entity);
		CHandle handle = entity;
		EXPECT_TRUE(static_cast<bool>(handle));

		m_entityManager->DestroyEntity(&entity);

		EXPECT_FALSE(static_cast<bool>(handle));
		entity = handle;
		EXPECT_EQ(nullptr, entity);
	}
}