// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_LINEANALYZER_LINEANALYZERDOC_H_
#define OTLS_LINEANALYZER_LINEANALYZERDOC_H_

#include <list>

#include "models/base/units.h"
#include "models/transmissionline/transmission_line.h"
#include "wx/docview.h"

/// This enum class contains types of update hints.
enum class HintType {
};

/// \par OVERVIEW
///
/// This class is a used when updating the view associated with the document.
class UpdateHint : public wxObject {
 public:
  /// \brief Default Constructor.
  UpdateHint() {};

  /// \brief Alternate constructor.
  UpdateHint(HintType hint) {type_ = hint;};

  /// \brief Sets the hint type.
  /// \param[in] type
  ///   The hint type.
  void set_type(const HintType& type) {type_ = type;};

  /// \brief Gets the hint type.
  /// \return The hint type.
  const HintType& type() const {return type_;};

 private:
  /// \var type_
  ///   The hint type.
  HintType type_;
};

/// \par OVERVIEW
///
/// This struct pairs a cable with an external file.
struct CableFile {
 public:
  /// \var cable
  ///   The cable.
  Cable cable;

  /// \var filepath
  ///   The file path.
  std::string filepath;
};

/// \par OVERVIEW
///
/// This struct pairs hardware with an external file.
struct HardwareFile {
 public:
  /// \var hardware
  ///   The hardware.
  Hardware hardware;

  /// \var filepath
  ///   The file path.
  std::string filepath;
};

/// \par OVERVIEW
///
/// This struct pairs a structure iwth an external file.
struct StructureFile {
  /// \var filepath
  ///   The file path.
  std::string filepath;

  /// \var structure
  ///   The structure.
  Structure structure;
};

/// \par OVERVIEW
///
/// This is a LineAnalyzer application document, which holds all of the
/// information for a LineAnalyzer project file.
///
/// \par TRANSMISSION LINES
///
/// The primary data that this document stores is transmission lines. Many lines
/// can be stored, so different variants or optimizations can be modeled in a
/// single document.
///
/// Only one transmission line can be selected (activated) for editing. This
/// document will always maintain an active transmission line, even if creating
/// one is necessary. The other transmission lines are still exposed for
/// modeling, but edits cannot be directed to them.
///
/// \par SHARED DATA AND EXTERNAL FILES
///
/// This document also stores shared data that can be used to build or model
/// transmission lines. This data is available for reference to all transmission
/// lines in the document.
///
/// Some of shared data may originate from external files. This document loads
/// the data from the external files when the document is loaded. When the data
/// that originates in an external file is modified, it is immediately saved.
/// When data that originates from the document is modified, it is saved when
/// the rest of the document is. Document file saves will not overwrite external
/// files.
///
/// Each item in the shared data sets must be unique in some way. This allows
/// the references to be re-established when the document is saved and loaded
/// again from a file. For shared data that originates from an external file,
/// the file path is the unique identifier. For shared data that is stored in
/// the document, the name of the individual item is the unique identifier.
///
/// The shared data types, along with the file ownership and unique identifier
/// types are:
/// - cables       (ownership = external, id = filepath)
/// - hardware     (ownership = external, id = filepath)
/// - structures   (ownership = external, id = filepath)
/// - weathercases (ownership = document, id = name)
///
/// \par wxWIDGETS LIBRARY BUILD NOTE
///
/// This class requires that the wxWidgets library deviate from the standard
/// build. The wxUSE_STD_IOSTREAM configuration flag must be set to 0, which
/// will force wxWidgets to generate wxInputStream/wxOutputStream classes on the
/// LoadObject() and SaveObject() functions. This is required because this class
/// uses a series of XML handlers to load/save the document, and wxWidgets does
/// not allow std::streams to be used with the XmlDocument::Load() and
/// XmlDocument::Save() functions.
class LineAnalyzerDoc : public wxDocument {
 public:
  /// \brief Constructor.
  LineAnalyzerDoc();

  /// \brief Destructor.
  ~LineAnalyzerDoc();

  /// \brief Converts the document between unit styles.
  /// \param[in] system
  ///   The unit system.
  /// \param[in] style_from
  ///   The unit style to convert from.
  /// \param[in] style_to
  ///   The unit style to convert to.
  void ConvertUnitStyle(const units::UnitSystem& system,
                        const units::UnitStyle& style_from,
                        const units::UnitStyle& style_to);

