#include "plugin_manager.h"

#include <dlfcn.h>

#include <iostream>

PluginInstance::PluginInstance(const std::string& name) : name_(name) {}

PluginInstance::~PluginInstance() {}

bool PluginInstance::Load() {
  handle_ = dlopen((path_ + "/" + name_).c_str(), RTLD_LAZY);
  if (!handle_) {
    std::cout << "open " << name_ << " failed!" << std::endl;
    return false;
  }
  typedef int (*init_t)();
  init_t init = (init_t)dlsym(handle_, "PluginInit");
  const char* dl_error = dlerror();
  if (dl_error) {
    std::cout << "load sym PluginInit failed: " << dl_error << std::endl;
    return false;
  }

  init();
  return true;
}

bool PluginInstance::Unload() {
  typedef int (*free_t)();
  free_t free = (free_t)dlsym(handle_, "PluginFree");
  const char* dl_error = dlerror();
  if (dl_error) {
    std::cout << "load sym PluginFree failed: " << dl_error << std::endl;
    return false;
  }
  free();
  if (handle_) {
    dlclose(handle_);
  }
  return true;
}

std::string PluginInstance::GetDisplayName() { return name_; }

std::string PluginInstance::GetFileName() { return path_ + "/" + name_; }

PluginManager* PluginManager::manager_ = nullptr;
std::mutex PluginManager::mutex_;

PluginManager::PluginManager() {}
PluginManager::~PluginManager() {}
PluginManager* PluginManager::GetInstance() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (manager_ == nullptr) {
    manager_ = new PluginManager();
  }
  return manager_;
}

bool PluginManager::Load(const std::string& name) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (mPlugins_.count(name) == 0) {
    PluginInstance* instance = new PluginInstance(name);
    instance->Load();
    mPlugins_.emplace(name, instance);
  }
  return true;
}

bool PluginManager::Unload(const std::string& name) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (mPlugins_.count(name) == 0) {
    return true;
  }
  PluginInstance* instance = mPlugins_[name];
  instance->Unload();
  delete instance;
  mPlugins_.erase(name);
  return true;
}

bool PluginManager::LoadAll() {
  // from config file or list in directory
  std::vector<std::string> all_plugins({"libopengl.so"});
  for(auto &name:all_plugins){
    Load(name);
  }
}

bool PluginManager::UnloadAll() {
    for(auto &plugin:mPlugins_){
        Unload(plugin.first);
    }
}
