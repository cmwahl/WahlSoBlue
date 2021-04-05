#pragma once
#include <initguid.h>
#include <string>

namespace GuidHelper {
	GUID StringToGuid(const std::string& str);
	std::string GuidToString(GUID guid);
}