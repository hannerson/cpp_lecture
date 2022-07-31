// plugin_manager.cpp
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <map>

#include "defines.h"

class CORE_API PluginInstance {
 public:
  explicit PluginInstance(const std::string &name);
  ~PluginInstance();
  bool Load();
  bool Unload();
  std::string GetFileName();
  std::string GetDisplayName();

 private:
  PluginInstance(const PluginInstance &) = delete;
  const PluginInstance &operator=(const PluginInstance &) = delete;
  std::string name_;
  std::string path_ = "./plugin";
  void *handle_ = nullptr;
};

class CORE_API PluginManager {
 public:
  static PluginManager *GetInstance();
  bool LoadAll();
  bool Load(const std::string &name);
  bool UnloadAll();
  bool Unload(const std::string &name);
  std::vector<PluginInstance *> GetAllPlugins();

 private:
  PluginManager();
  ~PluginManager();
  PluginManager(const PluginManager &);
  const PluginManager &operator=(const PluginManager &);
  std::map<std::string, PluginInstance *> mPlugins_;
  static PluginManager *manager_;
  static std::mutex mutex_;
};