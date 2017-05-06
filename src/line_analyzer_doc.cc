// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "line_analyzer_doc.h"

#include "appcommon/units/cable_unit_converter.h"
#include "appcommon/units/hardware_unit_converter.h"
#include "appcommon/units/structure_unit_converter.h"
#include "appcommon/units/transmission_line_unit_converter.h"
#include "appcommon/units/weather_load_case_unit_converter.h"
#include "appcommon/widgets/status_bar_log.h"
#include "appcommon/xml/transmission_line_xml_handler.h"
#include "wx/cmdproc.h"
#include "wx/wx.h"

#include "file_handler.h"
#include "line_analyzer_app.h"
#include "line_analyzer_doc_xml_handler.h"

IMPLEMENT_DYNAMIC_CLASS(LineAnalyzerDoc, wxDocument)

LineAnalyzerDoc::LineAnalyzerDoc() {
  lines_.push_back(TransmissionLine());
  index_active_ = 0;
  line_active_ = &(*lines_.begin());
}

LineAnalyzerDoc::~LineAnalyzerDoc() {
}

void LineAnalyzerDoc::ConvertUnitStyle(const units::UnitSystem& system,
                                       const units::UnitStyle& style_from,
                                       const units::UnitStyle& style_to) {
  if (style_from == style_to) {
    return;
  }

  // converts cable files
  for (auto it = cables_.begin(); it != cables_.end(); it++) {
    CableFile& cablefile = *it;
    Cable& cable = cablefile.cable;
    CableUnitConverter::ConvertUnitStyle(system, style_from,
                                         style_to, cable);
  }

  // converts hardware files
  for (auto it = hardwares_.begin(); it != hardwares_.end(); it++) {
    HardwareFile& hardwarefile = *it;
    Hardware& hardware = hardwarefile.hardware;
    HardwareUnitConverter::ConvertUnitStyle(system, style_from,
                                            style_to, hardware);
  }

  // converts structure files
  for (auto it = structures_.begin(); it != structures_.end(); it++) {
    StructureFile& structurefile = *it;
    Structure& structure = structurefile.structure;
    StructureUnitConverter::ConvertUnitStyle(system, style_from,
                                             style_to, structure);
  }

  // converts weathercases
  for (auto it = weathercases_.begin(); it != weathercases_.end(); it++) {
    WeatherLoadCase& weathercase = *it;
    WeatherLoadCaseUnitConverter::ConvertUnitStyle(system, style_from,
                                                   style_to, weathercase);
  }

  // converts lines
  for (auto it = lines_.begin(); it != lines_.end(); it++) {
    TransmissionLine& line = *it;
    TransmissionLineUnitConverter::ConvertUnitStyle(system, style_from,
                                                    style_to, line);
  }
}

void LineAnalyzerDoc::ConvertUnitSystem(const units::UnitSystem& system_from,
                                        const units::UnitSystem& system_to) {
  if (system_from == system_to) {
    return;
  }

  // converts cable files
  for (auto it = cables_.begin(); it != cables_.end(); it++) {
    CableFile& cablefile = *it;
    Cable& cable = cablefile.cable;
    CableUnitConverter::ConvertUnitSystem(system_from, system_to, cable);
  }

  // converts hardware files
  for (auto it = hardwares_.begin(); it != hardwares_.end(); it++) {
    HardwareFile& hardwarefile = *it;
    Hardware& hardware = hardwarefile.hardware;
    HardwareUnitConverter::ConvertUnitSystem(system_from, system_to, hardware);
  }

  // converts structure files
  for (auto it = structures_.begin(); it != structures_.end(); it++) {
    StructureFile& structurefile = *it;
    Structure& structure = structurefile.structure;
    StructureUnitConverter::ConvertUnitSystem(system_from, system_to,
                                              structure);
  }

  // converts weathercases
  for (auto it = weathercases_.begin(); it != weathercases_.end(); it++) {
    WeatherLoadCase& weathercase = *it;
    WeatherLoadCaseUnitConverter::ConvertUnitSystem(system_from, system_to,
                                                    weathercase);
  }

  // converts lines
  for (auto it = lines_.begin(); it != lines_.end(); it++) {
    TransmissionLine& line = *it;
    TransmissionLineUnitConverter::ConvertUnitSystem(system_from, system_to,
                                                     line);
  }
}

