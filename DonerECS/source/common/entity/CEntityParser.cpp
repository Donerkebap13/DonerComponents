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

#include <donerecs/ErrorMessages.h>
#include <donerecs/CDonerECSSystems.h>
#include <donerecs/entity/CEntity.h>
#include <donerecs/entity/CEntityParser.h>
#include <donerecs/entity/CPrefabManager.h>
#include <donerecs/ErrorMessages.h>
#include <donerecs/component/CComponent.h>
#include <donerecs/utils/memory/CMemoryDataProvider.h>
#include <donerecs/utils/hash/CStrID.h>
#include <donerecs/component/CComponentFactoryManager.h>

#include <donerecs/json/json.h>

namespace DonerECS
{
	CEntityParser::CEntityParser()
		: m_entityManager(*CDonerECSSystems::Get()->GetEntityManager())
		, m_prefabManager(*CDonerECSSystems::Get()->GetPrefabManager())
	{}

	CHandle CEntityParser::ParseSceneFromFile(const char* const path)
	{
		return ParseFromFile(path, EParsedEntityType::Scene);
	}

	CHandle CEntityParser::ParseSceneFromMemory(const unsigned char* jsonStringBuffer, std::size_t size)
	{
		return ParseFromMemory(jsonStringBuffer, size, EParsedEntityType::Scene);
	}

	CHandle CEntityParser::ParsePrefabFromFile(const char* const path)
	{
		return ParseFromFile(path, EParsedEntityType::Prefab);
	}

	CHandle CEntityParser::ParsePrefabFromMemory(const unsigned char* jsonStringBuffer, std::size_t size)
	{
		return ParseFromMemory(jsonStringBuffer, size, EParsedEntityType::Prefab);
	}

	CHandle CEntityParser::ParseSceneFromJson(const char* const jsonStr)
	{
		return ParseFromJson(jsonStr, EParsedEntityType::Scene);
	}

	CHandle CEntityParser::ParsePrefabFromJson(const char* const jsonStr)
	{
		return ParseFromJson(jsonStr, EParsedEntityType::Prefab);
	}

	CHandle CEntityParser::ParseFromFile(const char* const path, EParsedEntityType type)
	{
		CMemoryDataProvider mdp(path);
		if (!mdp.IsValid())
		{
			DECS_ERROR_MSG(EErrorCode::FileNotFound, "error opening %s", path);
			return CHandle();
		}

		return ParseFromJson((const char*)mdp.GetBaseData(), type);
	}

	CHandle CEntityParser::ParseFromMemory(const unsigned char* jsonStringBuffer, std::size_t size, EParsedEntityType type)
	{
		CMemoryDataProvider mdp(jsonStringBuffer, size);
		if (!mdp.IsValid())
		{
			DECS_ERROR_MSG(EErrorCode::ReadFromBufferFailed, "Error reading from Buffer");
			return CHandle();
		}

		return ParseFromJson((const char*)mdp.GetBaseData(), type);
	}

	CHandle CEntityParser::ParseFromJson(const char* const jsonStr, EParsedEntityType type)
	{
		CHandle result;
		Json::Value jsonValue;
		Json::Reader reader;
		bool parsingSuccessful = reader.parse(jsonStr, jsonValue);
		if (!parsingSuccessful)
		{
			std::string error = reader.getFormattedErrorMessages();
			DECS_ERROR_MSG(EErrorCode::JSONError, "Error processing Json: %s", error.c_str());
			return result;
		}

		Json::Value& rootEntity = jsonValue["root"];

		switch (type)
		{
			case DonerECS::CEntityParser::EParsedEntityType::Scene:
			{
				result = ParseEntity(rootEntity, nullptr);
				CEntity* entity = result;
				if (entity)
				{
					entity->Init();
					entity->CheckFirstActivation();
				}
			}
			break;
			case DonerECS::CEntityParser::EParsedEntityType::Prefab:
			{
				result = ParsePrefab(rootEntity);
			}
			break;
		}

		return result;
	}

	CHandle CEntityParser::ParseEntity(Json::Value& entityData, CEntity* parent)
	{
		CEntity* entity = nullptr;
		if (!entityData["prefab"].empty())
		{
			entity = m_prefabManager.ClonePrefab(CStrID(entityData["prefab"].asCString()), CPrefabManager::ECloneMode::KeepUninitialized);
		}
		else
		{
			entity = m_entityManager.CreateEntity();
		}

		if (!entity)
		{
			return CHandle();
		}

		if (!entityData["name"].empty())
		{
			entity->SetName(entityData["name"].asCString());
		}

		ParseOverrideableData(entityData, entity);

		if (parent)
		{
			parent->AddChild(entity);
		}
		return entity;
	}

	void CEntityParser::ParseOverrideableData(Json::Value& entityData, CEntity* entity)
	{
		bool initiallyActive = !entityData["initially_active"].empty() ? entityData["initially_active"].asBool() : true;
		entity->SetIsInitiallyActive(initiallyActive);

		ParseTags(entityData["tags"], entity);
		ParseComponents(entityData["components"], entity);
		ParseChildren(entityData["children"], entity);
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
			for (Json::ArrayIndex i = 0; i < tags.size(); ++i)
			{
				Json::Value& tag = tags[i];
				entity->AddTags(tag.asCString());
			}
			return true;
		}
		else if (tags.type() != Json::nullValue)
		{
			DECS_ERROR_MSG(EErrorCode::JSONBadFormat, "Your tags info for entity '%s' is bad formatted", entity->GetName().c_str());
		}
		return false;
	}

	bool CEntityParser::ParseComponents(Json::Value& components, CEntity* entity)
	{
		if (components.type() == Json::arrayValue)
		{
			for (Json::ArrayIndex i = 0; i < components.size(); ++i)
			{
				Json::Value& componentJson = components[i];
				CStrID componentId = CStrID(componentJson["name"].asCString());
				CComponent* component = entity->GetComponent(componentId);
				if (!component)
				{
					component = entity->AddComponent(componentId);
				}
				if (component)
				{
					bool initiallyActive = !componentJson["initially_active"].empty() ? componentJson["initially_active"].asBool() : true;
					component->SetIsInitiallyActive(initiallyActive);
					component->ParseAtts(componentJson);
				}
				else
				{
					return false;
				}
			}
			return true;
		}
		else if (components.type() != Json::nullValue)
		{
			DECS_ERROR_MSG(EErrorCode::JSONBadFormat, "Your components info for entity '%s' is bad formatted", entity->GetName().c_str());
		}
		return false;
	}

	bool CEntityParser::ParseChildren(Json::Value& children, CEntity* entity)
	{
		if (children.type() == Json::arrayValue)
		{
			for (Json::ArrayIndex i = 0; i < children.size(); ++i)
			{
				Json::Value& childJson = children[i];

				CEntity* existingChild = entity->GetChildByName(childJson["name"].asString());
				if (existingChild)
				{
					ParseOverrideableData(childJson, existingChild);
				}
				else
				{
					ParseEntity(childJson, entity);
				}
			}
			return true;
		}
		else if (children.type() != Json::nullValue)
		{
			DECS_ERROR_MSG(EErrorCode::JSONBadFormat, "Your children info for entity '%s' is bad formatted", entity->GetName().c_str());
		}
		return false;
	}
}
