// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_LINEANALYZER_TRANSMISSIONLINECOMMAND_H_
#define OTLS_LINEANALYZER_TRANSMISSIONLINECOMMAND_H_

#include <string>

#include "wx/cmdproc.h"
#include "wx/xml/xml.h"

#include "line_analyzer_doc.h"

/// \par OVERVIEW
///
/// This class is a command that modifies a TransmissionLine in the document.
///
/// \par NAME
///
/// The command must be initialized with one of the declared names, as it will
/// determine what kind of action is to be performed.
///
/// \par TRANSMISSION LINE XML NODES
///
/// This command stores transmission line xml nodes. This is done so the command
/// can be applied even if the shared data pointers become invalid. Not having
/// to keep memory references valid removes a whole set of potential problems.
///
/// Up to two nodes are stored. One of the nodes will be committed to the
/// document on a 'do' method, while the other will be committed to the document
/// on an 'undo' method. These nodes may not be needed for all command types,
/// and will be kept as a nullptr if not used.
class TransmissionLineCommand : public wxCommand {
 public:
  /// \var kNameActivate
  ///   The command name to use for activating. This command type requires an
  ///   index, but not an xml node.
  static const std::string kNameActivate;

  /// \var kNameDelete
  ///   The command name to use for deleting. This command type requires an
  ///   index, but not an xml node.
  static const std::string kNameDelete;

  /// \var kNameInsert
  ///   The command name to use for inserting. This command type requires an
  ///   index and xml node.
  static const std::string kNameInsert;

  /// \var kNameModify
  ///   The command name to use for modifying. This command requires an xml
  ///   node, but not an index (applied to activated line in doc).
  static const std::string kNameModify;

  /// \var kNameMoveDown
  ///   The command name to use for moving up in the container. This command
  ///   requires an index, but not an xml node.
  static const std::string kNameMoveDown;

  /// \var kNameMoveUp
  ///   The command name to use for moving down in the container. This command
  ///   requires an index, but not an xml node.
  static const std::string kNameMoveUp;

  /// \brief Constructor.
  /// \param[in] name
  ///   The command name, which should match one of the defined strings.
  TransmissionLineCommand(const std::string& name);

  /// \brief Destructor.
  virtual ~TransmissionLineCommand();

  /// \brief Creates a TransmissionLine xml node.
  /// \param[in] line
  ///   The transmission line.
  /// \return A TransmissionLine xml node.
  static wxXmlNode* CreateTransmissionLineXmlNode(const TransmissionLine& line);

  /// \brief Does the command.
  /// \return True if the action has taken place, false otherwise.
  virtual bool Do();

  /// \brief Undoes the command.
  /// \return True if the action has taken place, false otherwise.
  virtual bool Undo();

  /// \brief Gets the index.
  /// \return The index.
  int index() const;

  /// \brief Gets the transmission line xml node.
  /// \return The transmission line xml node.
  const wxXmlNode* node_transmission_line() const;

  /// \brief Sets the index.
  /// \param[in] index
  ///   The index.
  void set_index(const int& index);

  /// \brief Sets the transmission line xml node.
  /// \param[in] node_transmission_line
  ///   The transmission line xml node.
  void set_node_transmission_line(const wxXmlNode* node_transmission_line);

 private:
  /// \brief Does the activate command.
  /// \return The success status.
  bool DoActivate();

  /// \brief Does the delete command.
  /// \return The success status.
  bool DoDelete();

  /// \brief Does the insert command.
  /// \param[in] node
  ///   The node to use when modifying (do/undo).
  /// \return The success status.
  bool DoInsert(const wxXmlNode* node);

  /// \brief Does the modify command.
  /// \param[in] node
  ///   The node to use when modifying (do/undo).
  /// \return The success status.
  bool DoModify(const wxXmlNode* node);

  /// \brief Does the move down command.
  /// \return The success status.
  bool DoMoveDown();

  /// \brief Does the move up command.
  /// \return The success status.
  bool DoMoveUp();

  /// \var doc_
  ///   The document.
  LineAnalyzerDoc* doc_;

  /// \var index_
  ///   The list index in the document. The command will be applied at this
  ///   index.
  int index_;

  /// \var node_do_
  ///   The TransmissionLine xml node that is committed to the document on a do
  ///   operation. This command owns the allocated node.
  const wxXmlNode* node_do_;

  /// \var node_undo_
  ///   The TransmissionLine xml node that is committed to the document on an
  ///   undo operation. It contains the state of the transmission line in the
  ///   document before any edits occur. This commands owns the allocated node.
  const wxXmlNode* node_undo_;
};

#endif  // OTLS_LINEANALYZER_TRANSMISSIONLINECOMMAND_H_
