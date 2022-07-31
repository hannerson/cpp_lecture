// pluginapi.h
#pragma once

#include "defines.h"
#include "renderer.h"

#define CORE_FUNC extern "C" CORE_API
#define PLUGIN_FUNC extern "C" PLUGIN_API

#define PLUGIN_INIT() PLUGIN_FUNC int PluginInit()
#define PLUGIN_FREE() PLUGIN_FUNC int PluginFree()

typedef IRenderer *(*RendererInitFunc)();
typedef void (*RendererFreeFunc)(IRenderer *);

CORE_FUNC void RegisterRenderer(const char *type, RendererInitFunc init_cb,
                                RendererFreeFunc free_cb);
CORE_FUNC void UnregisterRenderer(const char *type);