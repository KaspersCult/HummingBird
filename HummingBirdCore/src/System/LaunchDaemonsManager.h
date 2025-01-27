//
// Created by Kasper de Bruin on 08/01/2024.
//

#pragma once

#include <PCH/pch.h>
#include "Utils/Plist/PlistUtil.h"
#include <HBUI/HBUI.h>
#include <HBUI/UIWindow.h>

namespace HummingBirdCore {
  namespace System {
    enum LaunchDaemonStatus {
      Running,
      OK,
      Stopped,
      Unknown
    };

    struct LaunchDaemon {
  private:
      Utils::File file;
      std::shared_ptr<Utils::PlistUtil::Plist> plist;
      bool saved = true;
  public:
      LaunchDaemon(Utils::File file) : file(file),
                                                plist(std::make_unique<Utils::PlistUtil::Plist>()) {
        CORE_TRACE("Loading plist file: " + file.getFullPath());
        if(!init()){
            CORE_ERROR("Failed to load plist file: " + file.getFullPath());
        }
      }

      bool init(){
        if (Utils::FileUtils::fileExists(file.getFullPath()) || file.getName().empty()) {
          if(!plist->parsePlist(file.getFullPath()))
            return false;

        } else {
          CORE_INFO("File " + file.getFullPath() + " does not exist, creating a new plist file");
          plist->createNewPlist();
          file.setName("NewDaemon");
          file.setExtension(".plist");
        }

        return true;
      }

      void reset(Utils::File file){
        this->file = file;
        plist = std::make_unique<Utils::PlistUtil::Plist>();
        CORE_TRACE("Loading plist file: " + file.getFullPath());
        if(!init()){
          CORE_ERROR("Failed to load plist file: " + file.getFullPath());
        }
      }

      void copyData(LaunchDaemon& daemon){
        daemon.reset(this->file);
      }

  public:
      Utils::PlistUtil::Plist *getPlist() {
        return plist.get();
      }

      Utils::File &getFile() {
        return file;
      }

      const std::string getPath() const {
        return file.getPath();
      }

      const std::string getFileName() const {
        return file.getName();
      }

      const bool isSaved() const {
        return saved;
      }

      void setSaved(bool saved) {
        this->saved = saved;
      }
  public:
      void save() {
        if(getPlist()->writePlist(file)) {
          CORE_INFO("Succesfully saved daemon: " + file.getName() + file.getExtension());
          setSaved(true);
        }else{
          CORE_ERROR("Failed to save daemon: " + file.getName() + file.getExtension());
        }
      }
    };

    class LaunchDaemonsManager : public UIWindow {
  public:
      explicit LaunchDaemonsManager(const std::string &name) : UIWindow(name, ImGuiWindowFlags_None), m_userAgent()
      {
        fetchAllDaemons();
      }

      ~LaunchDaemonsManager() {
      }

      void render() override;
      void renderDaemon(LaunchDaemon &daemon);
      void renderNode(Utils::PlistUtil::PlistNode &node, int index);
      void fetchAllDaemons();
      bool selectDaemon(int idx,  std::vector<LaunchDaemon> &daemons);
      bool renderTab(const std::string &name, std::vector<LaunchDaemon> &daemons);

  private:
      int m_selectedIdx = 0;

      LaunchDaemon m_selectedDaemon = LaunchDaemon(Utils::File());
      LaunchDaemon m_copyOfSelectedDaemonStart = LaunchDaemon(Utils::File());

      //TODO: Reenable
      std::string m_selectedTab = "";
      std::vector<LaunchDaemon> m_userAgent = {};
      const std::filesystem::path c_userAgentPath =    "~/Library/LaunchAgents";

      //UI
      const ImGuiWindowFlags c_leftWindowFlags = ImGuiWindowFlags_NoScrollbar;
      const ImGuiChildFlags c_leftChildFlags = ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX;

      bool m_wrapText = false;
    };

  }// namespace System
}// namespace HummingBirdCore
