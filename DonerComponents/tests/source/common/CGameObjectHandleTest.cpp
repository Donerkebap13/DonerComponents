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
#include <donercomponents/handle/CHandle.h>
#include <donercomponents/component/CComponentFactoryManager.h>

#include <gtest/gtest.h>
#include <unordered_map>

namespace DonerComponents
{
	namespace VersionableFactoryTestInternal
	{
		class Foo : public CFactoryElement
		{};
	}

	class CGameObjectHandleTest : public ::testing::Test
	{
	public:
		CGameObjectHandleTest()
			: m_gameObjectManager(nullptr)
			, m_componentFactoryManager(nullptr)
		{
			CDonerComponentsSystems& systems = CDonerComponentsSystems::CreateInstance()->Init();
			m_gameObjectManager = systems.GetGameObjectManager();
			m_componentFactoryManager = systems.GetComponentFactoryManager();
		}

		~CGameObjectHandleTest()
		{
			CDonerComponentsSystems::DestroyInstance();
		}

		CGameObjectManager *m_gameObjectManager;
		CComponentFactoryManager *m_componentFactoryManager;
	};

	TEST_F(CGameObjectHandleTest, new_handle_is_invalid)
	{
		CHandle handle;
		EXPECT_FALSE(handle);
		EXPECT_EQ(0, handle.m_componentIdx);
		EXPECT_EQ(0, handle.m_elementPosition);
		EXPECT_EQ(0, handle.m_elementType);
		EXPECT_EQ(0, handle.m_version);

		CGameObject* gameObject = handle;
		EXPECT_EQ(nullptr, gameObject);
		CComponent* component = handle;
		EXPECT_EQ(nullptr, component);
	}

	TEST_F(CGameObjectHandleTest, get_new_gameObject_pointer)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		EXPECT_NE(nullptr, gameObject);
	}

	TEST_F(CGameObjectHandleTest, get_new_gameObject_as_handle)
	{
		CHandle gameObjectHandle = m_gameObjectManager->CreateGameObject();
		EXPECT_EQ(CHandle::EElementType::GameObject, gameObjectHandle.m_elementType);
		EXPECT_EQ(0, gameObjectHandle.m_componentIdx);
		EXPECT_EQ(0, gameObjectHandle.m_elementPosition);
		EXPECT_EQ(0, gameObjectHandle.m_version);
		EXPECT_TRUE(static_cast<bool>(gameObjectHandle));

		CHandle gameObjectHandle2 = m_gameObjectManager->CreateGameObject();
		EXPECT_EQ(CHandle::EElementType::GameObject, gameObjectHandle2.m_elementType);
		EXPECT_EQ(0, gameObjectHandle2.m_componentIdx);
		EXPECT_EQ(1, gameObjectHandle2.m_elementPosition);
		EXPECT_EQ(0, gameObjectHandle2.m_version);
		EXPECT_TRUE(static_cast<bool>(gameObjectHandle));
	}

	TEST_F(CGameObjectHandleTest, cast_handle_to_gameObject)
	{
		CHandle gameObjectHandle = m_gameObjectManager->CreateGameObject();

		CGameObject* gameObject = gameObjectHandle;
		EXPECT_NE(nullptr, gameObject);

		int pos = m_gameObjectManager->GetPositionForElement(gameObject);
		EXPECT_EQ(pos, gameObjectHandle.m_elementPosition);
		EXPECT_EQ(gameObject->GetVersion(), gameObjectHandle.m_version);
		CGameObject* gameObject2 = m_gameObjectManager->GetElementByIdxAndVersion(pos, gameObject->GetVersion());
		EXPECT_EQ(gameObject2, gameObject);
	}

	TEST_F(CGameObjectHandleTest, cast_gameObject_to_handle)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();

		CHandle gameObjectHandle = gameObject;
		EXPECT_EQ(CHandle::EElementType::GameObject, gameObjectHandle.m_elementType);
		EXPECT_EQ(0, gameObjectHandle.m_componentIdx);
		EXPECT_EQ(0, gameObjectHandle.m_elementPosition);
		EXPECT_EQ(0, gameObjectHandle.m_version);
		EXPECT_TRUE(static_cast<bool>(gameObjectHandle));
	}

	TEST_F(CGameObjectHandleTest, invalidate_handle)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();

		CHandle gameObjectHandle = gameObject;
		EXPECT_TRUE(static_cast<bool>(gameObjectHandle));

		gameObject->Destroy();
		m_gameObjectManager->ExecuteScheduledDestroys();

		EXPECT_FALSE(static_cast<bool>(gameObjectHandle));
	}

	TEST_F(CGameObjectHandleTest, null_gameObject_from_invalid_handle)
	{
		CHandle handle;
		EXPECT_FALSE(static_cast<bool>(handle));
		CGameObject* gameObject = handle;
		EXPECT_EQ(nullptr, gameObject);
	}

	TEST_F(CGameObjectHandleTest, invalid_handle_from_null_gameObject)
	{
		CGameObject* gameObject = nullptr;
		CHandle handle = gameObject;
		EXPECT_FALSE(static_cast<bool>(handle));
	}

	TEST_F(CGameObjectHandleTest, invalid_cast_to_handle_after_destroy)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		CHandle handle = gameObject;

		gameObject->Destroy();
		m_gameObjectManager->ExecuteScheduledDestroys();

		EXPECT_FALSE(static_cast<bool>(handle));
		gameObject = handle;
		EXPECT_EQ(nullptr, gameObject);
	}

	TEST_F(CGameObjectHandleTest, destroy_gameObject_through_handle)
	{
		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		CHandle handle = gameObject;

		handle.Destroy();
		EXPECT_TRUE(gameObject->IsDestroyed());
		EXPECT_FALSE(static_cast<bool>(handle));
	}

	TEST_F(CGameObjectHandleTest, handle_as_valid_map_index)
	{
		static constexpr int testValue = 1337;

		CGameObject* gameObject = m_gameObjectManager->CreateGameObject();
		CHandle handle = gameObject;

		std::unordered_map<CHandle, int> map;
		map[handle] = testValue;

		auto it = map.find(handle);
		ASSERT_TRUE(it != map.end());

		int value = it->second;
		ASSERT_EQ(testValue, value);
	}
}