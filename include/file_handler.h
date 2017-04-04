// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_LINEANALYZER_FILEHANDLER_H_
#define OTLS_LINEANALYZER_FILEHANDLER_H_

#include "line_analyzer_config.h"

/// \par OVERVIEW
///
/// This class handles reading and writing application files that are not
/// document related.
class FileHandler {
 public:
  /// \brief Constructor.
  FileHandler();

  /// \brief Destructor.
  ~FileHandler();

  /// \brief Loads the config file.
  /// \param[in] filepath
  ///   The filepath.
  /// \param[out] config
  ///   The application configuration settings.
  /// \return 0 if no errors, -1 if file related errors, or 1 if parsing
  ///   errors are encountered.
  /// All errors are logged to the active application log target.
  static int LoadConfigFile(const std::string& filepath,
                            LineAnalyzerConfig& config);

  /// \brief Saves the application configuration file.
  /// \param[in] filepath
  ///   The filepath.
  /// \param[in] config
  ///   The application configuration settings.
  /// This function will create a file in the application directory.
  static void SaveConfigFile(const std::string& filepath,
                             const LineAnalyzerConfig& config);
};

#endif  // OTLS_LINEANALYZER_FILEHANDLER_H_
