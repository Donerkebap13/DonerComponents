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

#include <donercomponents/ErrorMessages.h>
#include <donercomponents/CDonerComponentsSystems.h>
#include <donercomponents/gameObject/CGameObject.h>
#include <donercomponents/gameObject/CGameObjectParser.h>
#include <donercomponents/gameObject/CPrefabManager.h>
#include <donercomponents/ErrorMessages.h>
#include <donercomponents/component/CComponent.h>
#include <donercomponents/utils/memory/CMemoryDataProvider.h>
#include <donercomponents/utils/hash/CStrID.h>
#include <donercomponents/component/CComponentFactoryManager.h>

namespace DonerComponents
{
	CGameObjectParser::CGameObjectParser()
		: m_gameObjectManager(*CDonerComponentsSystems::Get()->GetGameObjectManager())
		, m_prefabManager(*CDonerComponentsSystems::Get()->GetPrefabManager())
	{}

	CHandle CGameObjectParser::ParseSceneFromFile(const char* const path)
	{
		return ParseFromFile(path, EParsedGameObjectType::Scene);
	}

	CHandle CGameObjectParser::ParseSceneFromMemory(const unsigned char* jsonStringBuffer, std::size_t size)
	{
		return ParseFromMemory(jsonStringBuffer, size, EParsedGameObjectType::Scene);
	}

	CHandle CGameObjectParser::ParsePrefabFromFile(const char* const path)
	{
		return ParseFromFile(path, EParsedGameObjectType::Prefab);
	}

	CHandle CGameObjectParser::ParsePrefabFromMemory(const unsigned char* jsonStringBuffer, std::size_t size)
	{
		return ParseFromMemory(jsonStringBuffer, size, EParsedGameObjectType::Prefab);
	}

	CHandle CGameObjectParser::ParseSceneFromJson(const char* const jsonStr)
	{
		return ParseFromJson(jsonStr, EParsedGameObjectType::Scene);
	}

	CHandle CGameObjectParser::ParsePrefabFromJson(const char* const jsonStr)
	{
		return ParseFromJson(jsonStr, EParsedGameObjectType::Prefab);
	}

	CHandle CGameObjectParser::ParseFromFile(const char* const path, EParsedGameObjectType type)
	{
		CMemoryDataProvider mdp(path);
		if (!mdp.IsValid())
		{
			DC_ERROR_MSG(EErrorCode::FileNotFound, "error opening %s", path);
			return CHandle();
		}

		std::string zeroTerminatedStr((const char*)mdp.GetBaseData(), mdp.GetSize());
		return ParseFromJson(zeroTerminatedStr.c_str(), type);
	}

	CHandle CGameObjectParser::ParseFromMemory(const unsigned char* jsonStringBuffer, std::size_t size, EParsedGameObjectType type)
	{
		CMemoryDataProvider mdp(jsonStringBuffer, size);
		if (!mdp.IsValid())
		{
			DC_ERROR_MSG(EErrorCode::ReadFromBufferFailed, "Error reading from Buffer");
			return CHandle();
		}
		std::string zeroTerminatedStr((const char*)mdp.GetBaseData(), mdp.GetSize());
		return ParseFromJson(zeroTerminatedStr.c_str(), type);
	}

	CHandle CGameObjectParser::ParseFromJson(const char* const jsonStr, EParsedGameObjectType type)
	{
		rapidjson::Document document;
		rapidjson::Value& root = document.Parse(jsonStr);

		CHandle result;
		if (document.HasParseError())
		{
			rapidjson::ParseErrorCode error = document.GetParseError();
			DC_ERROR_MSG(EErrorCode::JSONError, "Error processing Json: %d", error);
			return result;
		}

		if (root.HasMember("root"))
		{
			rapidjson::Value& rootGameObject = root["root"];

			switch (type)
			{
				case DonerComponents::CGameObjectParser::EParsedGameObjectType::Scene:
				{
					result = ParseGameObject(rootGameObject, nullptr);
					CGameObject* gameObject = result;
					if (gameObject)
					{
						gameObject->Init();
						gameObject->CheckFirstActivation();
					}
				}
				break;
				case DonerComponents::CGameObjectParser::EParsedGameObjectType::Prefab:
				{
					result = ParsePrefab(rootGameObject);
				}
				break;
			}
		}

		return result;
	}

