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

#include <donerecs/component/CComponent.h>
#include <donerecs/component/CComponentFactoryManager.h>
#include <donerecs/handle/CHandle.h>

#include <gtest/gtest.h>

namespace DonerECS
{
	namespace ComponentTestInternal
	{
		const int LOOP_COUNT = 5;

		class CCompFoo : public CComponent
		{
		public:
			CCompFoo()
				: m_initCount(0)
				, m_updateCount(0)
				, m_destroyCount(0)
				, m_activateCount(0)
				, m_deactivateCount(0)
			{}

			void DoInit() override { ++m_initCount; }
			void DoUpdate(float /*dt*/) override { ++m_updateCount; }
			void DoDestroy() override { ++m_destroyCount; }
			void DoActivate() override { ++m_activateCount; }
			void DoDeactivate() override { ++m_deactivateCount; }

			int m_initCount;
			int m_updateCount;
			int m_destroyCount;
			int m_activateCount;
			int m_deactivateCount;
		};
        
        class CCompBar: public CComponent {};
	}

	class CComponentTest : public ::testing::Test
	{
	public:
		CComponentTest()
			: m_componentFactoryManager(CComponentFactoryManager::CreateInstance())
		{
			ADD_COMPONENT_FACTORY("foo", ComponentTestInternal::CCompFoo, 2);
		}

		~CComponentTest()
		{
			CComponentFactoryManager::DestroyInstance();
		}

		CComponentFactoryManager *m_componentFactoryManager;
	};

	TEST_F(CComponentTest, component_not_initalized_on_creation)
	{
		ComponentTestInternal::CCompFoo* component = static_cast<ComponentTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<ComponentTestInternal::CCompFoo>());
		EXPECT_NE(nullptr, component);

		EXPECT_FALSE(component->IsInitialized());
		EXPECT_FALSE(component->IsDestroyed());
		EXPECT_FALSE(component->IsActive());

