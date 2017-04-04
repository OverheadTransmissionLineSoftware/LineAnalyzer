// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "line_analyzer_frame.h"

#include "appcommon/units/cable_unit_converter.h"
#include "appcommon/units/weather_load_case_unit_converter.h"
#include "wx/aboutdlg.h"
#include "wx/xrc/xmlres.h"

#include "file_handler.h"
#include "line_analyzer_app.h"
#include "preferences_dialog.h"
#include "xpm/icon.xpm"

DocumentFileDropTarget::DocumentFileDropTarget(wxWindow* parent) {
  parent_ = parent;

  // creates data object to store dropped information
  SetDataObject(new wxFileDataObject());
  SetDefaultAction(wxDragCopy);
}

bool DocumentFileDropTarget::OnDropFiles(wxCoord x, wxCoord y,
                                         const wxArrayString& filenames) {
  // gets data from drag-and-drop operation
  wxFileDataObject* data = dynamic_cast<wxFileDataObject*>(GetDataObject());

  // tests if file exists
  // only the first file in the list is processed
  const wxString& str_file = data->GetFilenames().front();
  if (wxFileName::Exists(str_file) == false) {
    return false;
  }

  // tests if extension matches application document
  wxFileName path(str_file);
  if (path.GetExt() == ".lineanalyzer") {
    return false;
  }

  // freezes frame, opens document, and thaws frame
  parent_->Freeze();
  wxGetApp().manager_doc()->CreateDocument(str_file);
  parent_->Thaw();

  return true;
}

BEGIN_EVENT_TABLE(LineAnalyzerFrame, wxDocParentFrame)
  EVT_MENU(XRCID("menuitem_file_preferences"), LineAnalyzerFrame::OnMenuFilePreferences)
  EVT_MENU(XRCID("menuitem_help_about"), LineAnalyzerFrame::OnMenuHelpAbout)
END_EVENT_TABLE()

LineAnalyzerFrame::LineAnalyzerFrame(wxDocManager* manager)
    : wxDocParentFrame(manager, nullptr, wxID_ANY, "Line Analyzer") {
  // loads dialog from virtual xrc file system
  wxXmlResource::Get()->LoadMenuBar(this, "line_analyzer_menubar");

  // sets the frame icon
  SetIcon(wxIcon(icon_xpm));

  // sets the drag and drop target
  SetDropTarget(new DocumentFileDropTarget(this));

  // tells aui manager to manage this frame
  manager_.SetManagedWindow(this);

  // creates status bar
  wxStatusBar* status_bar = CreateStatusBar();
  const int kFieldsCount = 2;
  const int widths_field[2] = {-1, 170};
  status_bar->SetFieldsCount(kFieldsCount);
  status_bar->SetStatusWidths(kFieldsCount, widths_field);

  // creates log AUI window and adds to manager
  wxAuiPaneInfo info;
  info.Name("Log");
  info.Float();
  info.Caption("Log");
  info.CloseButton(true);
  info.Show(false);
  pane_log_ = new LogPane(this);
  manager_.AddPane(pane_log_, info);

  manager_.Update();
}

LineAnalyzerFrame::~LineAnalyzerFrame() {
  // saves frame size to application config
  LineAnalyzerConfig* config = wxGetApp().config();
  if (this->IsMaximized() == true) {
    config->size_frame = wxSize(0, 0);
  } else {
    config->size_frame = this->GetSize();
  }

  manager_.UnInit();
}

void LineAnalyzerFrame::OnMenuFilePreferences(wxCommandEvent& event) {
  // gets the application config
  LineAnalyzerConfig* config = wxGetApp().config();

  // stores a copy of the unit system before letting user edit
  units::UnitSystem units_before = config->units;

  // creates preferences editor dialog and shows
  // exits if user closes/cancels
  PreferencesDialog preferences(this, config);
  if (preferences.ShowModal() != wxID_OK) {
    return;
  }

  // updates logging level
  wxLog::SetLogLevel(config->level_log);
  if (config->level_log == wxLOG_Message) {
    wxLog::SetVerbose(false);
  } else if (config->level_log == wxLOG_Info) {
    wxLog::SetVerbose(true);
  }
}

void LineAnalyzerFrame::OnMenuHelpAbout(wxCommandEvent& event) {
  // sets the dialog info
  wxAboutDialogInfo info;
  info.SetIcon(wxICON(icon));
  info.SetName(wxGetApp().GetAppDisplayName());
  info.SetVersion("0.1.0");
  info.SetCopyright("License:   http://unlicense.org/");
  info.SetDescription(
    "This application provides a GUI for modeling transmission lines.\n"
    "\n"
    "This application is part of the Overhead Transmission Line Software\n"
    "suite. For the actual engineering modeling see the Models library.\n"
     "\n"
    "This software was developed so that transmission engineers can know\n"
    "exactly how results are calculated, and so that the software can be\n"
    "freely modified to fit the unique needs of the utility they represent.\n"
    "\n"
    "To get involved in project development, or to review the code, see the\n"
    "website link.");
  info.SetWebSite("https://github.com/OverheadTransmissionLineSoftware/LineAnalyzer");

  // shows the dialog
  wxAboutBox(info, this);
}

void LineAnalyzerFrame::OnMenuViewLog(wxCommandEvent& event) {
  wxAuiPaneInfo& info = manager_.GetPane("Log");
  if (info.IsShown() == false) {
    info.Show(true);
  } else {
    info.Show(false);
  }

  manager_.Update();
}

LogPane* LineAnalyzerFrame::pane_log() {
  return pane_log_;
}
