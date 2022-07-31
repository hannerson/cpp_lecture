// plugin.cpp

#include <iostream>

#include "pluginapi.h"

class OpenGLRenderer : public IRenderer {
 public:
  ~OpenGLRenderer() {}
  bool LoadScene(const char* filename) { return true; }
  void SetViewportSize(int w, int h) {}
  void SetCameraPosition(double x, double y, double z) {}
  void SetLookAt(double x, double y, double z) {}
  void Render() { std::cout << "OpenGL Render" << std::endl; }
};

PLUGIN_FUNC IRenderer* CreateRenderer() { return new OpenGLRenderer(); }

PLUGIN_FUNC void DestroyRenderer(IRenderer* r) { delete r; }

PLUGIN_INIT() {
  RegisterRenderer("opengl", CreateRenderer, DestroyRenderer);
  return 0;
}

PLUGIN_FREE() { UnregisterRenderer("opengl"); }