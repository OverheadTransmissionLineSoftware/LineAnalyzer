// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#ifndef OTLS_LINEANALYZER_LINEANALYZERCONFIG_H_
#define OTLS_LINEANALYZER_LINEANALYZERCONFIG_H_

#include <string>

#include "models/base/units.h"
#include "wx/wx.h"

/// \par OVERVIEW
///
/// This struct stores the application configuration settings.
struct LineAnalyzerConfig {
 public:
  /// \var level_log
  ///   The log level of the application.
  wxLogLevelValues level_log;

  /// \var perspective
  ///   The AUI perspective, which is based on the user layout of panes. The
  ///   perspective is loaded/saved on application startup/exit, and modified
  ///   on view open/close.
  std::string perspective;

  /// \var size_frame
  ///   The main application frame size.
  wxSize size_frame;

  /// \var units
  ///   The measurement unit system.
  units::UnitSystem units;
};

#endif  // OTLS_LINEANALYZER_LINEANALYZERCONFIG_H_
