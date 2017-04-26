// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "line_analyzer_app.h"

#include "appcommon/widgets/status_bar_log.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"
#include "wx/xrc/xmlres.h"

#include "file_handler.h"
#include "line_analyzer_doc.h"
#include "line_analyzer_view.h"

/// This function handles loading all of the xrc files that are processed by
/// the wxWidgets resource compiler. It is declared in an external file so
/// this file doesn't have to be recompiled every time the resources are
/// updated.
extern void InitXmlResource();

IMPLEMENT_APP(LineAnalyzerApp)

LineAnalyzerApp::LineAnalyzerApp() {
  frame_ = nullptr;
  manager_doc_ = nullptr;
}

LineAnalyzerApp::~LineAnalyzerApp() {
}

LineAnalyzerDoc* LineAnalyzerApp::GetDocument() const {
  return dynamic_cast<LineAnalyzerDoc*>(manager_doc_->GetCurrentDocument());
}

bool LineAnalyzerApp::OnCmdLineParsed(wxCmdLineParser& parser) {
  // gets the config file path
  wxString filepath_config;
  if (parser.Found("config", &filepath_config)) {
    // converts filepath to absolute if needed
    wxFileName path(filepath_config);
    if (path.IsAbsolute() == false) {
      path.MakeAbsolute(wxEmptyString, wxPATH_NATIVE);
    }

    filepath_config_ = path.GetFullPath();
  } else {
    wxFileName path(wxEmptyString, "lineanalyzer", "conf");

    // detects OS and specifies default config file path for user
    wxOperatingSystemId os = wxGetOsVersion();
    if (os == wxOS_WINDOWS_NT) {
      path.SetPath(wxStandardPaths::Get().GetUserConfigDir());
      path.AppendDir("OTLS");
      path.AppendDir("LineAnalyzer");
    } else if (os == wxOS_UNIX_LINUX) {
      path.SetPath(wxStandardPaths::Get().GetUserConfigDir());
      path.AppendDir(".config");
      path.AppendDir("OTLS");
      path.AppendDir("LineAnalyzer");
    } else {
      path.SetPath(wxStandardPaths::Get().GetExecutablePath());
    }

    filepath_config_ = path.GetFullPath();
  }

  // captures the start file which will be loaded when doc manager is created
  if (parser.GetParamCount() == 1) {
    filepath_start_ = parser.GetParam(0);
  }

  return true;
}

int LineAnalyzerApp::OnExit() {
  // disables logging because all windows are destroyed
  wxLog::EnableLogging(false);

  // saves config file
  wxFileName path(filepath_config_);
  if (wxFileName::DirExists(path.GetPath()) == false) {
    wxFileName::Mkdir(path.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
  }
  FileHandler::SaveConfig(filepath_config_, config_);

  // cleans up allocated resources
  delete manager_doc_;

  // continues exit process
  return wxApp::OnExit();
}

bool LineAnalyzerApp::OnInit() {
  // initializes command line
  if (wxApp::OnInit() == false) {
    return false;
  }

  // sets application name
  SetAppDisplayName("Line Analyzer");
  SetAppName("LineAnalyzer");

  // creates a document manager and sets to single document interface
  manager_doc_ = new wxDocManager();
  manager_doc_->SetMaxDocsOpen(1);

  // creates a document/view template relating drawing documents to their views
  (void) new wxDocTemplate(manager_doc_, "LineAnalyzer", "*.lineanalyzer",
                           "", "lineanalyzer", "LineAnalyzerDoc",
                           "LineAnalyzerView", CLASSINFO(LineAnalyzerDoc),
                           CLASSINFO(LineAnalyzerView));

  // loads all xml resource files into virtual file system
  wxXmlResource::Get()->InitAllHandlers();
  InitXmlResource();

  // creates main application frame
  frame_ = new LineAnalyzerFrame(manager_doc_);
  SetTopWindow(frame_);

  // sets application logging to a modeless dialog managed by the frame
  wxLogTextCtrl* log = new wxLogTextCtrl(frame_->pane_log()->textctrl());
  wxLog::SetActiveTarget(log);

  // manually initializes application config defaults
  config_.level_log = wxLOG_Message;
  config_.perspective = "";
  config_.size_frame = wxSize(0, 0);
  config_.units = units::UnitSystem::kImperial;

  // loads config settings from file
  // any settings defined in the file will override the app defaults
  // filehandler handles all logging
  wxFileName path;
  path = filepath_config_;
  FileHandler::LoadConfig(filepath_config_, config_);

  // sets log level specified in app config
  wxLog::SetLogLevel(config_.level_log);
  if (config_.level_log == wxLOG_Info) {
    wxLog::SetVerbose(true);
  }

  // loads a document if defined in command line
  if (filepath_start_ != wxEmptyString) {
    manager_doc_->CreateDocument(filepath_start_);
  }

  // sets application frame based on config setting and shows
  if (config_.size_frame.GetHeight() < 100
      || config_.size_frame.GetWidth() < 100) {
    frame_->Maximize();
  } else {
    frame_->SetSize(config_.size_frame);
  }
  frame_->Centre(wxBOTH);
  frame_->Show(true);

  // updates status bar
  status_bar_log::SetText("Ready", 0);

  return true;
}

void LineAnalyzerApp::OnInitCmdLine(wxCmdLineParser& parser) {
  // sets command line style and parameters
  parser.EnableLongOptions();
  parser.SetDesc(cmd_line_desc);
  parser.SetSwitchChars("--");
}

LineAnalyzerConfig* LineAnalyzerApp::config() {
  return &config_;
}

LineAnalyzerFrame* LineAnalyzerApp::frame() {
  return frame_;
}

wxDocManager* LineAnalyzerApp::manager_doc() {
  return manager_doc_;
}