		EXPECT_EQ(0, component->m_initCount);
		EXPECT_EQ(0, component->m_destroyCount);
		EXPECT_EQ(0, component->m_updateCount);
		EXPECT_EQ(0, component->m_activateCount);
		EXPECT_EQ(0, component->m_deactivateCount);
	}

	TEST_F(CComponentTest, component_call_doInit)
	{
		ComponentTestInternal::CCompFoo* component = static_cast<ComponentTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<ComponentTestInternal::CCompFoo>());
		EXPECT_NE(nullptr, component);

		EXPECT_FALSE(component->IsInitialized());

		component->Init();
		EXPECT_TRUE(component->IsInitialized());
		EXPECT_EQ(1, component->m_initCount);
	}

	TEST_F(CComponentTest, component_init_called_only_once)
	{
		ComponentTestInternal::CCompFoo* component = static_cast<ComponentTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<ComponentTestInternal::CCompFoo>());
		EXPECT_NE(nullptr, component);

		EXPECT_FALSE(component->IsInitialized());

		for (int i = 0; i < ComponentTestInternal::LOOP_COUNT; ++i)
		{
			component->Init();
			EXPECT_TRUE(component->IsInitialized());
			EXPECT_EQ(1, component->m_initCount);
		}
	}

	TEST_F(CComponentTest, component_call_doActivate)
	{
		ComponentTestInternal::CCompFoo* component = static_cast<ComponentTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<ComponentTestInternal::CCompFoo>());
		EXPECT_NE(nullptr, component);

		component->Init();

		EXPECT_FALSE(component->IsActive());
		component->Activate();
		EXPECT_TRUE(component->IsActive());
		EXPECT_EQ(1, component->m_activateCount);
	}

	TEST_F(CComponentTest, component_doActivate_called_only_once)
	{
		ComponentTestInternal::CCompFoo* component = static_cast<ComponentTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<ComponentTestInternal::CCompFoo>());
		EXPECT_NE(nullptr, component);

		component->Init();

		for (int i = 0; i < ComponentTestInternal::LOOP_COUNT; ++i)
		{
			component->Activate();
			EXPECT_TRUE(component->IsActive());
			EXPECT_EQ(1, component->m_activateCount);
		}
	}

	TEST_F(CComponentTest, component_dont_activate_itself_if_uninitialized)
	{
		ComponentTestInternal::CCompFoo* component = static_cast<ComponentTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<ComponentTestInternal::CCompFoo>());
		EXPECT_NE(nullptr, component);

		EXPECT_FALSE(component->IsInitialized());

		EXPECT_FALSE(component->IsActive());
		component->Activate();
		EXPECT_FALSE(component->IsActive());
		EXPECT_EQ(0, component->m_activateCount);
	}

	TEST_F(CComponentTest, component_call_doDeactivate)
	{
		ComponentTestInternal::CCompFoo* component = static_cast<ComponentTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<ComponentTestInternal::CCompFoo>());
		EXPECT_NE(nullptr, component);

		component->Init();

		EXPECT_FALSE(component->IsActive());
		component->Activate();
		EXPECT_TRUE(component->IsActive());

		component->Deactivate();
		EXPECT_FALSE(component->IsActive());
		EXPECT_EQ(1, component->m_deactivateCount);
	}

	TEST_F(CComponentTest, component_doDectivate_called_only_once)
	{
		ComponentTestInternal::CCompFoo* component = static_cast<ComponentTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<ComponentTestInternal::CCompFoo>());
		EXPECT_NE(nullptr, component);

		EXPECT_FALSE(component->IsInitialized());
		component->Init();

		component->Activate();

		for (int i = 0; i < ComponentTestInternal::LOOP_COUNT; ++i)
		{
			component->Deactivate();
			EXPECT_FALSE(component->IsActive());
			EXPECT_EQ(1, component->m_deactivateCount);
		}
	}

	TEST_F(CComponentTest, component_initially_disabled_dont_call_doDeactivate)
	{
		ComponentTestInternal::CCompFoo* component = static_cast<ComponentTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<ComponentTestInternal::CCompFoo>());
		EXPECT_NE(nullptr, component);

		EXPECT_FALSE(component->IsInitialized());
		component->Init();

		for (int i = 0; i < ComponentTestInternal::LOOP_COUNT; ++i)
		{
			component->Deactivate();
			EXPECT_FALSE(component->IsActive());
			EXPECT_EQ(0, component->m_deactivateCount);
		}
	}

	TEST_F(CComponentTest, component_dont_deactivate_itself_if_uninitialized)
	{
		ComponentTestInternal::CCompFoo* component = static_cast<ComponentTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<ComponentTestInternal::CCompFoo>());
		EXPECT_NE(nullptr, component);

		EXPECT_FALSE(component->IsInitialized());

		EXPECT_FALSE(component->IsActive());
		component->Activate();
		EXPECT_FALSE(component->IsActive());

		component->Deactivate();
		EXPECT_EQ(0, component->m_deactivateCount);
	}

	TEST_F(CComponentTest, component_call_doDestroy)
	{
		ComponentTestInternal::CCompFoo* component = static_cast<ComponentTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<ComponentTestInternal::CCompFoo>());
		EXPECT_NE(nullptr, component);

		EXPECT_FALSE(component->IsInitialized());

		component->Init();
		component->Destroy();
		EXPECT_TRUE(component->IsDestroyed());
		EXPECT_EQ(1, component->m_destroyCount);
	}

	TEST_F(CComponentTest, component_call_doDestroy_called_only_once)
	{
		ComponentTestInternal::CCompFoo* component = static_cast<ComponentTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<ComponentTestInternal::CCompFoo>());
		EXPECT_NE(nullptr, component);

		EXPECT_FALSE(component->IsInitialized());

		component->Init();

		for (int i = 0; i < ComponentTestInternal::LOOP_COUNT; ++i)
		{
			component->Destroy();
			EXPECT_TRUE(component->IsDestroyed());
			EXPECT_EQ(1, component->m_destroyCount);
		}
	}

	TEST_F(CComponentTest, component_dont_destroy_itself_if_uninitialized)
	{
		ComponentTestInternal::CCompFoo* component = static_cast<ComponentTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<ComponentTestInternal::CCompFoo>());
		EXPECT_NE(nullptr, component);

		EXPECT_FALSE(component->IsInitialized());

		component->Destroy();
		EXPECT_FALSE(component->IsDestroyed());
		EXPECT_EQ(0, component->m_destroyCount);
	}

	TEST_F(CComponentTest, component_call_doUpdate)
	{
		ComponentTestInternal::CCompFoo* component = static_cast<ComponentTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<ComponentTestInternal::CCompFoo>());
		EXPECT_NE(nullptr, component);

		EXPECT_FALSE(component->IsInitialized());

		component->Init();
		component->Activate();

		for (int i = 1; i < ComponentTestInternal::LOOP_COUNT; ++i)
		{
			component->Update(0.f);
			EXPECT_EQ(i, component->m_updateCount);
		}
	}

	TEST_F(CComponentTest, component_dont_update_if_inactive)
	{
		ComponentTestInternal::CCompFoo* component = static_cast<ComponentTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<ComponentTestInternal::CCompFoo>());
		EXPECT_NE(nullptr, component);

		EXPECT_FALSE(component->IsInitialized());

		component->Init();

		for (int i = 1; i < ComponentTestInternal::LOOP_COUNT; ++i)
		{
			component->Update(0.f);
			EXPECT_EQ(0, component->m_updateCount);
		}
	}

	TEST_F(CComponentTest, component_copy)
	{
		ComponentTestInternal::CCompFoo* component1 = static_cast<ComponentTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<ComponentTestInternal::CCompFoo>());
		EXPECT_NE(nullptr, component1);
		component1->DoInit();
		EXPECT_EQ(1, component1->m_initCount);

		ComponentTestInternal::CCompFoo* component2 = static_cast<ComponentTestInternal::CCompFoo*>(
			m_componentFactoryManager->CreateComponent<ComponentTestInternal::CCompFoo>());
		EXPECT_NE(nullptr, component2);
		EXPECT_EQ(0, component2->m_initCount);

		*component2 = *component1;
		EXPECT_EQ(1, component2->m_initCount);
	}
    
    TEST_F(CComponentTest, register_factory_twice_fails)
    {
        bool success = ADD_COMPONENT_FACTORY("foo", ComponentTestInternal::CCompFoo, 2);
        EXPECT_FALSE(success);
    }
    
    TEST_F(CComponentTest, create_unregistered_componet_fails)
    {
        ComponentTestInternal::CCompBar* component = static_cast<ComponentTestInternal::CCompBar*>(
            m_componentFactoryManager->CreateComponent<ComponentTestInternal::CCompBar>());
        EXPECT_EQ(nullptr, component);
    }
    
    TEST_F(CComponentTest, factoryManager_updates_components)
    {
        ComponentTestInternal::CCompFoo* component = static_cast<ComponentTestInternal::CCompFoo*>(
            m_componentFactoryManager->CreateComponent<ComponentTestInternal::CCompFoo>());
        EXPECT_NE(nullptr, component);
        
        EXPECT_FALSE(component->IsInitialized());
        
        component->Init();
        component->Activate();
        
        EXPECT_EQ(0, component->m_updateCount);
        m_componentFactoryManager->Update(0.f);
        EXPECT_EQ(1, component->m_updateCount);
    }
    
    TEST_F(CComponentTest, destroy_unregistered_component_fails)
    {
        CComponent* component = new ComponentTestInternal::CCompFoo();
        EXPECT_NE(nullptr, component);
        
        component->Init();
        component->Activate();
        EXPECT_FALSE(component->IsDestroyed());
        m_componentFactoryManager->DestroyComponent(&component);
        EXPECT_FALSE(component->IsDestroyed());
        
        delete component;
    }
}
