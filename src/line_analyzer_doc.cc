// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "line_analyzer_doc.h"

#include "appcommon/units/cable_unit_converter.h"
#include "appcommon/units/hardware_unit_converter.h"
#include "appcommon/units/structure_unit_converter.h"
#include "appcommon/units/transmission_line_unit_converter.h"
#include "appcommon/units/weather_load_case_unit_converter.h"
#include "appcommon/widgets/status_bar_log.h"
#include "wx/cmdproc.h"
#include "wx/wx.h"
#include "wx/xml/xml.h"

#include "file_handler.h"
#include "line_analyzer_app.h"
#include "line_analyzer_doc_xml_handler.h"

IMPLEMENT_DYNAMIC_CLASS(LineAnalyzerDoc, wxDocument)

LineAnalyzerDoc::LineAnalyzerDoc() {
  lines_.push_back(TransmissionLine());
  index_active_ = 0;
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

bool LineAnalyzerDoc::DeleteCableFile(const int& index) {
  // checks index
  if (IsValidIndex(index, cables_.size(), false) == false) {
    return false;
  }

  // checks for reference in transmission lines
  if (IsReferencedCableFile(index) == true) {
    return false;
  }

  // gets iterator with edit capability and deletes
  auto iter = std::next(cables_.begin(), index);
  cables_.erase(iter);

  // marks as modified
  Modify(true);

  return true;
}

bool LineAnalyzerDoc::DeleteHardwareFile(const int& index) {
  // checks index
  if (IsValidIndex(index, hardwares_.size(), false) == false) {
    return false;
  }

  // checks for reference in transmission lines
  if (IsReferencedHardwareFile(index) == true) {
    return false;
  }

  // gets iterator with edit capability and deletes
  auto iter = std::next(hardwares_.begin(), index);
  hardwares_.erase(iter);

  // marks as modified
  Modify(true);

  return true;
}

bool LineAnalyzerDoc::DeleteStructureFile(const int& index) {
  // checks index
  if (IsValidIndex(index, structures_.size(), false) == false) {
    return false;
  }

  // checks for reference in transmission lines
  if (IsReferencedStructureFile(index) == true) {
    return false;
  }

  // gets iterator with edit capability and deletes
  auto iter = std::next(structures_.begin(), index);
  structures_.erase(iter);

  // marks as modified
  Modify(true);

  return true;
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

  // marks as modified
  Modify(true);

  return true;
}

bool LineAnalyzerDoc::DeleteWeatherCase(const int& index) {
  // checks index
  if (IsValidIndex(index, weathercases_.size(), false) == false) {
    return false;
  }

  // checks for reference in transmission lines
  if (IsReferencedWeatherCase(index) == true) {
    return false;
  }

  // gets iterator with edit capability and deletes
  auto iter = std::next(weathercases_.begin(), index);
  weathercases_.erase(iter);

  // marks as modified
  Modify(true);

  return true;
}

bool LineAnalyzerDoc::InsertCableFile(const int& index,
                                      const CableFile& cablefile) {
  // checks index
  if (IsValidIndex(index, cables_.size(), true) == false) {
    return false;
  }

  // checks to see if cable file path is already in list
  for (auto iter = cables_.cbegin(); iter != cables_.cend(); iter++) {
    const CableFile& cablefile_list = *iter;
    if (cablefile_list.filepath == cablefile.filepath) {
      // duplicate filepath is found
      return false;
    }
  }

  // gets iterator with edit capability and inserts
  auto iter = std::next(cables_.begin(), index);
  cables_.insert(iter, cablefile);

  // marks as modified
  Modify(true);

  return true;
}

bool LineAnalyzerDoc::InsertHardwareFile(const int& index,
                                         const HardwareFile& hardwarefile) {
  // checks index
  if (IsValidIndex(index, hardwares_.size(), true) == false) {
    return false;
  }

  // checks to see if hardware file path is already in list
  for (auto iter = hardwares_.cbegin(); iter != hardwares_.cend(); iter++) {
    const HardwareFile& hardwarefile_list = *iter;
    if (hardwarefile_list.filepath == hardwarefile.filepath) {
      // duplicate filepath is found
      return false;
    }
  }

  // gets iterator with edit capability and inserts
  auto iter = std::next(hardwares_.begin(), index);
  hardwares_.insert(iter, hardwarefile);

  // marks as modified
  Modify(true);

  return true;
}

bool LineAnalyzerDoc::InsertStructureFile(const int& index,
                                          const StructureFile& structurefile) {
  // checks index
  if (IsValidIndex(index, hardwares_.size(), true) == false) {
    return false;
  }

  // checks to see if structure file path is already in list
  for (auto iter = structures_.cbegin(); iter != structures_.cend(); iter++) {
    const StructureFile& structurefile_list = *iter;
    if (structurefile_list.filepath == structurefile.filepath) {
      // duplicate filepath is found
      return false;
    }
  }

  // gets iterator with edit capability and inserts
  auto iter = std::next(structures_.begin(), index);
  structures_.insert(iter, structurefile);

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

bool LineAnalyzerDoc::InsertWeatherCase(const int& index,
                                        const WeatherLoadCase& weathercase) {
  // checks index
  if (IsValidIndex(index, weathercases_.size(), true) == false) {
    return false;
  }

  // checks for unique name
  if (IsUniqueWeathercaseName(weathercase.description, index) == false) {
    return false;
  }

  // gets iterator with edit capability and inserts
  auto iter = std::next(weathercases_.begin(), index);
  weathercases_.insert(iter, weathercase);

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

  // resets modified status to false because the xml parser uses functions
  // that mark it as modified
  Modify(false);

  status_bar_log::PopText(0);

  return stream;
}

bool LineAnalyzerDoc::ModifyCableFile(const int& index,
                                      const CableFile& cablefile) {
  // checks index
  if (IsValidIndex(index, cables_.size(), false) == false) {
    return false;
  }

  // gets an iterator with edit capability and modifies
  auto iter = std::next(cables_.begin(), index);
  *iter = cablefile;

  // updates external file
  FileHandler::SaveCable(cablefile.filepath, cablefile.cable,
                          wxGetApp().config()->units);

  return true;
}

bool LineAnalyzerDoc::ModifyHardwareFile(const int& index,
                                         const HardwareFile& hardwarefile) {
  // checks index
  if (IsValidIndex(index, hardwares_.size(), false) == false) {
    return false;
  }

  // gets an iterator with edit capability and modifies
  auto iter = std::next(hardwares_.begin(), index);
  *iter = hardwarefile;

  // updates external file
  FileHandler::SaveHardware(hardwarefile.filepath, hardwarefile.hardware,
                            wxGetApp().config()->units);

  return true;
}

bool LineAnalyzerDoc::ModifyStructureFile(const int& index,
                                          const StructureFile& structurefile) {
  // checks index
  if (IsValidIndex(index, hardwares_.size(), false) == false) {
    return false;
  }

  // gets an iterator with edit capability and modifies
  auto iter = std::next(structures_.begin(), index);
  *iter = structurefile;

  // searches all transmission lines for affected structures
  const Structure* structure = &(*iter).structure;
  for (auto iter = lines_.begin(); iter != lines_.end(); iter++) {
    TransmissionLine& line = *iter;

    // searches all line structures
    int index = 0;
    for (auto it = line.line_structures()->cbegin();
         it != line.line_structures()->cend(); it++) {
      const LineStructure& line_structure = *it;

      // the line structure isn't modified, but this will trigger the
      // transmission line to clean up connections if needed
      if (line_structure.structure() == structure) {
        LineStructure line_structure_mod = line_structure;
        line.ModifyLineStructure(index, line_structure_mod);
      }

      index++;
    }

  }

  /// \todo Need to figure out how to implement this change
  ///  if attachment change with the modified structure, it could break all
  ///  of the line structure hardware and line cables that are attached

  // updates external file
  FileHandler::SaveStructure(structurefile.filepath, structurefile.structure,
                              wxGetApp().config()->units);

  return true;
}

bool LineAnalyzerDoc::ModifyWeatherCase(const int& index,
                                        const WeatherLoadCase& weathercase) {
  // checks index
  if (IsValidIndex(index, hardwares_.size(), false) == false) {
    return false;
  }

  // checks for unique name
  if (IsUniqueWeathercaseName(weathercase.description, index) == false) {
    return false;
  }

  // gets an iterator with edit capability and modifies
  auto iter = std::next(weathercases_.begin(), index);
  *iter = weathercase;

  return true;
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

const std::list<CableFile>& LineAnalyzerDoc::cables() const {
  return cables_;
}

const std::list<HardwareFile>& LineAnalyzerDoc::hardwares() const {
  return hardwares_;
}

int LineAnalyzerDoc::index_active() const {
  return index_active_;
}

const std::list<TransmissionLine>& LineAnalyzerDoc::lines() const {
  return lines_;
}

bool LineAnalyzerDoc::set_index_active(const int& index) {
  if (IsValidIndex(index, lines_.size(), false) == true) {
    index_active_ = index;
    return true;
  } else {
    return false;
  }
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
