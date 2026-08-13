// uicommandbardialog.cpp
//
// Copyright (c) 2026 Kristofer Berggren
// All rights reserved.
//
// nchat is distributed under the MIT license, see LICENSE for details.

#include "uicommandbardialog.h"

#include <algorithm>

#include "strutil.h"
#include "uicolorconfig.h"
#include "uihelpview.h"
#include "uikeyconfig.h"
#include "uimodel.h"

UiCommandBarDialog::UiCommandBarDialog(const UiDialogParams& p_Params)
  : UiListDialog(p_Params, false /*p_ShadeHidden*/)
{
  InitCommands();
  UpdateList();
}

UiCommandBarDialog::~UiCommandBarDialog()
{
}

CommandItem UiCommandBarDialog::GetSelectedCommand() const
{
  return m_SelectedCommand;
}

void UiCommandBarDialog::OnSelect()
{
  if (m_FilteredCommands.empty()) return;

  m_SelectedCommand = m_FilteredCommands.at(m_Index);
  m_Result = true;
  m_Running = false;
}

void UiCommandBarDialog::OnBack()
{
}

bool UiCommandBarDialog::OnTimer()
{
  return false;
}

static bool FuzzyMatch(const std::string& p_Pattern, const std::string& p_Str, int& p_Score)
{
  if (p_Pattern.empty())
  {
    p_Score = 0;
    return true;
  }

  size_t pIdx = 0;
  int score = 0;
  int consecutive = 0;
  bool isSubstring = (p_Str.find(p_Pattern) != std::string::npos);

  if (isSubstring)
  {
    score += (p_Str.find(p_Pattern) == 0) ? 1000 : 500;
  }

  for (size_t sIdx = 0; sIdx < p_Str.size() && pIdx < p_Pattern.size(); ++sIdx)
  {
    if (p_Str[sIdx] == p_Pattern[pIdx])
    {
      score += 10;
      if (sIdx == 0 || p_Str[sIdx - 1] == ' ' || p_Str[sIdx - 1] == '_' || p_Str[sIdx - 1] == '/')
      {
        score += 50; // Word boundary bonus
      }
      if (consecutive > 0)
      {
        score += consecutive * 20; // Consecutive match bonus
      }
      consecutive++;
      pIdx++;
    }
    else
    {
      consecutive = 0;
    }
  }

  if (pIdx == p_Pattern.size())
  {
    p_Score = score - static_cast<int>(p_Str.size());
    return true;
  }

  return false;
}

