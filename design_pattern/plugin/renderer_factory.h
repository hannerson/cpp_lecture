// renderer_factory.h

#include <map>
#include <string>

#include "renderer.h"

class RendererFactory {
 public:
  typedef IRenderer* (*CreateCallback)();
  static void RegisterRenderer(const std::string& type, CreateCallback cb);
  static void UnregisterRenderer(const std::string& type);
  static IRenderer* CreateRenderer(const std::string& type);

 private:
  typedef std::map<std::string, CreateCallback> CallbackMap;
  static CallbackMap mRenderers_;
};