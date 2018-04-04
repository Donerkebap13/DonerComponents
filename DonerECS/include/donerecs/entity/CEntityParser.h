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

#pragma once

#include <donerecs/common/CFactory.h>
#include <donerecs/entity/CEntity.h>

namespace DonerECS
{
	class CEntity;
	class CEntityManager;
	class CPrefabManager;

	namespace Json
	{
		class Value;
	}

	class CEntityParser
	{
	public:
		enum class EParsedEntityType { Scene, Prefab };
		
		CEntityParser();

		CHandle ParseSceneFromFile(const char* const path);
		CHandle ParseSceneFromMemory(const unsigned char* jsonStringBuffer, std::size_t size);

		CHandle ParsePrefabFromFile(const char* const path);
		CHandle ParsePrefabFromMemory(const unsigned char* jsonStringBuffer, std::size_t size);

		CHandle ParseSceneFromJson(const char* const jsonStr);
		CHandle ParsePrefabFromJson(const char* const jsonStr);

	private:
		CHandle ParseFromFile(const char* const path, EParsedEntityType type);
		CHandle ParseFromMemory(const unsigned char* jsonStringBuffer, std::size_t size, EParsedEntityType type);
		CHandle ParseFromJson(const char* const jsonStr, EParsedEntityType type);

		CHandle ParseEntity(Json::Value& entityData, CEntity* parent);
		CHandle ParsePrefab(Json::Value& entityData);

		void ParseOverrideableData(Json::Value& entityData, CEntity* entity);

		bool ParseTags(Json::Value& tags, CEntity* entity);
		bool ParseComponents(Json::Value& components, CEntity* entity);
		bool ParseChildren(Json::Value& children, CEntity* entity);

		CEntityManager& m_entityManager;
		CPrefabManager& m_prefabManager;
	};
}
