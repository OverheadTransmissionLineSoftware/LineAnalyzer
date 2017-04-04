// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_LINEANALYZER_LINEANALYZERAPP_H_
#define OTLS_LINEANALYZER_LINEANALYZERAPP_H_

#include <string>

#include "wx/cmdline.h"
#include "wx/docview.h"
#include "wx/wx.h"

#include "line_analyzer_config.h"
#include "line_analyzer_doc.h"
#include "line_analyzer_frame.h"

/// \par OVERVIEW
///
/// This is the LineAnalyzer application class.
///
/// \par DOCUMENT / VIEW FRAMEWORK
///
/// This class implements the wxWidgets Document/View framework, which allows
/// loading/saving/printing of application files.
///
/// \par CONFIGURATION SETTINGS
///
/// The application stores configuration settings. This includes:
///   - AUI pane layout and frame size
///   - data file location
class LineAnalyzerApp : public wxApp {
 public:
  /// \brief Constructor.
  LineAnalyzerApp();

  /// \brief Destructor.
  ~LineAnalyzerApp();

  /// \brief Gets the current document.
  /// \return The document. If no document is open, a nullptr is returned.
  LineAnalyzerDoc* GetDocument() const;

  /// \brief Parses the command line options provided to the application on
  ///   startup.
  /// \param[in] parser
  ///   The command line parser, which is provided by wxWidgets.
  /// \return The success of command line parsing.
  virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

  /// \brief Cleans up application resources.
  /// \return The wxApp::OnExit return value.
  virtual int OnExit();

  /// \brief Initializes the application.
  /// \return The success of the application initialization.
  virtual bool OnInit();

  /// \brief Initializes the command line parser settings.
  /// \param[in] parser
  ///   The command line parser, which is provided by wxWidgets.
  virtual void OnInitCmdLine(wxCmdLineParser& parser);

  /// \brief Gets the application configuration settings.
  /// \return The application configuration settings.
  LineAnalyzerConfig* config();

  /// \brief Gets the application directory.
  /// \return The application directory.
  /// In debug mode, this will return the working directory. Otherwise, it will
  /// return the directory that the application executable is stored.
  wxString directory();

  /// \brief Gets the main application frame.
  /// \return The main application frame.
  LineAnalyzerFrame* frame();

  /// \brief Gets the document manager.
  /// \return The document manager.
  wxDocManager* manager_doc();

 private:
  /// \var config_
  ///   The application configuration settings.
  LineAnalyzerConfig config_;

  /// \var directory_
  ///   The reference directory of the application. The debug switch will set
  ///   this to the working directory, otherwise it will be the directory that
  ///   the application is located.
  std::string directory_;

  /// \var filepath_config_
  ///   The config file that is loaded on application startup. This is specified
  ///   as a command line option.
  std::string filepath_config_;

  /// \var filepath_start_
  ///   The file that is loaded on application startup. This is specified as
  ///   a command line option.
  std::string filepath_start_;

  /// \var frame_
  ///   The main application frame.
  LineAnalyzerFrame* frame_;

  /// \var manager_doc_
  ///   The document manager.
  wxDocManager* manager_doc_;
};

/// This is an array of command line options.
static const wxCmdLineEntryDesc cmd_line_desc [] = {
  {wxCMD_LINE_SWITCH, nullptr, "help", "shows this help message",
      wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP},
  {wxCMD_LINE_OPTION, nullptr, "config", "the application configuration file",
      wxCMD_LINE_VAL_STRING},
  {wxCMD_LINE_SWITCH, nullptr, "debug", "loads application resources from the "
      "working directory instead of application directory",
      wxCMD_LINE_VAL_NONE},
  {wxCMD_LINE_PARAM, nullptr, nullptr, "file",
      wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},

  {wxCMD_LINE_NONE}
};

DECLARE_APP(LineAnalyzerApp)

#endif  // OTLS_LINEANALYZER_LINEANALYZERAPP_H_
