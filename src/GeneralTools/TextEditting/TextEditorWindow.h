//
// Created by Kasper de Bruin on 11/12/2023.
//

#ifndef KBTOOLS_TEXTEDITORWINDOW_H
#define KBTOOLS_TEXTEDITORWINDOW_H

#include <vendor/filedialog/FileBrowser/ImGuiFileBrowser.h>
#include <vendor/ImGuiColorTextEdit/TextEditor.h>

#include "../../UIWindows/UIWindow.h"
#include "../../Utils/Utils.h"

namespace HummingBird::GeneralTools {
  class TextEditorWindow : public KBTools::UIWindow {
public:
    TextEditorWindow() : UIWindow(ImGuiWindowFlags_MenuBar) {}

    const TextEditor getTextEditor() const { return m_textEditor; }

    void Render() override;

private:
    TextEditor m_textEditor;
    imgui_addons::ImGuiFileBrowser file_dialog; // As a class member or globally
  };

}// namespace HummingBird::GeneralTools


#endif//KBTOOLS_TEXTEDITORWINDOW_H