// pluginapi.cpp

#include "pluginapi.h"

#include "renderer_factory.h"

CORE_FUNC void RegisterRenderer(const char* type, RendererInitFunc init_cb,
                                RendererFreeFunc free_cb) {
  RendererFactory::RegisterRenderer(
      type, reinterpret_cast<RendererFactory::CreateCallback>(init_cb));
}

CORE_FUNC void UnregisterRenderer(const char* type) {
  RendererFactory::UnregisterRenderer(type);
}