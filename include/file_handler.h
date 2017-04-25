// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_LINEANALYZER_FILEHANDLER_H_
#define OTLS_LINEANALYZER_FILEHANDLER_H_

#include "models/transmissionline/cable.h"
#include "models/transmissionline/hardware.h"
#include "models/transmissionline/structure.h"

#include "line_analyzer_config.h"

/// \par OVERVIEW
///
/// This class handles reading and writing application files that are not
/// document related.
class FileHandler {
 public:
  /// \brief Loads a cable file.
  /// \param[in] filepath
  ///   The filepath.
  /// \param[in] units
  ///   The unit system to convert to.
  /// \param[out] cable
  ///   The cable that is populated.
  /// \return 0 if no errors, -1 if file related errors, or 1 if parsing
  ///   errors are encountered.
  /// All errors are logged to the active application log target.
  static int LoadCable(const std::string& filepath,
                       const units::UnitSystem& units,
                       Cable& cable);

  /// \brief Loads the config file.
  /// \param[in] filepath
  ///   The filepath.
  /// \param[out] config
  ///   The application configuration settings.
  /// \return 0 if no errors, -1 if file related errors, or 1 if parsing
  ///   errors are encountered.
  /// All errors are logged to the active application log target.
  static int LoadConfig(const std::string& filepath,
                        LineAnalyzerConfig& config);

  /// \brief Loads a hardware file.
  /// \param[in] filepath
  ///   The filepath.
  /// \param[in] units
  ///   The unit system to convert to.
  /// \param[out] hardware
  ///   The hardware that is populated.
  /// \return 0 if no errors, -1 if file related errors, or 1 if parsing
  ///   errors are encountered.
  /// All errors are logged to the active application log target.
  static int LoadHardware(const std::string& filepath,
                          const units::UnitSystem& units,
                          Hardware& hardware);

  /// \brief Loads a structure file.
  /// \param[in] filepath
  ///   The filepath.
  /// \param[in] units
  ///   The unit system to convert to.
  /// \param[out] structure
  ///   The structure that is populated.
  /// \return 0 if no errors, -1 if file related errors, or 1 if parsing
  ///   errors are encountered.
  /// All errors are logged to the active application log target.
  static int LoadStructure(const std::string& filepath,
                           const units::UnitSystem& units,
                           Structure& structure);

  /// \brief Saves a cable file.
  /// \param[in] filepath
  ///   The filepath.
  /// \param[in] cable
  ///   The cable.
  /// \param[in] units
  ///   The current unit system, which will be labeled in the file.
  /// This function will create a copy of the cable before converting the
  /// unit style so the cable will not be modified.
  static void SaveCable(const std::string& filepath, const Cable& cable,
                        const units::UnitSystem& units);

  /// \brief Saves a hardware file.
  /// \param[in] filepath
  ///   The filepath.
  /// \param[in] hardware
  ///   The hardware.
  /// \param[in] units
  ///   The current unit system, which will be labeled in the file.
  /// This function will create a copy of the hardware before converting the
  /// unit style so the hardware will not be modified.
  static void SaveHardware(const std::string& filepath,
                           const Hardware& hardware,
                           const units::UnitSystem& units);

  /// \brief Saves a structure file.
  /// \param[in] filepath
  ///   The filepath.
  /// \param[in] structure
  ///   The structure.
  /// \param[in] units
  ///   The current unit system, which will be labeled in the file.
  /// This function will create a copy of the structure before converting the
  /// unit style so the structure will not be modified.
  static void SaveStructure(const std::string& filepath,
                            const Structure& structure,
                            const units::UnitSystem& units);

  /// \brief Saves the application configuration file.
  /// \param[in] filepath
  ///   The filepath.
  /// \param[in] config
  ///   The application configuration settings.
  /// This function will create a file in the application directory.
  static void SaveConfig(const std::string& filepath,
                         const LineAnalyzerConfig& config);
};

#endif  // OTLS_LINEANALYZER_FILEHANDLER_H_