bool LineAnalyzerDoc::DeleteTransmissionLine(const int& index) {
  // checks index
  if (IsValidIndex(index, lines_.size(), false) == false) {
    return false;
  }

  // gets iterator with edit capability and deletes
  auto iter = std::next(lines_.begin(), index);
  lines_.erase(iter);

  // changes active transmission line to index before deleted one
  index_active_--;

  // ensures that at least one transmission line exists and is active
  if (lines_.size() == 0) {
    lines_.push_back(TransmissionLine());
    index_active_ = 0;
  }

  UpdateActiveLineReference();

  // marks as modified
  Modify(true);

  return true;
}

bool LineAnalyzerDoc::InsertTransmissionLine(const int& index,
                                             const TransmissionLine& line) {
  // checks index
  if (IsValidIndex(index, lines_.size(), true) == false) {
    return false;
  }

  // gets iterator with edit capability and inserts
  auto iter = std::next(lines_.begin(), index);
  lines_.insert(iter, line);

  // marks as modified
  Modify(true);

  return true;
}

bool LineAnalyzerDoc::IsReferencedCableFile(const int& index) const {
  // checks index
  if (IsValidIndex(index, cables_.size(), false) == false) {
    return false;
  }

  // gets pointer to item
  auto iter_cable = std::next(cables_.cbegin(), index);
  const CableFile& cablefile = *iter_cable;
  const Cable* cable = &cablefile.cable;

  // searches transmission lines
  for (auto iter_lines = lines_.cbegin(); iter_lines != lines_.cend();
      iter_lines++) {
    const TransmissionLine& line = *iter_lines;

    // searches line cables
    for (auto it = line.line_cables()->cbegin();
         it != line.line_cables()->cend(); it++) {
      const LineCable& line_cable = *it;
      if (line_cable.cable() == cable) {
        // match is found
        return true;
      }
    }
  }

  // no match is found
  return false;
}

bool LineAnalyzerDoc::IsReferencedHardwareFile(const int& index) const {
  // checks index
  if (IsValidIndex(index, hardwares_.size(), false) == false) {
    return false;
  }

  // gets pointer to item
  auto iter_hardware = std::next(hardwares_.cbegin(), index);
  const HardwareFile& hardwarefile = *iter_hardware;
  const Hardware* hardware = &hardwarefile.hardware;

  // searches transmission lines
  for (auto iter_lines = lines_.cbegin(); iter_lines != lines_.cend();
      iter_lines++) {
    const TransmissionLine& line = *iter_lines;

    // searches line structures
    for (auto it = line.line_structures()->cbegin();
         it != line.line_structures()->cend(); it++) {
      const LineStructure& line_structure = *it;

      // searches line structure hardware
      for (auto i = line_structure.hardwares()->cbegin();
           i != line_structure.hardwares()->cend(); i++) {
        const Hardware* line_hardware = *i;
        if (line_hardware == hardware) {
          // match is found
          return true;
        }
      }
    }
  }

  // no match is found
  return false;
}

bool LineAnalyzerDoc::IsReferencedStructureFile(const int& index) const {
  // checks index
  if (IsValidIndex(index, structures_.size(), false) == false) {
    return false;
  }

  // gets pointer to item
  auto iter_structure = std::next(structures_.cbegin(), index);
  const StructureFile& structurefile = *iter_structure;
  const Structure* structure = &structurefile.structure;

  // searches transmission lines
  for (auto iter_lines = lines_.cbegin(); iter_lines != lines_.cend();
      iter_lines++) {
    const TransmissionLine& line = *iter_lines;

    // searches line structures
    for (auto it = line.line_structures()->cbegin();
         it != line.line_structures()->cend(); it++) {
      const LineStructure& line_structure = *it;
      if (line_structure.structure() == structure) {
        // match is found
        return true;
      }
    }
  }

  // no match is found
  return false;
}

