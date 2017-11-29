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
#include <donerecs/entity/CEntityParser.h>
#include <donerecs/entity/CEntityManager.h>
#include <donerecs/entity/CPrefabManager.h>
#include <donerecs/component/CComponent.h>
#include <donerecs/utils/memory/CMemoryDataProvider.h>
#include <donerecs/utils/hash/CStrID.h>
#include <donerecs/component/CComponentFactoryManager.h>

#include <donerecs/json/json.h>

namespace DonerECS
{
	CEntityParser::CEntityParser()
		: m_entityManager(*CEntityManager::Get())
		, m_prefabManager(*CPrefabManager::Get())
	{}

	CHandle CEntityParser::ParseScene(const char* const path)
	{
		CMemoryDataProvider mdp(path);
		if (!mdp.IsValid())
		{
			printf("CEntityParser::error opening %s!\n", path);
			return CHandle();
		}

		return ParseSceneFromJson((const char*)mdp.GetBaseData());
	}

	CHandle CEntityParser::ParseSceneFromMemory(void* jsonStringBuffer, std::size_t size)
	{
		CMemoryDataProvider mdp((const unsigned char*)jsonStringBuffer, size);
		if (!mdp.IsValid())
		{
			printf("CEntityParser::error reading from Buffer!\n");
			return CHandle();
		}

		return ParseSceneFromJson((const char*)mdp.GetBaseData());
	}

	CHandle CEntityParser::ParseSceneFromJson(const char* const jsonStr)
	{
		CHandle result;
		Json::Value jsonValue;
		Json::Reader reader;
		bool parsingSuccessful = reader.parse(jsonStr, jsonValue);
		if (!parsingSuccessful)
		{
			std::string error = reader.getFormattedErrorMessages();
			printf("CEntityParser::Error processing Json: %s\n", error.c_str());
			return result;
		}

		Json::Value& rootEntity = jsonValue["root"];
		if (rootEntity.type() == Json::objectValue)
		{
			if (rootEntity["type"].asString() == "entity")
			{
				result = ParseEntity(rootEntity, nullptr);
				CEntity* entity = result;
				if (entity)
				{
					entity->Init();
					entity->CheckFirstActivation();
				}
			}
			else if (rootEntity["type"].asString() == "prefab")
			{
				result = ParsePrefab(rootEntity);
			}
		}

		return result;
	}

	CHandle CEntityParser::ParseEntity(Json::Value& entityData, CEntity* parent)
	{
		CEntity* entity = nullptr;
		if (!entityData["prefab"].empty())
		{
			entity = m_prefabManager.ClonePrefab(CStrID(entityData["prefab"].asCString()));
		}
		else
		{
			entity = m_entityManager.GetNewElement();
		}

		if (!entity)
		{
			// ERROR
			return CHandle();
		}

		if (!entityData["name"].empty())
		{
			entity->SetName(entityData["name"].asCString());
		}

		bool initiallyActive = !entityData["initiallyActive"].empty() ? entityData["initiallyActive"].asBool() : true;
		entity->SetIsInitiallyActive(initiallyActive);

		ParseTags(entityData["tags"], entity);
		ParseComponents(entityData["components"], entity);
		ParseChildren(entityData["children"], entity);

		if (parent)
		{
			parent->AddChild(entity);
		}
		return entity;
	}

	CHandle CEntityParser::ParsePrefab(Json::Value& entityData)
	{
		CEntity* prefab = ParseEntity(entityData, nullptr);
		if (prefab)
		{
			m_prefabManager.RegisterPrefab(CStrID(entityData["name"].asCString()), prefab);
		}
		return prefab;
	}

	bool CEntityParser::ParseTags(Json::Value& tags, CEntity* entity)
	{
		if (tags.type() == Json::arrayValue)
		{
			for (size_t i = 0; i < tags.size(); ++i)
			{
				Json::Value& tag = tags[i];
				entity->AddTags(tag.asCString());
			}
			return true;
		}
		return false;
	}

	bool CEntityParser::ParseComponents(Json::Value& components, CEntity* entity)
	{
		if (components.type() == Json::arrayValue)
		{
			for (size_t i = 0; i < components.size(); ++i)
			{
				Json::Value& componentJson = components[i];
				CStrID componentId = CStrID(componentJson["type"].asCString());
				CComponent* component = entity->GetComponent(componentId);
				if (!component)
				{
					component = entity->AddComponent(componentId);
				}
				if (component)
				{
					bool initiallyActive = !componentJson["initiallyActive"].empty() ? componentJson["initiallyActive"].asBool() : true;
					component->SetIsInitiallyActive(initiallyActive);
					component->ParseAtts(componentJson);
				}
				else
				{
					// ERROR
					return false;
				}
			}
			return true;
		}
		return false;
	}

	bool CEntityParser::ParseChildren(Json::Value& children, CEntity* entity)
	{
		if (children.type() == Json::arrayValue)
		{
			for (size_t i = 0; i < children.size(); ++i)
			{
				Json::Value& childJson = children[i];
				ParseEntity(childJson, entity);
			}
			return true;
		}
		return false;
	}
}