void UiCommandBarDialog::UpdateList()
{
  m_Index = 0;
  m_Items.clear();
  m_FilteredCommands.clear();

  const bool isMessageSelected = m_Model->GetSelectMessageActive();
  const std::string filter = StrUtil::ToFold(StrUtil::ToString(m_FilterStr));

  std::vector<std::pair<int, CommandItem>> scoredCommands;

  for (const auto& cmd : m_AllCommands)
  {
    // Context-dependent filtering
    bool isMessageAction = (cmd.func == "edit_msg" || cmd.func == "delete_msg" ||
                            cmd.func == "react" || cmd.func == "forward_msg" ||
                            cmd.func == "open" || cmd.func == "save" ||
                            cmd.func == "open_link" || cmd.func == "jump_quoted" ||
                            cmd.func == "jump_pinned" || cmd.func == "open_msg");

    bool isChatAction = (cmd.func == "delete_chat" || cmd.func == "archive_chat" ||
                         cmd.func == "pin");

    if (isMessageAction && !isMessageSelected) continue;
    if (isChatAction && isMessageSelected) continue;

    if (filter.empty())
    {
      scoredCommands.push_back(std::make_pair(0, cmd));
    }
    else
    {
      std::string foldLabel = StrUtil::ToFold(cmd.label);
      std::string foldFunc = StrUtil::ToFold(cmd.func);

      int labelScore = 0;
      int funcScore = 0;
      bool matchLabel = FuzzyMatch(filter, foldLabel, labelScore);
      bool matchFunc = FuzzyMatch(filter, foldFunc, funcScore);

      if (matchLabel || matchFunc)
      {
        int score = std::max(matchLabel ? labelScore : -1000, matchFunc ? (funcScore - 50) : -1000);
        scoredCommands.push_back(std::make_pair(score, cmd));
      }
    }
  }

  if (!filter.empty())
  {
    std::stable_sort(scoredCommands.begin(), scoredCommands.end(),
                     [](const std::pair<int, CommandItem>& lhs, const std::pair<int, CommandItem>& rhs)
    {
      return lhs.first > rhs.first;
    });
  }

  for (const auto& scoredCmd : scoredCommands)
  {
    const CommandItem& cmd = scoredCmd.second;
    m_FilteredCommands.push_back(cmd);

    // Format display string with right-aligned shortcut
    std::string keyDisplay = UiHelpView::GetKeyDisplay(cmd.func);
    std::wstring labelW = StrUtil::ToWString(cmd.label);
    std::wstring keyW = StrUtil::ToWString(keyDisplay);

    int availableWidth = m_W;
    int labelWidth = StrUtil::WStringWidth(labelW);
    int keyWidth = StrUtil::WStringWidth(keyW);

    std::wstring itemW;
    if (keyWidth > 0 && labelWidth + keyWidth + 2 <= availableWidth)
    {
      int padding = availableWidth - labelWidth - keyWidth;
      itemW = labelW + std::wstring(padding, L' ') + keyW;
    }
    else
    {
      itemW = StrUtil::TrimPadWString(labelW, availableWidth);
    }

    m_Items.push_back(itemW);
  }
}

void UiCommandBarDialog::InitCommands()
{
  std::vector<std::pair<std::string, std::string>> commands = {
    { "send_msg", "Send Message" },
    { "select_emoji", "Add Emoji" },
    { "goto_chat", "Go to Chat" },
    { "transfer", "Send File / Attachment" },
    { "ext_edit", "External Edit (Use EDITOR)" },
    { "ext_call", "Make Voice Call" },
    { "find", "Search Chat Messages" },
    { "find_next", "Find Next Chat Match" },
    { "select_contact", "Open Address Book / Contacts" },
    { "select_mention", "Add Mention" },
    { "spell", "Run External Spell Checker" },
    { "auto_compose", "Auto Compose Message" },
    { "decrease_list_width", "Decrease Sidebar Width" },
    { "increase_list_width", "Increase Sidebar Width" },
    { "toggle_emoji", "Toggle Emoji Panel" },
    { "toggle_list", "Toggle Sidebar (List View)" },
    { "toggle_top", "Toggle Top Status Bar" },
    { "toggle_help", "Toggle Help Bar" },
    { "next_chat", "Go to Next Chat" },
    { "prev_chat", "Go to Previous Chat" },
    { "unread_chat", "Jump to Unread Chat" },
    { "delete_msg", "Delete Selected Message" },
    { "edit_msg", "Edit Selected Message" },
    { "open", "Open Selected Attachment" },
    { "save", "Save Selected Attachment" },
    { "open_link", "Open Link in Selected Message" },
    { "jump_quoted", "Jump to Quoted Message" },
    { "jump_pinned", "Jump to Pinned Message" },
    { "react", "Add Reaction to Message" },
    { "open_msg", "View Message in Pager" },
    { "forward_msg", "Forward Selected Message" },
    { "delete_chat", "Delete Selected Chat" },
    { "archive_chat", "Archive Selected Chat" },
    { "pin", "Pin / Unpin Selected Chat" },
    { "quit", "Quit nchat" }
  };

  for (const auto& cmd : commands)
  {
    CommandItem item;
    item.func = cmd.first;
    item.label = cmd.second;
    item.keyCode = UiKeyConfig::GetKey(cmd.first);
    m_AllCommands.push_back(item);
  }
}