	CHandle CGameObjectParser::ParseGameObject(const rapidjson::Value& gameObjectData, CGameObject* parent)
	{
		CGameObject* gameObject = nullptr;
		if (gameObjectData.HasMember("prefab"))
		{
			gameObject = m_prefabManager.ClonePrefab(CStrID(gameObjectData["prefab"].GetString()), CPrefabManager::ECloneMode::KeepUninitialized);
		}
		else
		{
			gameObject = m_gameObjectManager.CreateGameObject();
		}

		if (!gameObject)
		{
			return CHandle();
		}

		if (gameObjectData.HasMember("name"))
		{
			gameObject->SetName(gameObjectData["name"].GetString());
		}

		ParseOverrideableData(gameObjectData, gameObject);

		if (parent)
		{
			parent->AddChild(gameObject);
		}
		return gameObject;
	}

	void CGameObjectParser::ParseOverrideableData(const rapidjson::Value& gameObjectData, CGameObject* gameObject)
	{
		bool initiallyActive = gameObjectData.HasMember("initially_active") ? gameObjectData["initially_active"].GetBool() : true;
		gameObject->SetIsInitiallyActive(initiallyActive);

		if (gameObjectData.HasMember("tags"))
		{
			ParseTags(gameObjectData["tags"], gameObject);
		}
		if (gameObjectData.HasMember("components"))
		{
			ParseComponents(gameObjectData["components"], gameObject);
		}
		if (gameObjectData.HasMember("children"))
		{
			ParseChildren(gameObjectData["children"], gameObject);
		}
	}

	CHandle CGameObjectParser::ParsePrefab(const rapidjson::Value& gameObjectData)
	{
		CGameObject* prefab = ParseGameObject(gameObjectData, nullptr);
		if (prefab)
		{
			m_prefabManager.RegisterPrefab(CStrID(gameObjectData["name"].GetString()), prefab);
		}
		return prefab;
	}

	bool CGameObjectParser::ParseTags(const rapidjson::Value& tags, CGameObject* gameObject)
	{
		if (tags.IsArray())
		{
			for (const rapidjson::Value& tag : tags.GetArray())
			{
				gameObject->AddTags(tag.GetString());
			}
			return true;
		}
		else if (!tags.IsNull())
		{
			DC_ERROR_MSG(EErrorCode::JSONBadFormat, "Your tags info for gameObject '%s' is bad formatted", gameObject->GetName().c_str());
		}
		return false;
	}

	bool CGameObjectParser::ParseComponents(const rapidjson::Value& components, CGameObject* gameObject)
	{
		if (components.IsArray())
		{
			for (const rapidjson::Value& componentJson : components.GetArray())
			{
				CStrID componentId = CStrID(componentJson["name"].GetString());
				CComponent* component = gameObject->GetComponent(componentId);
				if (!component)
				{
					component = gameObject->AddComponent(componentId);
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
			DC_ERROR_MSG(EErrorCode::JSONBadFormat, "Your components info for gameObject '%s' is bad formatted", gameObject->GetName().c_str());
		}
		return false;
	}

	bool CGameObjectParser::ParseChildren(const rapidjson::Value& children, CGameObject* gameObject)
	{
		if (children.IsArray())
		{
			for (const rapidjson::Value& childJson : children.GetArray())
			{
				CGameObject* existingChild = gameObject->GetChildByName(childJson["name"].GetString());
				if (existingChild)
				{
					ParseOverrideableData(childJson, existingChild);
				}
				else
				{
					ParseGameObject(childJson, gameObject);
				}
			}
			return true;
		}
		else if (!children.IsNull())
		{
			DC_ERROR_MSG(EErrorCode::JSONBadFormat, "Your children info for gameObject '%s' is bad formatted", gameObject->GetName().c_str());
		}
		return false;
	}
}
