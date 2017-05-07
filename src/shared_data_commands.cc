// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "shared_data_commands.h"

#include "wx/wx.h"

#include "line_analyzer_app.h"

SharedDataCommandBase::SharedDataCommandBase(const std::string& name)
    : wxCommand(true, name) {
  doc_ = wxGetApp().GetDocument();
  node_ = nullptr;
}

SharedDataCommandBase::~SharedDataCommandBase() {
  delete node_;
}

bool SharedDataCommandBase::Do() {
  // saves the document state of the transmission lines
  node_ = doc_->SaveTransmissionLinesToXml();

  // swaps command and document data
  DoModify();

  // posts a view update
  UpdateHint hint(HintType::kSharedDataEdit);
  doc_->UpdateAllViews(nullptr, &hint);

  return true;
}

bool SharedDataCommandBase::Undo() {
  // swaps command and document data
  DoModify();

  // restores the previous document state for the transmission lines
  const bool status = doc_->LoadTransmissionLinesFromXml(node_);

  // posts a view update
  UpdateHint hint(HintType::kSharedDataEdit);
  doc_->UpdateAllViews(nullptr, &hint);

  return status;
}


CableFileCommand::CableFileCommand()
    : SharedDataCommandBase("Modify Cable Files") {
}

CableFileCommand::~CableFileCommand() {
}

void CableFileCommand::set_cablefiles(const std::list<CableFile>& cablefiles) {
  cablefiles_ = cablefiles;
}

const std::list<CableFile> CableFileCommand::cablefiles() const {
  return cablefiles_;
}

bool CableFileCommand::DoModify() {
  // swaps the data from the document and the command
  std::list<CableFile> cablefiles_tmp = doc_->cables();
  doc_->set_cables(cablefiles_);
  cablefiles_ = cablefiles_tmp;

  return true;
}


HardwareFileCommand::HardwareFileCommand()
    : SharedDataCommandBase("Modify Hardware Files") {
}

HardwareFileCommand::~HardwareFileCommand() {
}

void HardwareFileCommand::set_hardwarefiles(
    const std::list<HardwareFile>& hardwarefiles) {
  hardwarefiles_ = hardwarefiles;
}

const std::list<HardwareFile> HardwareFileCommand::hardwarefiles() const {
  return hardwarefiles_;
}

bool HardwareFileCommand::DoModify() {
  // swaps the data from the document and the command
  std::list<HardwareFile> hardwarefiles_tmp = doc_->hardwares();
  doc_->set_hardwares(hardwarefiles_);
  hardwarefiles_ = hardwarefiles_tmp;

  return true;
}


StructureFileCommand::StructureFileCommand()
    : SharedDataCommandBase("Modify Structure Files") {
}

StructureFileCommand::~StructureFileCommand() {
}

void StructureFileCommand::set_structurefiles(
    const std::list<StructureFile>& structurefiles) {
  structurefiles_ = structurefiles;
}

const std::list<StructureFile> StructureFileCommand::structurefiles() const {
  return structurefiles_;
}

bool StructureFileCommand::DoModify() {
  // swaps the data from the document and the command
  std::list<StructureFile> structurefiles_tmp = doc_->structures();
  doc_->set_structures(structurefiles_);
  structurefiles_ = structurefiles_tmp;

  return true;
}


WeatherLoadCaseCommand::WeatherLoadCaseCommand()
    : SharedDataCommandBase("Modify Weather Cases") {
}

WeatherLoadCaseCommand::~WeatherLoadCaseCommand() {
}

void WeatherLoadCaseCommand::set_weathercases(
    const std::list<WeatherLoadCase>& weathercases) {
  weathercases_ = weathercases;
}

const std::list<WeatherLoadCase> WeatherLoadCaseCommand::weathercases() const {
  return weathercases_;
}

bool WeatherLoadCaseCommand::DoModify() {
  // swaps the data from the document and the command
  std::list<WeatherLoadCase> weathercases_tmp = doc_->weathercases();
  doc_->set_weathercases(weathercases_);
  weathercases_ = weathercases_tmp;

  return true;
}
