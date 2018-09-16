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

#include <donercomponents/common/CFactory.h>
#include <donercomponents/gameObject/CGameObject.h>

#include <rapidjson/document.h>

namespace DonerComponents
{
	class CGameObject;
	class CGameObjectManager;
	class CPrefabManager;

	namespace Json
	{
		class Value;
	}

	class CGameObjectParser
	{
	public:
		enum class EParsedGameObjectType { Scene, Prefab };
		
		CGameObjectParser();

		CHandle ParseSceneFromFile(const char* const path);
		CHandle ParseSceneFromMemory(const unsigned char* jsonStringBuffer, std::size_t size);

		CHandle ParsePrefabFromFile(const char* const path);
		CHandle ParsePrefabFromMemory(const unsigned char* jsonStringBuffer, std::size_t size);

		CHandle ParseSceneFromJson(const char* const jsonStr);
		CHandle ParsePrefabFromJson(const char* const jsonStr);

	private:
		CHandle ParseFromFile(const char* const path, EParsedGameObjectType type);
		CHandle ParseFromMemory(const unsigned char* jsonStringBuffer, std::size_t size, EParsedGameObjectType type);
		CHandle ParseFromJson(const char* const jsonStr, EParsedGameObjectType type);

		CHandle ParseGameObject(const rapidjson::Value& gameObjectData, CGameObject* parent);
		CHandle ParsePrefab(const rapidjson::Value& gameObjectData);

		void ParseOverrideableData(const rapidjson::Value& gameObjectData, CGameObject* gameObject);

		bool ParseTags(const rapidjson::Value& tags, CGameObject* gameObject);
		bool ParseComponents(const rapidjson::Value& components, CGameObject* gameObject);
		bool ParseChildren(const rapidjson::Value& children, CGameObject* gameObject);

		CGameObjectManager& m_gameObjectManager;
		CPrefabManager& m_prefabManager;
	};
}