bool LineAnalyzerDoc::IsReferencedWeatherCase(const int& index) const {
  // checks index
  if (IsValidIndex(index, weathercases_.size(), false) == false) {
    return false;
  }

  // gets pointer to item
  auto iter_weather = std::next(weathercases_.cbegin(), index);
  const WeatherLoadCase* weathercase = &(*iter_weather);

  // searches transmission lines
  for (auto iter_lines = lines_.cbegin(); iter_lines != lines_.cend();
      iter_lines++) {
    const TransmissionLine& line = *iter_lines;

    // searches line cables
    for (auto it = line.line_cables()->cbegin();
         it != line.line_cables()->cend(); it++) {
      const LineCable& line_cable = *it;

      // checks constraint
      if (line_cable.constraint().case_weather == weathercase) {
        return true;
      }

      // checks stretch-creep
      if (line_cable.weathercase_stretch_creep() == weathercase) {
        return true;
      }

      // checks stretch-load
      if (line_cable.weathercase_stretch_load() == weathercase) {
        return true;
      }
    }
  }

  // no match is found
  return false;
}

wxInputStream& LineAnalyzerDoc::LoadObject(wxInputStream& stream) {
  wxBusyCursor cursor;

  std::string message;

  message = "Loading document file: " + this->GetFilename();
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // attempts to load an xml document from the input stream
  wxXmlDocument doc_xml;
  if (doc_xml.Load(stream) == false) {
    // notifies user of error
    message = GetFilename() + "  --  "
              "Document file contains an invalid xml structure. The document "
              "will close.";
    wxLogError(message.c_str());
    wxMessageBox(message);

    status_bar_log::PopText(0);

    // sets stream to invalid state and returns
    stream.Reset(wxSTREAM_READ_ERROR);
    return stream;
  }

  // checks for valid xml root
  const wxXmlNode* root = doc_xml.GetRoot();
  if (root->GetName() != "line_analyzer_doc") {
    // notifies user of error
    message = GetFilename() + "  --  "
              "Document file contains an invalid xml root. The document "
              "will close.";
    wxLogError(message.c_str());
    wxMessageBox(message);

    status_bar_log::PopText(0);

    // sets stream to invalide state and returns
    stream.Reset(wxSTREAM_READ_ERROR);
    return stream;
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
      // notifies user of error
      message = GetFilename() + "  --  "
                "Document file contains an invalid units attribute. The "
                "document will close.";
      wxLogError(message.c_str());
      wxMessageBox(message);

      status_bar_log::PopText(0);

      // sets stream to invalide state and returns
      stream.Reset(wxSTREAM_READ_ERROR);
      return stream;
    }
  } else {
    // notifies user of error
    message = GetFilename() + "  --  "
              "Document file is missing units attribute. The document will "
              "close.";
    wxLogError(message.c_str());
    wxMessageBox(message);

    status_bar_log::PopText(0);

    // sets stream to invalide state and returns
    stream.Reset(wxSTREAM_READ_ERROR);
    return stream;
  }

  // clears active line in document
  lines_.clear();

  // parses the XML node and loads into the document
  std::string filename = this->GetFilename();

  const bool status_node = LineAnalyzerDocXmlHandler::ParseNode(
      root, filename, wxGetApp().config()->units, *this);
  if (status_node == false) {
    // notifies user of error
    message = GetFilename() + "  --  "
              "Document file contains parsing error(s). Check logs.";
    wxMessageBox(message);
  }

  // converts units to consistent style
  ConvertUnitStyle(units_file,
                   units::UnitStyle::kDifferent,
                   units::UnitStyle::kConsistent);

  // converts unit systems if the file doesn't match applicaton config
  units::UnitSystem units_config = wxGetApp().config()->units;
  if (units_file != units_config) {
    ConvertUnitSystem(units_file, units_config);
  }

  // ensures that at least one transmission line exists
  if (lines_.size() == 0) {
    lines_.push_back(TransmissionLine());
  }

  // sets active transmission line
  index_active_ = 0;
  UpdateActiveLineReference();

  // resets modified status to false because the xml parser uses functions
  // that mark it as modified
  Modify(false);

  status_bar_log::PopText(0);

  return stream;
}

