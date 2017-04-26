// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "line_analyzer_doc_xml_handler.h"

#include "appcommon/xml/transmission_line_xml_handler.h"
#include "appcommon/xml/weather_load_case_xml_handler.h"
#include "wx/filename.h"

#include "file_handler.h"

wxXmlNode* LineAnalyzerDocXmlHandler::CreateNode(
    const LineAnalyzerDoc& doc,
    const units::UnitSystem& units) {
  // initializes variables used to create XML node
  wxXmlNode* node_root = nullptr;
  wxXmlNode* node_element = nullptr;
  std::string title;

  // gets document directory
  wxFileName filename_doc(doc.GetFilename());
  filename_doc.MakeAbsolute(wxEmptyString, wxPATH_NATIVE);
  wxString dirname_doc = filename_doc.GetPath();

  // creates a node for the root
  node_root = new wxXmlNode(wxXML_ELEMENT_NODE, "line_analyzer_doc");
  node_root->AddAttribute("version", "1");

  // creates structure files node and adds to root node
  title = "structures";
  node_element = new wxXmlNode(wxXML_ELEMENT_NODE, title);

  for (auto iter = doc.structures().cbegin(); iter != doc.structures().cend();
       iter++) {
    const StructureFile& structurefile = *iter;

    // converts absolute path to a relative unix-style path
    wxFileName filename(structurefile.filepath);
    filename.MakeRelativeTo(dirname_doc);
    std::string filepath_relative = filename.GetFullPath(wxPATH_UNIX);

    wxXmlNode* sub_node = CreateElementNodeWithContent("file",
                                                       filepath_relative);
    node_element->AddChild(sub_node);
  }

  node_root->AddChild(node_element);

  // creates hardware files node and adds to root node
  title = "hardwares";
  node_element = new wxXmlNode(wxXML_ELEMENT_NODE, title);

  for (auto iter = doc.hardwares().cbegin(); iter != doc.hardwares().cend();
       iter++) {
    const HardwareFile& hardwarefile = *iter;

    // converts absolute path to a relative unix-style path
    wxFileName filename(hardwarefile.filepath);
    filename.MakeRelativeTo(dirname_doc);
    std::string filepath_relative = filename.GetFullPath(wxPATH_UNIX);

    wxXmlNode* sub_node = CreateElementNodeWithContent("file",
                                                       filepath_relative);
    node_element->AddChild(sub_node);
  }

  node_root->AddChild(node_element);

  // creates cable files node and adds to root node
  title = "cables";
  node_element = new wxXmlNode(wxXML_ELEMENT_NODE, title);

  for (auto iter = doc.cables().cbegin(); iter != doc.cables().cend();
       iter++) {
    const CableFile& cablefile = *iter;

    // converts absolute path to a relative unix-style path
    wxFileName filename(cablefile.filepath);
    filename.MakeRelativeTo(dirname_doc);
    std::string filepath_relative = filename.GetFullPath(wxPATH_UNIX);

    wxXmlNode* sub_node = CreateElementNodeWithContent("file",
                                                       filepath_relative);
    node_element->AddChild(sub_node);
  }

  node_root->AddChild(node_element);

  // creates weathercase node and adds to root node
  title = "weather_load_cases";
  node_element = new wxXmlNode(wxXML_ELEMENT_NODE, title);

  for (auto iter = doc.weathercases().cbegin();
       iter != doc.weathercases().cend(); iter++) {
    const WeatherLoadCase& weathercase = *iter;
    wxXmlNode* sub_node =
        WeatherLoadCaseXmlHandler::CreateNode(weathercase, "", units);
    node_element->AddChild(sub_node);
  }

  node_root->AddChild(node_element);

  // creates transmission lines node and adds to root node
  title = "transmission_lines";
  node_element = new wxXmlNode(wxXML_ELEMENT_NODE, title);

  for (auto iter = doc.lines().cbegin(); iter != doc.lines().cend(); iter++) {
    const TransmissionLine& line = *iter;
    wxXmlNode* sub_node =
        TransmissionLineXmlHandler::CreateNode(line, "", units);
    node_element->AddChild(sub_node);
  }

  node_root->AddChild(node_element);

  // returns node
  return node_root;
}

