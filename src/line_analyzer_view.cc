// This is free and unencumbered software released into the public domain.
// For more information, please refer to <http://unlicense.org/>

#include "line_analyzer_view.h"

IMPLEMENT_DYNAMIC_CLASS(LineAnalyzerView, wxView)

LineAnalyzerView::LineAnalyzerView() {
}

LineAnalyzerView::~LineAnalyzerView() {
}

bool LineAnalyzerView::OnCreate(wxDocument *doc, long flags) {
  if (!wxView::OnCreate(doc, flags)) {
    return false;
  }

  return true;
}

bool LineAnalyzerView::OnClose(bool WXUNUSED(deleteWindow)) {
  if (!GetDocument()->Close()) {
    return false;
  }

  return true;
}

void LineAnalyzerView::OnDraw(wxDC *dc) {
}

void LineAnalyzerView::OnUpdate(wxView* sender, wxObject* hint) {
  // passes to base class first
  wxView::OnUpdate(sender, hint);
}
