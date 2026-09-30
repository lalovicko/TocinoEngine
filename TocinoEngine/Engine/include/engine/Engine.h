#pragma once

#include "API.h"

extern "C"
{
	ENGINE_API const wchar_t* Engine_GetName() noexcept;
	ENGINE_API int Engine_RunSmokeTest() noexcept;
}