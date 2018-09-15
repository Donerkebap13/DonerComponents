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

#include <cassert>
#include <cstdarg>
#include <sstream>
#include <string.h>

namespace DonerComponents
{
	enum class EErrorCode
	{
		FileNotFound = 1,
		ReadFromBufferFailed,
		JSONError,
		JSONBadFormat,
		InvalidSceneParsingType,
		NoMoreEntitiesAvailable,
		NoMoreComponentsAvailable,
		PrefabNotRegistered,
		PrefabAlreadyRegistered,
		ComponentFactoryAlreadyRegistered,
		ComponentFactoryNotRegistered,
		InvalidComponentFactoryIndex,
		TagAlreadyExists,
		TagNotFound,
		MessageAlreadyRegistered,
		GameObjectChildAlreadyExists,
		GameObjectChildNotFound,
		ComponentdNotFoundInGameObject,
		ComponentdAlreadyFoundInGameObject,
		ComponentNotRegisteredInFactory,
		GameObjectNotRegisteredInFactory
	};

#if defined _DEBUG
	static void PrintMessage(const char* type, const char* file, const char* function, int line, std::string errorType, int errorCode, const char* message, ...)
	{
		char buf[4096];
		va_list args;
		va_start(args, message);
		vsnprintf(buf, 4096, message, args);
		va_end(args);

		printf("%s(%d) [%s] -> %s: %s (Code %d): %s\n", file, line, function, type, errorType.c_str(), errorCode, buf);
	}

	static const char* GetShortFilename(const char* filename)
	{
		const char* shortFilename = filename + strlen(filename) - 1;
		while ((*shortFilename != '/') && (*shortFilename != '\\') && (shortFilename != filename))
		{
			--shortFilename;
		}
		if (shortFilename != filename)
		{  
			++shortFilename;
		}
		return shortFilename;
	}

	static std::string GetShortErrorName(std::string errorName)
	{
		static const std::string s_prefix("EErrorCode::");
		return errorName.substr(s_prefix.size()).c_str();
	}

	#define DC_MSG(type, errorCode, ...) \
	DonerComponents::PrintMessage(#type, GetShortFilename(__FILE__), __FUNCTION__, __LINE__, GetShortErrorName(#errorCode), static_cast<int>(errorCode), __VA_ARGS__)

	#define DC_ERROR_MSG(errorCode, ...) \
	DC_MSG(ERROR, errorCode, __VA_ARGS__)

	#define DC_WARNING_MSG(warningCode, ...) \
	DC_MSG(WARNING, warningCode, __VA_ARGS__)
#else
	#define DC_MSG(type, errorCode, ...) {}
	#define DC_ERROR_MSG(errorCode, ...) {}
	#define DC_WARNING_MSG(warningCode, ...) {}
#endif
}