bool LineAnalyzerDoc::LoadTransmissionLineFromXml(const wxXmlNode* node) {
  // clears active transmission line
  *line_active_ = TransmissionLine();

  // creates a list of structure const pointers for parsing
  std::list<const Structure*> structures;
  for (auto iter = structures_.cbegin(); iter != structures_.cend(); iter++) {
    const StructureFile& structurefile = *iter;
    const Structure* structure = &structurefile.structure;
    structures.push_back(structure);
  }

  // creates a list of hardware const pointers for parsing
  std::list<const Hardware*> hardwares;
  for (auto iter = hardwares_.cbegin(); iter != hardwares_.cend(); iter++) {
    const HardwareFile& hardwarefile = *iter;
    const Hardware* hardware = &hardwarefile.hardware;
    hardwares.push_back(hardware);
  }

  // creates a list of cable const pointers for parsing
  std::list<const Cable*> cables;
  for (auto iter = cables_.cbegin(); iter != cables_.cend(); iter++) {
    const CableFile& cablefile = *iter;
    const Cable* cable = &cablefile.cable;
    cables.push_back(cable);
  }

  // creates a list of weathercase const pointers for parsing
  std::list<const WeatherLoadCase*> weathercases;
  for (auto iter = weathercases_.cbegin(); iter != weathercases_.cend();
       iter++) {
    const WeatherLoadCase* weathercase = &(*iter);
    weathercases.push_back(weathercase);
  }

  const bool status_node = TransmissionLineXmlHandler::ParseNode(
      node, "", &structures, &hardwares, &cables, &weathercases,
      *line_active_);
  if (status_node == false) {
    wxString message =
        TransmissionLineXmlHandler::FileAndLineNumber("", node)
        + "Invalid transmission line. Skipping.";
    wxLogError(message);
  }

  return status_node;
}

bool LineAnalyzerDoc::LoadTransmissionLinesFromXml(const wxXmlNode* node) {
  // deletes all transmission lines
  lines_.clear();

  // creates a list of structure const pointers for parsing
  std::list<const Structure*> structures;
  for (auto iter = structures_.cbegin(); iter != structures_.cend(); iter++) {
    const StructureFile& structurefile = *iter;
    const Structure* structure = &structurefile.structure;
    structures.push_back(structure);
  }

  // creates a list of hardware const pointers for parsing
  std::list<const Hardware*> hardwares;
  for (auto iter = hardwares_.cbegin(); iter != hardwares_.cend(); iter++) {
    const HardwareFile& hardwarefile = *iter;
    const Hardware* hardware = &hardwarefile.hardware;
    hardwares.push_back(hardware);
  }

  // creates a list of cable const pointers for parsing
  std::list<const Cable*> cables;
  for (auto iter = cables_.cbegin(); iter != cables_.cend(); iter++) {
    const CableFile& cablefile = *iter;
    const Cable* cable = &cablefile.cable;
    cables.push_back(cable);
  }

  // creates a list of weathercase const pointers for parsing
  std::list<const WeatherLoadCase*> weathercases;
  for (auto iter = weathercases_.cbegin(); iter != weathercases_.cend();
       iter++) {
    const WeatherLoadCase* weathercase = &(*iter);
    weathercases.push_back(weathercase);
  }

  // parses all sub-nodes
  bool status = true;
  wxXmlNode* sub_node = node->GetChildren();
  while (sub_node != nullptr) {
    // creates a transmission line and parses
    TransmissionLine line;
    const bool status_node = TransmissionLineXmlHandler::ParseNode(
        sub_node, "", &structures, &hardwares, &cables, &weathercases,
        line);
    if (status_node == false) {
      status = false;
    }

    // adds to document if no file errors were encountered
    if (status_node == true) {
      lines_.push_back(line);
    } else {
      wxString message =
          TransmissionLineXmlHandler::FileAndLineNumber("", sub_node)
          + "Invalid transmission line. Skipping.";
      wxLogError(message);
      status = false;
    }

    sub_node = sub_node->GetNext();
  }

  return status;
}

