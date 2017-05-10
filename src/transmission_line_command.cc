// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "transmission_line_command.h"

#include "appcommon/xml/transmission_line_xml_handler.h"
#include "wx/wx.h"

#include "line_analyzer_app.h"

// These are the string definitions for the various command types.
const std::string TransmissionLineCommand::kNameActivate = "Activate Line";
const std::string TransmissionLineCommand::kNameDelete = "Delete Line";
const std::string TransmissionLineCommand::kNameInsert = "Insert Line";
const std::string TransmissionLineCommand::kNameModify = "Modify Line";
const std::string TransmissionLineCommand::kNameMoveDown = "Move Line Down";
const std::string TransmissionLineCommand::kNameMoveUp = "Move Line Up";

TransmissionLineCommand::TransmissionLineCommand(const std::string& name) {
  doc_ = wxGetApp().GetDocument();
  index_ = -1;
  node_do_ = nullptr;
  node_undo_ = nullptr;
}

TransmissionLineCommand::~TransmissionLineCommand() {
  delete node_do_;
  delete node_undo_;
}

wxXmlNode* TransmissionLineCommand::CreateTransmissionLineXmlNode(
    const TransmissionLine& line) {
  units::UnitSystem units = wxGetApp().config()->units;

  return TransmissionLineXmlHandler::CreateNode(line, "", units);;
}

bool TransmissionLineCommand::Do() {
  bool status = false;

  // selects based on command name
  const std::string name = GetName();
  if (name == kNameActivate) {
    status = DoActivate();
  } else if (name == kNameDelete) {
    // saves the document state of the transmission lines
    node_undo_ = doc_->SaveTransmissionLineToXml();

    status = DoDelete();
  } else if (name == kNameInsert) {
    status = DoInsert();
  } else if (name == kNameModify) {
    status = doc_->LoadTransmissionLineFromXml(node_undo_);
  } else if (name == kNameMoveDown) {
    status = DoMoveDown();
  } else if (name == kNameMoveUp) {
    status = DoMoveUp();
  } else {
    status = false;

    // logs error and restores previous document state
    wxLogError("Invalid command. Aborting.");
    Undo();

    return status;
  }

  // checks if command succeeded
  if (status == true) {
    // posts a view update
    UpdateHint hint(HintType::kTransmissionLineEdit);
    doc_->UpdateAllViews(nullptr, &hint);
  } else {
    // logs error and restores previous document state
    wxLogError("Errors were encountered when executing command.");
    Undo();
  }

  return status;
}

bool TransmissionLineCommand::Undo() {
  bool status = false;

  // selects based on command name
  const std::string name = GetName();
  if (name == kNameActivate) {
    status = DoActivate();
  } else if (name == kNameDelete) {
    status = DoDelete();
  } else if (name == kNameInsert) {
    status = DoInsert();
  } else if (name == kNameModify) {
    doc_->LoadTransmissionLinesFromXml(node_undo_);
  } else if (name == kNameMoveDown) {
    status = DoMoveDown();
  } else if (name == kNameMoveUp) {
    status = DoMoveUp();
  } else {
    status = false;

    // logs error and restores previous document state
    wxLogError("Invalid undo command. Aborting.");
    Undo();

    return status;
  }

  // posts a view update
  UpdateHint hint(HintType::kTransmissionLineEdit);
  doc_->UpdateAllViews(nullptr, &hint);

  return status;
}

int TransmissionLineCommand::index() const {
  return index_;
}

const wxXmlNode* TransmissionLineCommand::node_transmission_line() const {
  return node_do_;
}

void TransmissionLineCommand::set_index(const int& index) {
  index_ = index;
}

void TransmissionLineCommand::set_node_transmission_line(
    const wxXmlNode* node_transmission_line) {
  node_do_ = node_transmission_line;
}

bool TransmissionLineCommand::DoActivate() {
  // checks index
  if (doc_->IsValidIndex(index_, doc_->lines().size(), false) == false) {
    wxLogError("Invalid index. Aborting activate command.");
    return false;
  }

  // caches the current index
  const int index = doc_->index_active();

  // activates the line
  doc_->set_index_active(index_);

  // saves the old index
  index_ = index;

  return true;
}

bool TransmissionLineCommand::DoDelete() {
  // checks index
  if (doc_->IsValidIndex(index_, doc_->lines().size(), false) == false) {
    wxLogError("Invalid index. Aborting delete command.");
    return false;
  }

  // deletes from document
  return doc_->DeleteTransmissionLine(index_);
}

bool TransmissionLineCommand::DoInsert() {
  // checks index
  if (doc_->IsValidIndex(index_, doc_->lines().size(), true) == false) {
    wxLogError("Invalid index. Aborting insert command.");
    return false;
  }

  /// \todo implement this
  return true;
}

bool TransmissionLineCommand::DoModify() {
  // checks index
  if (doc_->IsValidIndex(index_, doc_->lines().size(), true) == false) {
    wxLogError("Invalid index. Aborting insert command.");
    return false;
  }

  /// \todo implement this
  return true;
}

bool TransmissionLineCommand::DoMoveDown() {
  // checks index, and checks against the last valid index
  const int kSize = doc_->lines().size();
  if ((doc_->IsValidIndex(index_, kSize, false) == false)
      || (index_ == kSize - 1)) {
    wxLogError("Invalid index. Aborting move down command.");
    return false;
  }

  // swaps within the document
  return doc_->MoveTransmissionLine(index_, index_ + 2);
}

bool TransmissionLineCommand::DoMoveUp() {
  // checks index, and checks against the first valid index
  if ((doc_->IsValidIndex(index_, doc_->lines().size(), false) == false)
      || (index_ == 0)) {
    wxLogError("Invalid index. Aborting move down command.");
    return false;
  }

  // swaps the within the document
  return doc_->MoveTransmissionLine(index_, index_ - 1);
}
