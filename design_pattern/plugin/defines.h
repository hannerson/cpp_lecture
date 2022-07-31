// defines.h
#pragma once

#ifdef _WIN32
#ifdef BUILDING_CORE
#define CORE_API __declspec(dllexport)
#define PLUGIN_API __declspec(dllimport)
#else
#define CORE_API __declspec(dllimport)
#define PLUGIN_API __declspec(dllexport)
#endif
#else
#define CORE_API
#define PLUGIN_API
#endif