bool LineAnalyzerDoc::MoveTransmissionLine(const int& index_from,
                                           const int& index_to) {
  // checks indexes
  if (IsValidIndex(index_from, lines_.size(), false) == false) {
    return false;
  }

  if (IsValidIndex(index_to, lines_.size(), true) == false) {
    return false;
  }

  // gets iterators with edit capability
  auto iter_from = std::next(lines_.begin(), index_from);
  auto iter_to = std::next(lines_.begin(), index_to);

  // moves within list
  lines_.splice(iter_to, lines_, iter_from);

  // marks as modified
  Modify(true);

  return true;
}

bool LineAnalyzerDoc::OnCreate(const wxString& path, long flags) {
  // calls base class function
  return wxDocument::OnCreate(path, flags);
}

wxOutputStream& LineAnalyzerDoc::SaveObject(wxOutputStream& stream) {
  wxBusyCursor cursor;

  // logs
  std::string message = "Saving document file: " + GetFilename();
  wxLogVerbose(message.c_str());
  status_bar_log::PushText(message, 0);

  // gets the unit system from app config
  units::UnitSystem units = wxGetApp().config()->units;

  // converts to a different unit style for saving
  ConvertUnitStyle(units, units::UnitStyle::kConsistent,
                   units::UnitStyle::kDifferent);

  // generates an xml node
  wxXmlNode* root = LineAnalyzerDocXmlHandler::CreateNode(*this, units);

  // adds unit attribute to xml node
  // this attribute should be added at this step vs the xml handler because
  // the attribute describes all values in the file, and is consistent
  // with how the FileHandler functions work
  if (units == units::UnitSystem::kImperial) {
    root->AddAttribute("units", "Imperial");
  } else if (units == units::UnitSystem::kMetric) {
    root->AddAttribute("units", "Metric");
  }

  // creates an XML document and savaes to stream
  wxXmlDocument doc_xml;
  doc_xml.SetRoot(root);
  doc_xml.Save(stream);

  // converts back to a consistent unit style
  ConvertUnitStyle(units, units::UnitStyle::kDifferent,
                   units::UnitStyle::kConsistent);

  // clears commands in the processor
  wxCommandProcessor* processor = GetCommandProcessor();
  processor->ClearCommands();

  status_bar_log::PopText(0);

  return stream;
}

wxXmlNode* LineAnalyzerDoc::SaveTransmissionLineToXml() const {
  units::UnitSystem units = wxGetApp().config()->units;

  return TransmissionLineXmlHandler::CreateNode(*line_active_, "", units);;
}

wxXmlNode* LineAnalyzerDoc::SaveTransmissionLinesToXml() const {
  units::UnitSystem units = wxGetApp().config()->units;

  // creates a node for the root
  wxXmlNode* node_root = new wxXmlNode(wxXML_ELEMENT_NODE,
                                       "transmission_lines");

  // creates sub nodes for each transmission line
  for (auto iter = lines_.cbegin(); iter != lines_.cend(); iter++) {
    const TransmissionLine& line = *iter;
    wxXmlNode* sub_node =
        TransmissionLineXmlHandler::CreateNode(line, "", units);
    node_root->AddChild(sub_node);
  }

  return node_root;
}

const std::list<CableFile>& LineAnalyzerDoc::cables() const {
  return cables_;
}

const std::list<HardwareFile>& LineAnalyzerDoc::hardwares() const {
  return hardwares_;
}

int LineAnalyzerDoc::index_active() const {
  return index_active_;
}

const TransmissionLine& LineAnalyzerDoc::line() const {
  return *line_active_;
}

const std::list<TransmissionLine>& LineAnalyzerDoc::lines() const {
  return lines_;
}