  /// \brief Converts the document between unit systems.
  /// \param[in] system_from
  ///   The unit system to convert from.
  /// \param[in] system_to
  ///   The unit system to convert to.
  /// This function requires that the document is in a 'consistent' unit
  /// style.
  void ConvertUnitSystem(const units::UnitSystem& system_from,
                         const units::UnitSystem& system_to);

  /// \brief Deletes a cable file.
  /// \param[in] index
  ///   The list index.
  /// \return Success status.
  /// Cable files can't be removed from the document if referenced by a
  /// transmission line.
  bool DeleteCableFile(const int& index);

  /// \brief Deletes a hardware file.
  /// \param[in] index
  ///   The list index.
  /// \return Success status.
  /// Hardware files can't be removed from the document if referenced by a
  /// transmission line.
  bool DeleteHardwareFile(const int& index);

  /// \brief Deletes a structure file.
  /// \param[in] index
  ///   The list index.
  /// \return Success status.
  /// Structure files can't be removed from the document if referenced by a
  /// transmission line.
  bool DeleteStructureFile(const int& index);

  /// \brief Deletes a transmission line.
  /// \param[in] index
  ///   The list index.
  /// \return Success status.
  bool DeleteTransmissionLine(const int& index);

  /// \brief Deletes a weather case.
  /// \param[in] index
  ///   The list index.
  /// \return Success status.
  /// Weather cases can't be removed from the document if referenced by a
  /// transmission line.
  bool DeleteWeatherCase(const int& index);

  /// \brief Inserts a cable file.
  /// \param[in] index
  ///   The list index to insert before.
  /// \param[in] cablefile
  ///   The the cable file.
  /// \return Success status.
  bool InsertCableFile(const int& index, const CableFile& cablefile);

  /// \brief Inserts a transmission line.
  /// \param[in] index
  ///   The list index to insert before.
  /// \param[in] hardwarefile
  ///   The hardware file.
  /// \return Success status.
  bool InsertHardwareFile(const int& index, const HardwareFile& hardwarefile);

  /// \brief Inserts a structure file.
  /// \param[in] index
  ///   The list index to insert before.
  /// \param[in] structurefile
  ///   The structure file.
  /// \return Success status.
  bool InsertStructureFile(const int& index,
                           const StructureFile& structurefile);

  /// \brief Inserts a transmission line.
  /// \param[in] index
  ///   The list index to insert before.
  /// \param[in] line
  ///   The transmission line.
  /// \return Success status.
  bool InsertTransmissionLine(const int& index, const TransmissionLine& line);

  /// \brief Inserts a weather case.
  /// \param[in] index
  ///   The list index to insert before.
  /// \param[in] weathercase
  ///   The weathercase.
  /// \return Success status.
  bool InsertWeatherCase(const int& index, const WeatherLoadCase& weathercase);

  /// \brief Determines if the cable file is referenced.
  /// \param[in] index
  ///   The list index.
  /// \return If the cable file is referenced.
  bool IsReferencedCableFile(const int& index) const;

  /// \brief Determines if the hardware file is referenced.
  /// \param[in] index
  ///   The list index.
  /// \return If the hardware file is referenced.
  bool IsReferencedHardwareFile(const int& index) const;

  /// \brief Determines if the structure file is referenced.
  /// \param[in] index
  ///   The list index.
  /// \return If the structure file is referenced.
  bool IsReferencedStructureFile(const int& index) const;

  /// \brief Determines if the weather case is referenced.
  /// \param[in] index
  ///   The list index.
  /// \return If the weather case is referenced.
  bool IsReferencedWeatherCase(const int& index) const;

  /// \brief Loads the document.
  /// \param[in] stream
  ///   The input stream generated by wxWidgets.
  /// \return The input stream.
  wxInputStream& LoadObject(wxInputStream& stream);

  /// \brief Modifies a cable file.
  /// \param[in] index
  ///   The list index.
  /// \param[in] cablefile
  ///   The cable file.
  /// \return Success status.
  /// The external file will be overwritten with the new data.
  bool ModifyCableFile(const int& index, const CableFile& cablefile);

