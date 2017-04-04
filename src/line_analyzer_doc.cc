// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "line_analyzer_doc.h"

IMPLEMENT_DYNAMIC_CLASS(LineAnalyzerDoc, wxDocument)

LineAnalyzerDoc::LineAnalyzerDoc() {
}

LineAnalyzerDoc::~LineAnalyzerDoc() {
}

wxInputStream& LineAnalyzerDoc::LoadObject(wxInputStream& stream) {
  return stream;
}

bool LineAnalyzerDoc::OnCreate(const wxString& path, long flags) {
  // calls base class function
  return wxDocument::OnCreate(path, flags);
}

wxOutputStream& LineAnalyzerDoc::SaveObject(wxOutputStream& stream) {
  return stream;
}
