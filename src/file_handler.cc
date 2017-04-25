// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "file_handler.h"

#include "appcommon/units/cable_unit_converter.h"
#include "appcommon/units/hardware_unit_converter.h"
#include "appcommon/units/structure_unit_converter.h"
#include "appcommon/widgets/status_bar_log.h"
#include "appcommon/xml/cable_xml_handler.h"
#include "appcommon/xml/hardware_xml_handler.h"
#include "appcommon/xml/structure_xml_handler.h"
#include "wx/dir.h"
#include "wx/filename.h"
#include "wx/xml/xml.h"

#include "line_analyzer_app.h"
#include "line_analyzer_config_xml_handler.h"

int FileHandler::LoadCable(const std::string& filepath,
                           const units::UnitSystem& units,
                           Cable& cable) {
  std::string message = "Loading cable file: " + filepath;
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // checks if the file exists
  if (wxFileName::Exists(filepath) == false) {
    message = "Cable file (" + filepath + ") does not exist. Aborting.";
    wxLogError(message.c_str());
    return -1;
  }

  // uses an xml document to load cable file
  wxXmlDocument doc;
  if (doc.Load(filepath) == false) {
    message = filepath + "  --  "
              "Cable file contains an invalid xml structure. Aborting.";
    wxLogError(message.c_str());
    return -1;
  }

  // checks for valid xml root
  const wxXmlNode* root = doc.GetRoot();
  if (root->GetName() != "cable") {
    message = filepath + "  --  "
              "Cable file contains an invalid xml root. Aborting.";
    wxLogError(message.c_str());
    return 1;
  }

  // gets unit system attribute from file
  wxString str_units;
  units::UnitSystem units_file;
  if (root->GetAttribute("units", &str_units) == true) {
    if (str_units == "Imperial") {
      units_file = units::UnitSystem::kImperial;
    } else if (str_units == "Metric") {
      units_file = units::UnitSystem::kMetric;
    } else {
      message = filepath + "  --  "
                "Cable file contains an invalid units attribute. Aborting.";
      wxLogError(message.c_str());
      return 1;
    }
  } else {
    message = filepath + "  --  "
              "Cable file is missing units attribute. Aborting.";
    wxLogError(message.c_str());
    return 1;
  }

  // parses the xml node to populate cable object
  const bool status_node = CableXmlHandler::ParseNode(root, filepath, cable);

  // converts units to consistent style
  CableUnitConverter::ConvertUnitStyle(
      units_file,
      units::UnitStyle::kDifferent,
      units::UnitStyle::kConsistent,
      cable);

  // converts unit systems if the file doesn't match applicaton config
  units::UnitSystem units_config = wxGetApp().config()->units;
  if (units_file != units_config) {
    CableUnitConverter::ConvertUnitSystem(units_file, units_config, cable);
  }

  // adds any missing polynomial coefficients
  // at least 5 coefficients per polynomial
  std::vector<double>* coefficients = nullptr;
  const unsigned int kSizeRequired = 5;
  int num_needed = 0;

  coefficients = &cable.component_core.coefficients_polynomial_creep;
  num_needed = kSizeRequired - coefficients->size();
  for (int i = 0; i < num_needed; i++) {
    coefficients->push_back(0);
  }

  coefficients = &cable.component_core.coefficients_polynomial_loadstrain;
  num_needed = kSizeRequired - coefficients->size();
  for (int i = 0; i < num_needed; i++) {
    coefficients->push_back(0);
  }

  coefficients = &cable.component_shell.coefficients_polynomial_creep;
  num_needed = kSizeRequired - coefficients->size();
  for (int i = 0; i < num_needed; i++) {
    coefficients->push_back(0);
  }

  coefficients = &cable.component_shell.coefficients_polynomial_loadstrain;
  num_needed = kSizeRequired - coefficients->size();
  for (int i = 0; i < num_needed; i++) {
    coefficients->push_back(0);
  }

  // resets statusbar
  status_bar_log::PopText(0);

  // selects return based on parsing status
  if (status_node == true) {
    return 0;
  } else {
    return 1;
  }
}