bool LineAnalyzerDocXmlHandler::ParseNode(
    const wxXmlNode* root,
    const std::string& filepath,
    const units::UnitSystem& units,
    LineAnalyzerDoc& doc) {
  wxString message;

  // checks for valid root node
  if (root->GetName() != "line_analyzer_doc") {
    message = FileAndLineNumber(filepath, root) +
              " Invalid root node. Aborting node parse.";
    wxLogError(message);
    return false;
  }

  // gets version attribute
  wxString version;
  if (root->GetAttribute("version", &version) == false) {
    message = FileAndLineNumber(filepath, root) +
              " Version attribute is missing. Aborting node parse.";
    wxLogError(message);
    return false;
  }

  // sends to proper parsing function
  if (version == "1") {
    return ParseNodeV1(root, filepath, units, doc);
  } else {
    message = FileAndLineNumber(filepath, root) +
              " Invalid version number. Aborting node parse.";
    wxLogError(message);
    return false;
  }
}

bool LineAnalyzerDocXmlHandler::ParseNodeV1(
    const wxXmlNode* root,
    const std::string& filepath,
    const units::UnitSystem& units,
    LineAnalyzerDoc& doc) {
  bool status = true;
  wxString message;

  // gets document directory
  wxFileName filename_doc(doc.GetFilename());
  filename_doc.MakeAbsolute(wxEmptyString, wxPATH_NATIVE);
  wxString dirname_doc = filename_doc.GetPath();

  // evaluates each child node
  const wxXmlNode* node = root->GetChildren();
  while (node != nullptr) {
    const wxString title = node->GetName();
    const wxString content = ParseElementNodeWithContent(node);

    if (title == "structures") {
      // gets node for structure file
      wxXmlNode* sub_node = node->GetChildren();

      int index = 0;
      while (sub_node != nullptr) {
        // gets filepath and converts to absolute if needed
        wxString filepath_node = ParseElementNodeWithContent(sub_node);
        wxFileName filename(filepath_node);
        if (filename.IsAbsolute() == false) {
          filename.MakeAbsolute(dirname_doc, wxPATH_NATIVE);
        }

        // loads structure file
        // filehandler function handles all logging
        StructureFile structurefile;
        structurefile.filepath = filename.GetFullPath();
        const int status_node = FileHandler::LoadStructure(
            structurefile.filepath,
            units,
            structurefile.structure);

        // adds to document if no file errors were encountered
        if ((status_node == 0) || (status_node == 1)) {
          doc.InsertStructureFile(index, structurefile);
        } else {
          message = FileAndLineNumber(filepath, sub_node)
                    + "Invalid structure file. Skipping.";
          wxLogError(message);
          status = false;
        }

        index++;
        sub_node = sub_node->GetNext();
      }
    } else if (title == "hardwares") {
      // gets node for hardware file
      wxXmlNode* sub_node = node->GetChildren();

      int index = 0;
      while (sub_node != nullptr) {
        // gets filepath and converts to absolute if needed
        wxString filepath_node = ParseElementNodeWithContent(sub_node);
        wxFileName filename(filepath_node);
        if (filename.IsAbsolute() == false) {
          filename.MakeAbsolute(dirname_doc, wxPATH_NATIVE);
        }

        // loads hardware file
        // filehandler function handles all logging
        HardwareFile hardwarefile;
        hardwarefile.filepath = filename.GetFullPath();
        const int status_node = FileHandler::LoadHardware(
            hardwarefile.filepath,
            units,
            hardwarefile.hardware);

        // adds to document if no file errors were encountered
        if ((status_node == 0) || (status_node == 1)) {
          doc.InsertHardwareFile(index, hardwarefile);
        } else {
          message = FileAndLineNumber(filepath, sub_node)
                    + "Invalid hardware file. Skipping.";
          wxLogError(message);
          status = false;
        }

        index++;
        sub_node = sub_node->GetNext();
      }
    } else if (title == "cables") {
      // gets node for cable file
      wxXmlNode* sub_node = node->GetChildren();

      int index = 0;
      while (sub_node != nullptr) {
        // gets filepath and converts to absolute if needed
        wxString filepath_node = ParseElementNodeWithContent(sub_node);
        wxFileName filename(filepath_node);
        if (filename.IsAbsolute() == false) {
          filename.MakeAbsolute(dirname_doc, wxPATH_NATIVE);
        }

        // loads cable file
        // filehandler function handles all logging
        CableFile cablefile;
        cablefile.filepath = filename.GetFullPath();
        const int status_node = FileHandler::LoadCable(cablefile.filepath,
                                                       units,
                                                       cablefile.cable);

        // adds to document if no file errors were encountered
        if ((status_node == 0) || (status_node == 1)) {
          doc.InsertCableFile(index, cablefile);
        } else {
          message = FileAndLineNumber(filepath, sub_node)
                    + "Invalid cable file. Skipping.";
          wxLogError(message);
          status = false;
        }

        index++;
        sub_node = sub_node->GetNext();
      }
    } else if (title == "weather_load_cases") {
      // gets node for weather load case
      wxXmlNode* sub_node = node->GetChildren();

      int index = 0;
      while (sub_node != nullptr) {
        // creates a weathercase and parses
        WeatherLoadCase weathercase;
        const bool status_node = WeatherLoadCaseXmlHandler::ParseNode(
            sub_node, filepath, weathercase);
        if (status_node == false) {
          status = false;
        }

        // adds to document if no file errors were encountered
        if ((status_node == 0) || (status_node == 1)) {
          doc.InsertWeatherCase(index, weathercase);
        } else {
          message = FileAndLineNumber(filepath, sub_node)
                    + "Invalid weathercase. Skipping.";
          wxLogError(message);
          status = false;
        }

        index++;
        sub_node = sub_node->GetNext();
      }
    } else if (title == "transmission_lines") {
      // gets node for weather load case
      wxXmlNode* sub_node = node->GetChildren();

      // creates a list of structure const pointers for parsing
      std::list<const Structure*> structures;
      for (auto iter = doc.structures().cbegin();
           iter != doc.structures().cend(); iter++) {
        const StructureFile& structurefile = *iter;
        const Structure* structure = &structurefile.structure;
        structures.push_back(structure);
      }

      // creates a list of hardware const pointers for parsing
      std::list<const Hardware*> hardwares;
      for (auto iter = doc.hardwares().cbegin();
           iter != doc.hardwares().cend(); iter++) {
        const HardwareFile& hardwarefile = *iter;
        const Hardware* hardware = &hardwarefile.hardware;
        hardwares.push_back(hardware);
      }

      // creates a list of cable const pointers for parsing
      std::list<const Cable*> cables;
      for (auto iter = doc.cables().cbegin();
           iter != doc.cables().cend(); iter++) {
        const CableFile& cablefile = *iter;
        const Cable* cable = &cablefile.cable;
        cables.push_back(cable);
      }

      // creates a list of weathercase const pointers for parsing
      std::list<const WeatherLoadCase*> weathercases;
      for (auto iter = doc.weathercases().cbegin();
           iter != doc.weathercases().cend(); iter++) {
        const WeatherLoadCase* weathercase = &(*iter);
        weathercases.push_back(weathercase);
      }

      int index = 0;
      while (sub_node != nullptr) {
        // creates a transmission line and parses
        TransmissionLine line;
        const bool status_node = TransmissionLineXmlHandler::ParseNode(
            sub_node, filepath, &structures, &hardwares, &cables, &weathercases,
            line);
        if (status_node == false) {
          status = false;
        }

        // adds to document if no file errors were encountered
        if ((status_node == 0) || (status_node == 1)) {
          doc.InsertTransmissionLine(index, line);
        } else {
          message = FileAndLineNumber(filepath, sub_node)
                    + "Invalid transmission line. Skipping.";
          wxLogError(message);
          status = false;
        }

        index++;
        sub_node = sub_node->GetNext();
      }
    } else {
      message = FileAndLineNumber(filepath, node)
                + "XML node isn't recognized.";
      wxLogError(message);
      status = false;
    }

    node = node->GetNext();
  }

  return status;
}
