#pragma once

#include <cassert>
#include <cstdarg>
#include <sstream>
#include <string.h>

namespace DonerECS
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
		EntityChildAlreadyExists,
		EntityChildNotFound,
		ComponentdNotFoundInEntity,
		ComponentdAlreadyFoundInEntity,
		ComponentNotRegisteredInFactory,
		EntityNotRegisteredInFactory
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

	#define DECS_MSG(type, errorCode, ...) \
	DonerECS::PrintMessage(#type, GetShortFilename(__FILE__), __FUNCTION__, __LINE__, GetShortErrorName(#errorCode), static_cast<int>(errorCode), __VA_ARGS__)

	#define DECS_ERROR_MSG(errorCode, ...) \
	DECS_MSG(ERROR, errorCode, __VA_ARGS__)

	#define DECS_WARNING_MSG(warningCode, ...) \
	DECS_MSG(WARNING, warningCode, __VA_ARGS__)
#else
	#define DECS_MSG(type, errorCode, ...) {}
	#define DECS_ERROR_MSG(errorCode, ...) {}
	#define DECS_WARNING_MSG(warningCode, ...) {}
#endif
}
