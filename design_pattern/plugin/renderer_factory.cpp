// renderer_factory.cpp

#include "renderer_factory.h"

RendererFactory::CallbackMap RendererFactory::mRenderers_;

void RendererFactory::RegisterRenderer(const std::string& type,
                                       CreateCallback cb) {
  mRenderers_[type] = cb;
}

void RendererFactory::UnregisterRenderer(const std::string& type) {
  mRenderers_.erase(type);
}

IRenderer* RendererFactory::CreateRenderer(const std::string& type) {
  CallbackMap::iterator it = mRenderers_.find(type);
  if (it != mRenderers_.end()) {
    return (it->second)();
  }
  return nullptr;
}