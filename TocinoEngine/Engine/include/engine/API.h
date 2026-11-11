/**
 * @file API.h
 * @brief Define la macro @c ENGINE_API para exportar/importar símbolos de la DLL del Engine.
 *
 * - Con @c ENGINE_BUILD_DLL definido (al compilar el Engine): @c __declspec(dllexport).
 * - Sin @c ENGINE_BUILD_DLL (al consumir el Engine, p. ej. el Sandbox): @c __declspec(dllimport).
 * - En plataformas que no son Windows se expande a nada.
 */
#pragma once

#if defined(_WIN32)

#if defined(ENGINE_BUILD_DLL)
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#else
#define ENGINE_API
#endif