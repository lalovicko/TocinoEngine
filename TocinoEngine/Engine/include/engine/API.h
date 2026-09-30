/**
 * @file API.h
 * @brief Macro de exportación/importación de símbolos para Engine.dll.
 *
 * Todo símbolo público del motor (clases y funciones) se marca con ENGINE_API
 * para que el mismo header sirva tanto al compilar la DLL como al usarla.
 */

#pragma once

 /**
  * @def ENGINE_API
  * @brief Controla si un símbolo se exporta o se importa de la DLL.
  *
  * | Contexto                                        | Valor                   |
  * |-------------------------------------------------|-------------------------|
  * | Windows, compilando Engine (`ENGINE_BUILD_DLL`)  | `__declspec(dllexport)` |
  * | Windows, compilando un cliente (ej. Sandbox)     | `__declspec(dllimport)` |
  * | Otras plataformas                                | vacío                   |
  *
  * @note `ENGINE_BUILD_DLL` solo se define en el proyecto Engine (`Engine.vcxproj`).
  */
#if defined(_WIN32)

#if defined(ENGINE_BUILD_DLL)
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#else
#define ENGINE_API
#endif 