bool LineAnalyzerDoc::set_cables(const std::list<CableFile>& cablefiles) {
  // saves xml node for transmission lines and clears lines
  const wxXmlNode* node = SaveTransmissionLinesToXml();
  lines_.clear();

  // temporarily caches and then replaces existing cables
  std::list<CableFile> cables_cache = cables_;
  cables_ = cablefiles;

  // reloads transmission lines from xml node
  bool status = LoadTransmissionLinesFromXml(node);
  if (status == false) {
    // loading lines failed, so resets to starting condition
    cables_ = cables_cache;

    lines_.clear();
    LoadTransmissionLinesFromXml(node);
  }

  return status;
}

bool LineAnalyzerDoc::set_hardwares(
    const std::list<HardwareFile>& hardwarefiles) {
  // saves xml node for transmission lines and clears lines
  const wxXmlNode* node = SaveTransmissionLinesToXml();
  lines_.clear();

  // temporarily caches and then replaces existing hardware
  std::list<HardwareFile> hardwares_cache = hardwares_;
  hardwares_ = hardwarefiles;

  // reloads transmission lines from xml node
  bool status = LoadTransmissionLinesFromXml(node);
  if (status == false) {
    // loading lines failed, so resets to starting condition
    hardwares_ = hardwares_cache;

    lines_.clear();
    LoadTransmissionLinesFromXml(node);
  }

  return status;
}

bool LineAnalyzerDoc::set_index_active(const int& index) {
  if (IsValidIndex(index, lines_.size(), false) == true) {
    index_active_ = index;
    UpdateActiveLineReference();
    return true;
  } else {
    return false;
  }
}

bool LineAnalyzerDoc::set_structures(
    const std::list<StructureFile>& structurefiles) {
  // saves xml node for transmission lines and clears lines
  const wxXmlNode* node = SaveTransmissionLinesToXml();
  lines_.clear();

  // temporarily caches and then replaces existing structures
  std::list<StructureFile> structures_cache = structures_;
  structures_ = structurefiles;

  // reloads transmission lines from xml node
  bool status = LoadTransmissionLinesFromXml(node);
  if (status == false) {
    // loading lines failed, so resets to starting condition
    structures_ = structures_cache;

    lines_.clear();
    LoadTransmissionLinesFromXml(node);
  }

  return status;
}

bool LineAnalyzerDoc::set_weathercases(
    const std::list<WeatherLoadCase>& weathercases) {
  // saves xml node for transmission lines and clears lines
  const wxXmlNode* node = SaveTransmissionLinesToXml();
  lines_.clear();

  // temporarily caches and then replaces existing weathercases
  std::list<WeatherLoadCase> weathercases_cache = weathercases_;
  weathercases_ = weathercases;

  // reloads transmission lines from xml node
  bool status = LoadTransmissionLinesFromXml(node);
  if (status == false) {
    // loading lines failed, so resets to starting condition
    weathercases_ = weathercases_cache;

    lines_.clear();
    LoadTransmissionLinesFromXml(node);
  }

  return status;
}

const std::list<StructureFile>& LineAnalyzerDoc::structures() const {
  return structures_;
}

const std::list<WeatherLoadCase>& LineAnalyzerDoc::weathercases() const {
  return weathercases_;
}

bool LineAnalyzerDoc::IsUniqueWeathercaseName(const std::string& name,
                                              const int& index_ignore) const {
  int index = 0;
  for (auto iter = weathercases_.cbegin(); iter != weathercases_.cend();
       iter++) {
    const WeatherLoadCase& weathercase = *iter;

    if (index == index_ignore) {
      continue;
    } else {
      if (weathercase.description == name) {
        // match is found, name is not unique
        return false;
      }
    }
  }

  return true;
}

bool LineAnalyzerDoc::IsValidIndex(const int& index,
                                   const int& size,
                                   const bool& is_included_end) const {
  if ((0 <= index) && (index < size)) {
    return true;
  } else if ((index == size) && (is_included_end == true)) {
    return true;
  } else {
    return false;
  }
}

void LineAnalyzerDoc::UpdateActiveLineReference() {
  line_active_ = &(*std::next(lines_.begin(), index_active_));
}
