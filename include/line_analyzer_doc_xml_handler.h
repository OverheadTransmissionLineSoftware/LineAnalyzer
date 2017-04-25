// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_LINEANALYZER_LINEANALYZERDOCXMLHANDLER_H_
#define OTLS_LINEANALYZER_LINEANALYZERDOCXMLHANDLER_H_

#include <list>

#include "appcommon/xml/xml_handler.h"
#include "models/base/units.h"

#include "line_analyzer_doc.h"

/// \par OVERVIEW
///
/// This class parses and generates a document XML node. The data is transferred
/// between the XML node and the data object.
///
/// \par VERSION
///
/// This class can parse all versions of the XML node. However, new nodes will
/// only be generated with the most recent version.
///
/// \par UNIT ATTRIBUTES
///
/// This class supports attributing the child XML nodes for various unit
/// systems.
///
/// \par EXTERNAL FILES
///
/// The document owns data that is stored in external files. Only the file
/// path of the data is stored in the xml node.
class LineAnalyzerDocXmlHandler : public XmlHandler {
 public:
  /// \brief Creates an XML node for a document.
  /// \param[in] doc
  ///   The document.
  /// \param[in] units
  ///   The unit system, which is used for attributing child XML nodes.
  /// \return An XML node for the document.
  static wxXmlNode* CreateNode(const LineAnalyzerDoc& doc,
                               const units::UnitSystem& units);

  /// \brief Parses an XML node and populates a document.
  /// \param[in] root
  ///   The XML root node for the document.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[in] units
  ///   The unit system, which is used by the filehandler to convert external
  ///   files (ex: cable files) as they are loaded into the data. Data that is
  ///   stored internally in the data file is NOT converted.
  /// \param[out] doc
  ///   The document that is populated.
  /// \return The status of the xml node parse. If any errors are encountered
  ///   false is returned.
  /// All errors are logged to the active application log target. Critical
  /// errors cause the parsing to abort. Non-critical errors set the object
  /// property to an invalid state (if applicable).
  static bool ParseNode(const wxXmlNode* root,
                        const std::string& filepath,
                        const units::UnitSystem& units,
                        LineAnalyzerDoc& doc);

 private:
  /// \brief Parses a version 1 XML node and populates a document.
  /// \param[in] root
  ///   The XML root node for the document.
  /// \param[in] filepath
  ///   The filepath that the xml node was loaded from. This is for logging
  ///   purposes only and can be left blank.
  /// \param[in] units
  ///   The unit system, which is used by the filehandler to convert external
  ///   files (ex: cable files) as they are loaded into the data. Data that is
  ///   stored internally in the data file is NOT converted.
  /// \param[out] doc
  ///   The document that is populated.
  /// \return The status of the xml node parse. If any errors are encountered
  ///   false is returned.
  /// All errors are logged to the active application log target. Critical
  /// errors cause the parsing to abort. Non-critical errors set the object
  /// property to an invalid state (if applicable).
  static bool ParseNodeV1(const wxXmlNode* root,
                          const std::string& filepath,
                          const units::UnitSystem& units,
                          LineAnalyzerDoc& doc);
};

#endif  // OTLS_LINEANALYZER_LINEANALYZERDOCXMLHANDLER_H_