int FileHandler::LoadConfig(const std::string& filepath,
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

int FileHandler::LoadHardware(const std::string& filepath,
                              const units::UnitSystem& units,
                              Hardware& hardware) {
  std::string message = "Loading hardware file: " + filepath;
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // checks if the file exists
  if (wxFileName::Exists(filepath) == false) {
    message = "Hardware file (" + filepath + ") does not exist. Aborting.";
    wxLogError(message.c_str());
    return -1;
  }

  // uses an xml document to load hardware file
  wxXmlDocument doc;
  if (doc.Load(filepath) == false) {
    message = filepath + "  --  "
              "Hardware file contains an invalid xml structure. Aborting.";
    wxLogError(message.c_str());
    return -1;
  }

  // checks for valid xml root
  const wxXmlNode* root = doc.GetRoot();
  if (root->GetName() != "hardware") {
    message = filepath + "  --  "
              "Hardware file contains an invalid xml root. Aborting.";
    wxLogError(message.c_str());
    return 1;
  }

  // gets unit system attribute from file
  wxString str_units;
  units::UnitSystem units_file;
  if (root->GetAttribute("units", &str_units) == true) {
    if (str_units == "Imperial") {
      units_file = units::UnitSystem::kImperial;
    } else if (str_units == "Metric") {
      units_file = units::UnitSystem::kMetric;
    } else {
      message = filepath + "  --  "
                "Hardware file contains an invalid units attribute. Aborting.";
      wxLogError(message.c_str());
      return 1;
    }
  } else {
    message = filepath + "  --  "
              "Hardware file is missing units attribute. Aborting.";
    wxLogError(message.c_str());
    return 1;
  }

  // parses the xml node to populate hardware object
  const bool status_node = HardwareXmlHandler::ParseNode(root, filepath,
                                                         hardware);

  // converts units to consistent style
  HardwareUnitConverter::ConvertUnitStyle(
      units_file,
      units::UnitStyle::kDifferent,
      units::UnitStyle::kConsistent,
      hardware);

  // converts unit systems if the file doesn't match applicaton config
  units::UnitSystem units_config = wxGetApp().config()->units;
  if (units_file != units_config) {
    HardwareUnitConverter::ConvertUnitSystem(units_file, units_config,
                                             hardware);
  }

  // resets statusbar
  status_bar_log::PopText(0);

  // selects return based on parsing status
  if (status_node == true) {
    return 0;
  } else {
    return 1;
  }
}

int FileHandler::LoadStructure(const std::string& filepath,
                               const units::UnitSystem& units,
                               Structure& structure) {
  std::string message = "Loading structure file: " + filepath;
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // checks if the file exists
  if (wxFileName::Exists(filepath) == false) {
    message = "Structure file (" + filepath + ") does not exist. Aborting.";
    wxLogError(message.c_str());
    return -1;
  }

  // uses an xml document to load structure file
  wxXmlDocument doc;
  if (doc.Load(filepath) == false) {
    message = filepath + "  --  "
              "Structure file contains an invalid xml structure. Aborting.";
    wxLogError(message.c_str());
    return -1;
  }

  // checks for valid xml root
  const wxXmlNode* root = doc.GetRoot();
  if (root->GetName() != "structure") {
    message = filepath + "  --  "
              "Structure file contains an invalid xml root. Aborting.";
    wxLogError(message.c_str());
    return 1;
  }

  // gets unit system attribute from file
  wxString str_units;
  units::UnitSystem units_file;
  if (root->GetAttribute("units", &str_units) == true) {
    if (str_units == "Imperial") {
      units_file = units::UnitSystem::kImperial;
    } else if (str_units == "Metric") {
      units_file = units::UnitSystem::kMetric;
    } else {
      message = filepath + "  --  "
                "Structure file contains an invalid units attribute. Aborting.";
      wxLogError(message.c_str());
      return 1;
    }
  } else {
    message = filepath + "  --  "
              "Structure file is missing units attribute. Aborting.";
    wxLogError(message.c_str());
    return 1;
  }

  // parses the xml node to populate structure object
  const bool status_node = StructureXmlHandler::ParseNode(root, filepath,
                                                          structure);

  // converts units to consistent style
  StructureUnitConverter::ConvertUnitStyle(
      units_file,
      units::UnitStyle::kDifferent,
      units::UnitStyle::kConsistent,
      structure);

  // converts unit systems if the file doesn't match applicaton config
  units::UnitSystem units_config = wxGetApp().config()->units;
  if (units_file != units_config) {
    StructureUnitConverter::ConvertUnitSystem(units_file, units_config,
                                              structure);
  }

  // resets statusbar
  status_bar_log::PopText(0);

  // selects return based on parsing status
  if (status_node == true) {
    return 0;
  } else {
    return 1;
  }
}

void FileHandler::SaveCable(const std::string& filepath, const Cable& cable,
                            const units::UnitSystem& units) {
  // logs
  std::string message = "Saving cable file: " + filepath;
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // creates a copy of the cable and converts to different unit style
  Cable cable_converted = cable;
  CableUnitConverter::ConvertUnitStyle(units,
                                       units::UnitStyle::kConsistent,
                                       units::UnitStyle::kDifferent,
                                       cable_converted);

  // generates an xml node
  wxXmlNode* root = CableXmlHandler::CreateNode(cable_converted, filepath,
                                                units);
  // gets the units
  if (units == units::UnitSystem::kImperial) {
    root->AddAttribute("units", "Imperial");
  } else if (units == units::UnitSystem::kMetric) {
    root->AddAttribute("units", "Metric");
  }

  // creates an xml document and saves
  wxXmlDocument doc;
  doc.SetRoot(root);
  doc.Save(filepath, 2);

  // resets statusbar
  status_bar_log::PopText(0);
}

void FileHandler::SaveConfig(const std::string& filepath,
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

void FileHandler::SaveHardware(const std::string& filepath,
                               const Hardware& hardware,
                               const units::UnitSystem& units) {
  // logs
  std::string message = "Saving hardware file: " + filepath;
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // creates a copy of the structure and converts to different unit style
  Hardware hardware_converted = hardware;
  HardwareUnitConverter::ConvertUnitStyle(units,
                                          units::UnitStyle::kConsistent,
                                          units::UnitStyle::kDifferent,
                                          hardware_converted);

  // generates an xml node
  wxXmlNode* root = HardwareXmlHandler::CreateNode(hardware_converted,
                                                   filepath,
                                                   units);
  // gets the units
  if (units == units::UnitSystem::kImperial) {
    root->AddAttribute("units", "Imperial");
  } else if (units == units::UnitSystem::kMetric) {
    root->AddAttribute("units", "Metric");
  }

  // creates an xml document and saves
  wxXmlDocument doc;
  doc.SetRoot(root);
  doc.Save(filepath, 2);

  // resets statusbar
  status_bar_log::PopText(0);
}

void FileHandler::SaveStructure(const std::string& filepath,
                                const Structure& structure,
                                const units::UnitSystem& units) {
  // logs
  std::string message = "Saving structure file: " + filepath;
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // creates a copy of the structure and converts to different unit style
  Structure structure_converted = structure;
  StructureUnitConverter::ConvertUnitStyle(units,
                                           units::UnitStyle::kConsistent,
                                           units::UnitStyle::kDifferent,
                                           structure_converted);

  // generates an xml node
  wxXmlNode* root = StructureXmlHandler::CreateNode(structure_converted,
                                                    filepath,
                                                    units);
  // gets the units
  if (units == units::UnitSystem::kImperial) {
    root->AddAttribute("units", "Imperial");
  } else if (units == units::UnitSystem::kMetric) {
    root->AddAttribute("units", "Metric");
  }

  // creates an xml document and saves
  wxXmlDocument doc;
  doc.SetRoot(root);
  doc.Save(filepath, 2);

  // resets statusbar
  status_bar_log::PopText(0);
}
