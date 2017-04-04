// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "file_handler.h"

#include "appcommon/widgets/status_bar_log.h"
#include "wx/dir.h"
#include "wx/filename.h"
#include "wx/xml/xml.h"

#include "line_analyzer_app.h"
#include "line_analyzer_config_xml_handler.h"

FileHandler::FileHandler() {
}

FileHandler::~FileHandler() {
}

int FileHandler::LoadConfigFile(const std::string& filepath,
                                LineAnalyzerConfig& config) {
  std::string message = "Loading config file: " + filepath;
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // checks if the file exists
  if (wxFileName::Exists(filepath) == false) {
    message = "Config file (" + filepath + ") does not exist. Keeping "
              "application defaults.";
    wxLogError(message.c_str());
    return -1;
  }

  // uses an xml document to load config file
  wxXmlDocument doc;
  if (doc.Load(filepath) == false) {
    message = filepath + "  --  "
              "Config file contains an invalid xml structure. Keeping "
              "application defaults.";
    wxLogError(message.c_str());
    return -1;
  }

  // checks for valid xml root
  const wxXmlNode* root = doc.GetRoot();
  if (root->GetName() != "line_analyzer_config") {
    message = filepath + "  --  "
              "Config file contains an invalid xml root. Keeping "
              "application defaults.";
    wxLogError(message.c_str());
    return 1;
  }

  // parses the XML node and loads into the config struct
  const bool status_node = LineAnalyzerConfigXmlHandler::ParseNode(
      root, filepath, config);

  // resets statusbar
  status_bar_log::PopText(0);

  // selects return based on parsing status
  if (status_node == true) {
    return 0;
  } else {
    return 1;
  }
}

void FileHandler::SaveConfigFile(const std::string& filepath,
                                 const LineAnalyzerConfig& config) {
  // logs
  std::string message = "Saving config file: " + filepath;
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // generates an xml node
  wxXmlNode* root = LineAnalyzerConfigXmlHandler::CreateNode(config);

  // creates an xml document and saves
  wxXmlDocument doc;
  doc.SetRoot(root);
  doc.Save(filepath, 2);

  // resets statusbar
  status_bar_log::PopText(0);
}