  /// \brief Modifies a hardware file.
  /// \param[in] index
  ///   The list index.
  /// \param[in] hardwarefile
  ///   The hardware file.
  /// \return Success status.
  /// The external file will be overwritten with the new data.
  bool ModifyHardwareFile(const int& index, const HardwareFile& hardwarefile);

  /// \brief Modifies a structure file.
  /// \param[in] index
  ///   The list index.
  /// \param[in] structurefile
  ///   The structure file.
  /// \return Success status.
  /// The external file will be overwritten with the new data.
  bool ModifyStructureFile(const int& index,
                           const StructureFile& structurefile);

  /// \brief Modifies a weathercase.
  /// \param[in] index
  ///   The list index.
  /// \param[in] weathercase
  ///   The weathercase.
  /// \return Success status.
  bool ModifyWeatherCase(const int& index, const WeatherLoadCase& weathercase);

  /// \brief Moves the transmission line within the list.
  /// \param[in] index_from
  ///   The index item to move.
  /// \param[in] index_to
  ///   The index to move before.
  /// \return Success status.
  bool MoveTransmissionLine(const int& index_from, const int& index_to);

  /// \brief Initializes the document.
  /// \param[in] path
  ///   The document filepath.
  /// \param[in] flags
  ///   Document creation flags.
  /// \return If the document is created successfully.
  /// This is called by the wxWidgets framework. All defaults are still being
  /// kept, this function is just being added to and treated as an
  /// initialization constructor.
  virtual bool OnCreate(const wxString& path, long flags);

  /// \brief Saves the document.
  /// \param[out] stream
  ///   The output stream generated by wxWidgets.
  /// \return The output stream.
  wxOutputStream& SaveObject(wxOutputStream& stream);

  /// \brief Gets the cable files.
  /// \return The cable files.
  const std::list<CableFile>& cables() const;

  /// \brief Gets the hardware files.
  /// \return The hardware files.
  const std::list<HardwareFile>& hardwares() const;

  /// \brief Gets the active transmission line index.
  /// \return The active transmission line index
  int index_active() const;

  /// \brief Gets the transmission lines.
  /// \return The transmission lines.
  const std::list<TransmissionLine>& lines() const;

  /// \brief Sets the active transmission line.
  /// \param[in] index
  ///   The list index.
  /// \return Success status.
  bool set_index_active(const int& index);

  /// \brief Gets the structure files.
  /// \return The structure files.
  const std::list<StructureFile>& structures() const;

  /// \brief Gets the weathercases.
  /// \return The weathercases.
  const std::list<WeatherLoadCase>& weathercases() const;

 private:
  /// \brief Determines if the weathercase name is unique.
  /// \param[in] name
  ///   The name.
  /// \param[in] index_ignore
  ///   The list index to ignore.
  /// \return If the name is unique.
  bool IsUniqueWeathercaseName(const std::string& name,
                               const int& index_ignore) const;

  /// \brief Determines if the index is valid.
  /// \param[in] index
  ///   The list index.
  /// \param[in] size
  ///   The size of the container.
  /// \param[in] is_included_end
  ///   An indicator that tells if an extra index corresponding to the end
  ///   iterator position is valid.
  /// \return If the index is valid.
  bool IsValidIndex(const int& index, const int& size,
                    const bool& is_included_end) const;

  /// \var cables_
  ///   The cable files.
  std::list<CableFile> cables_;

  /// \var hardwares_
  ///   The hardware files.
  std::list<HardwareFile> hardwares_;

  /// \var index_active_
  ///   The index of the active transmission line.
  int index_active_;

  /// \var lines_
  ///   The transmission lines.
  std::list<TransmissionLine> lines_;

  /// \var structures_
  ///   The structure files.
  std::list<StructureFile> structures_;

  /// \var weathercases_
  ///   The weathercases.
  std::list<WeatherLoadCase> weathercases_;

  /// \brief This allows wxWidgets to create this class dynamically as part of
  ///   the docview framework.
  wxDECLARE_DYNAMIC_CLASS(LineAnalyzerDoc);
};

#endif  // OTLS_LINEANALYZER_LINEANALYZERDOC_H_
