// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_LINEANALYZER_SHAREDDATACOMMANDS_H_
#define OTLS_LINEANALYZER_SHAREDDATACOMMANDS_H_

#include <list>
#include <string>

#include "wx/cmdproc.h"
#include "wx/xml/xml.h"

#include "line_analyzer_doc.h"

/// \par OVERVIEW
///
/// This class is the base command that modifies the shared data in the
/// LineAnalyzerDoc, and is meant to be derived from.
///
/// \par TRANSMISSION LINES XML NODE
///
/// This command stores the document transmission lines the form of an xml node.
/// If the command fails, the document transmission lines can be restored from
/// the xml node. Only the transmission lines are stored in the flattened
/// xml node because the document shared/external data types do not reference
/// other data types, and no pointer references have to be re-established on an
/// undo operation.
class SharedDataCommandBase : public wxCommand {
 public:
  /// \brief Constructor.
  /// \param[in] name
  ///   The command name, which should match one of the defined strings.
  SharedDataCommandBase(const std::string& name);

  /// \brief Destructor.
  virtual ~SharedDataCommandBase();

  /// \brief Executes the command.
  /// \return True if the action has taken place, false otherwise.
  virtual bool Do();

  /// \brief Undoes the command.
  /// \return True if the action has taken place, false otherwise.
  virtual bool Undo();

 protected:
  /// \brief Does the modify command.
  /// \return The success status.
  virtual bool DoModify() = 0;

  /// \var doc_
  ///   The document.
  LineAnalyzerDoc* doc_;

  /// \var node_
  ///   An xml node that contains the state of all transmission lines in the
  ///   document before any edits occur. This can be used to restore the
  ///   document state if the command fails.
  const wxXmlNode* node_;
};


/// \par OVERVIEW
///
/// This class is a command that modifies the CableFile list in the document.
///
/// \par DATA EXCHANGE
///
/// This command stores information and exchanges it with the document with the
/// Do() method. The Undo() method reverses the direction of the exchange.
class CableFileCommand : public SharedDataCommandBase {
 public:
  /// \brief Constructor.
  CableFileCommand();

  /// \brief Destructor.
  virtual ~CableFileCommand();

  /// \brief Sets the cable files.
  /// \param[in] cablefiles
  ///   The cable files.
  void set_cablefiles(const std::list<CableFile>& cablefiles);

  /// \brief Gets the cable files.
  /// \return The cable files.
  const std::list<CableFile> cablefiles() const;

 private:
  /// \brief Does the modify command.
  /// \return The success status.
  /// This function will switch the stored data with the document data.
  virtual bool DoModify();

  /// \var cablefiles_
  ///   The cable file list that will be exchanged with the document.
  std::list<CableFile> cablefiles_;
};


/// \par OVERVIEW
///
/// This class is a command that modifies the HardwareFile list in the document.
///
/// \par DATA EXCHANGE
///
/// This command stores information and exchanges it with the document with the
/// Do() method. The Undo() method reverses the direction of the exchange.
class HardwareFileCommand : public SharedDataCommandBase {
 public:
  /// \brief Constructor.
  HardwareFileCommand();

  /// \brief Destructor.
  virtual ~HardwareFileCommand();

  /// \brief Sets the hardware files.
  /// \param[in] hardwarefiles
  ///   The hardware files.
  void set_hardwarefiles(const std::list<HardwareFile>& hardwarefiles);

  /// \brief Gets the hardware files.
  /// \return The hardware files.
  const std::list<HardwareFile> hardwarefiles() const;

 private:
  /// \brief Does the modify command.
  /// \return The success status.
  /// This function will switch the stored data with the document data.
  virtual bool DoModify();

  /// \var hardwarefiles_
  ///   The hardware file list that will be exchanged with the document.
  std::list<HardwareFile> hardwarefiles_;
};


/// \par OVERVIEW
///
/// This class is a command that modifies the StructureFile list in the document.
///
/// \par DATA EXCHANGE
///
/// This command stores information and exchanges it with the document with the
/// Do() method. The Undo() method reverses the direction of the exchange.
class StructureFileCommand : public SharedDataCommandBase {
 public:
  /// \brief Constructor.
  StructureFileCommand();

  /// \brief Destructor.
  virtual ~StructureFileCommand();

  /// \brief Sets the structure files.
  /// \param[in] structurefiles
  ///   The structure files.
  void set_structurefiles(const std::list<StructureFile>& structurefiles);

  /// \brief Gets the structure files.
  /// \return The structure files.
  const std::list<StructureFile> structurefiles() const;

 private:
  /// \brief Does the modify command.
  /// \return The success status.
  /// This function will switch the stored data with the document data.
  virtual bool DoModify();

  /// \var structurefiles_
  ///   The structure file list that will be exchanged with the document.
  std::list<StructureFile> structurefiles_;
};


/// \par OVERVIEW
///
/// This class is a command that modifies the WeatherLoadCase list in the
/// document.
///
/// \par DATA EXCHANGE
///
/// This command stores information and exchanges it with the document with the
/// Do() method. The Undo() method reverses the direction of the exchange.
class WeatherLoadCaseCommand : public SharedDataCommandBase {
 public:
  /// \brief Constructor.
  WeatherLoadCaseCommand();

  /// \brief Destructor.
  virtual ~WeatherLoadCaseCommand();

  /// \brief Sets the weather cases.
  /// \param[in] weathercases
  ///   The weather cases.
  void set_weathercases(const std::list<WeatherLoadCase>& weathercases);

  /// \brief Gets the weather cases.
  /// \return The weather cases.
  const std::list<WeatherLoadCase> weathercases() const;

 private:
  /// \brief Does the modify command.
  /// \return The success status.
  /// This function will switch the stored data with the document data.
  virtual bool DoModify();

  /// \var weathercases_
  ///   The weather case list that will be exchanged with the document.
  std::list<WeatherLoadCase> weathercases_;
};

#endif  // OTLS_LINEANALYZER_SHAREDDATACOMMANDS_H_
