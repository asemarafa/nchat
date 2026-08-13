// uicommandbardialog.h
//
// Copyright (c) 2026 Kristofer Berggren
// All rights reserved.
//
// nchat is distributed under the MIT license, see LICENSE for details.

#pragma once

#include <string>
#include <vector>

#include "uilistdialog.h"

struct CommandItem
{
  std::string func;
  std::string label;
  wint_t keyCode;
};

class UiCommandBarDialog : public UiListDialog
{
public:
  UiCommandBarDialog(const UiDialogParams& p_Params);
  virtual ~UiCommandBarDialog();

  CommandItem GetSelectedCommand() const;

protected:
  virtual void OnSelect() override;
  virtual void OnBack() override;
  virtual bool OnTimer() override;
  virtual void UpdateList() override;

private:
  void InitCommands();

private:
  std::vector<CommandItem> m_AllCommands;
  std::vector<CommandItem> m_FilteredCommands;
  CommandItem m_SelectedCommand;
};
