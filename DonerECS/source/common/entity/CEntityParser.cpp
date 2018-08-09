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

		std::string zeroTerminatedStr((const char*)mdp.GetBaseData(), mdp.GetSize());
		return ParseFromJson(zeroTerminatedStr.c_str(), type);
	}

	CHandle CEntityParser::ParseFromMemory(const unsigned char* jsonStringBuffer, std::size_t size, EParsedEntityType type)
	{
		CMemoryDataProvider mdp(jsonStringBuffer, size);
		if (!mdp.IsValid())
		{
			DECS_ERROR_MSG(EErrorCode::ReadFromBufferFailed, "Error reading from Buffer");
			return CHandle();
		}
		std::string zeroTerminatedStr((const char*)mdp.GetBaseData(), mdp.GetSize());
		return ParseFromJson(zeroTerminatedStr.c_str(), type);
	}

	CHandle CEntityParser::ParseFromJson(const char* const jsonStr, EParsedEntityType type)
	{
		rapidjson::Document document;
		rapidjson::Value& root = document.Parse(jsonStr);

		CHandle result;
		if (document.HasParseError())
		{
			rapidjson::ParseErrorCode error = document.GetParseError();
			DECS_ERROR_MSG(EErrorCode::JSONError, "Error processing Json: %d", error);
			return result;
		}

		if (root.HasMember("root"))
		{
			rapidjson::Value& rootEntity = root["root"];

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
		}

		return result;
	}

	CHandle CEntityParser::ParseEntity(const rapidjson::Value& entityData, CEntity* parent)
	{
		CEntity* entity = nullptr;
		if (entityData.HasMember("prefab"))
		{
			entity = m_prefabManager.ClonePrefab(CStrID(entityData["prefab"].GetString()), CPrefabManager::ECloneMode::KeepUninitialized);
		}
		else
		{
			entity = m_entityManager.CreateEntity();
		}

		if (!entity)
		{
			return CHandle();
		}

		if (entityData.HasMember("name"))
		{
			entity->SetName(entityData["name"].GetString());
		}

		ParseOverrideableData(entityData, entity);

		if (parent)
		{
			parent->AddChild(entity);
		}
		return entity;
	}

	void CEntityParser::ParseOverrideableData(const rapidjson::Value& entityData, CEntity* entity)
	{
		bool initiallyActive = entityData.HasMember("initially_active") ? entityData["initially_active"].GetBool() : true;
		entity->SetIsInitiallyActive(initiallyActive);

		if (entityData.HasMember("tags"))
		{
			ParseTags(entityData["tags"], entity);
		}
		if (entityData.HasMember("components"))
		{
			ParseComponents(entityData["components"], entity);
		}
		if (entityData.HasMember("children"))
		{
			ParseChildren(entityData["children"], entity);
		}
	}

	CHandle CEntityParser::ParsePrefab(const rapidjson::Value& entityData)
	{
		CEntity* prefab = ParseEntity(entityData, nullptr);
		if (prefab)
		{
			m_prefabManager.RegisterPrefab(CStrID(entityData["name"].GetString()), prefab);
		}
		return prefab;
	}

	bool CEntityParser::ParseTags(const rapidjson::Value& tags, CEntity* entity)
	{
		if (tags.IsArray())
		{
			for (const rapidjson::Value& tag : tags.GetArray())
			{
				entity->AddTags(tag.GetString());
			}
			return true;
		}
		else if (!tags.IsNull())
		{
			DECS_ERROR_MSG(EErrorCode::JSONBadFormat, "Your tags info for entity '%s' is bad formatted", entity->GetName().c_str());
		}
		return false;
	}

	bool CEntityParser::ParseComponents(const rapidjson::Value& components, CEntity* entity)
	{
		if (components.IsArray())
		{
			for (const rapidjson::Value& componentJson : components.GetArray())
			{
				CStrID componentId = CStrID(componentJson["name"].GetString());
				CComponent* component = entity->GetComponent(componentId);
				if (!component)
				{
					component = entity->AddComponent(componentId);
				}
				if (component)
				{
					bool initiallyActive = componentJson.HasMember("initially_active") ? componentJson["initially_active"].GetBool() : true;
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
		else if (!components.IsNull())
		{
			DECS_ERROR_MSG(EErrorCode::JSONBadFormat, "Your components info for entity '%s' is bad formatted", entity->GetName().c_str());
		}
		return false;
	}

	bool CEntityParser::ParseChildren(const rapidjson::Value& children, CEntity* entity)
	{
		if (children.IsArray())
		{
			for (const rapidjson::Value& childJson : children.GetArray())
			{
				CEntity* existingChild = entity->GetChildByName(childJson["name"].GetString());
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
		else if (!children.IsNull())
		{
			DECS_ERROR_MSG(EErrorCode::JSONBadFormat, "Your children info for entity '%s' is bad formatted", entity->GetName().c_str());
		}
		return false;
	}
}
