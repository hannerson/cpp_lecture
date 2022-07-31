#include "plugin_manager.h"
#include "renderer_factory.h"

int main() {
  PluginManager* manager = PluginManager::GetInstance();
  manager->LoadAll();

  IRenderer* render = RendererFactory::CreateRenderer("opengl");
  if(render == nullptr){
    return -1;
  }
  render->Render();
  return 0;
}