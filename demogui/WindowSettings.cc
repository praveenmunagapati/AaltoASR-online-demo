
#include "WindowSettings.hh"
#include "str.hh"

WindowSettings::WindowSettings(const PG_Widget *parent,
                               RecognizerProcess *recognizer,
                               WidgetSpectrogram *spectrogram)
  : WindowChild(parent, "Settings", 350, 300, true, true, "OK", "Cancel")
{
  this->m_beam_edit = NULL;
  this->m_lmscale_edit = NULL;
  this->m_recognizer = recognizer;
  this->m_spectrogram = spectrogram;
}

void
WindowSettings::initialize()
{
  std::string text;
  WindowChild::initialize();
  
  // Text labels.
  new PG_Label(this->m_window, PG_Rect(10, 50, 200, 20), "Enter parameters.");
  text = str::fmt(40, "Beam (%u-%u):", RecognizerProcess::MIN_BEAM,
                                       RecognizerProcess::MAX_BEAM);
  new PG_Label(this->m_window, PG_Rect(10, 100, 150, 20), text.data());
  text = str::fmt(40, "LM-scale (%u-%u):", RecognizerProcess::MIN_LMSCALE,
                                           RecognizerProcess::MAX_LMSCALE);
  new PG_Label(this->m_window, PG_Rect(10, 140, 150, 20), text.data());
  new PG_Label(this->m_window, PG_Rect(10, 180, 200, 20), "Spectrum contrast (0-1):");
  new PG_Label(this->m_window, PG_Rect(10, 220, 200, 20), "Spectrum darkness (0-1):");
  
  // Text edit boxes.
  this->m_beam_edit = new PG_LineEdit(this->m_window,
                                      PG_Rect(150, 100, 50, 20),
                                      "LineEdit",
                                      3);
  this->m_lmscale_edit = new PG_LineEdit(this->m_window,
                                         PG_Rect(150, 140, 50, 20),
                                         "LineEdit",
                                         3);
  this->m_exponent_edit = new PG_LineEdit(this->m_window,
                                          PG_Rect(220, 180, 100, 20),
                                          "LineEdit",
                                          8);
  this->m_suppressor_edit = new PG_LineEdit(this->m_window,
                                            PG_Rect(220, 220, 100, 20),
                                            "LineEdit",
                                            8);
  
  if (this->m_recognizer) {
    // Write current settings into line edits.  
    text = str::fmt(10, "%d", this->m_recognizer->get_beam());
    this->m_beam_edit->SetText(text.data());
    text = str::fmt(10, "%d", this->m_recognizer->get_lmscale());
    this->m_lmscale_edit->SetText(text.data());
  }
  text = str::fmt(10, "%f", this->m_spectrogram->get_magnitude_exponent());
  this->m_exponent_edit->SetText(text.data());
  text = str::fmt(10, "%f", this->m_spectrogram->get_magnitude_suppressor());
  this->m_suppressor_edit->SetText(text.data());
}

bool
WindowSettings::do_ok()
{
  bool ok = true;
  int beam = 0, lmscale = 0;
  double suppressor, exponent;
  
  // Read values from text edit boxes. Show error message and return false if
  // parameter is invalid.
  
  exponent = this->read_float_value(this->m_exponent_edit, 0, 1, &ok);
  if (!ok) {
    this->error("Spectrogram contrast must be a float in range (0,1).",
                ERROR_NORMAL);
    return false;
  }  
  suppressor = this->read_float_value(this->m_suppressor_edit, 0, 1, &ok);
  if (!ok) {
    this->error("Spectrogram darkness must be a float in range (0,1).",
                ERROR_NORMAL);
    return false;
  }  

  if (this->m_recognizer) {
    beam = this->read_long_value(this->m_beam_edit, 1, 300, &ok);
    if (!ok) {
      this->error(str::fmt(50,
                           "Beam must be an integer between %u-%u.",
                           RecognizerProcess::MIN_BEAM,
                           RecognizerProcess::MAX_BEAM),
                  ERROR_NORMAL);
      return false;
    }
  
    lmscale = this->read_long_value(this->m_lmscale_edit, 1, 100, &ok);
    if (!ok) {
      this->error(str::fmt(50,
                           "LM-scale must be an integer between %u-%u.",
                           RecognizerProcess::MIN_LMSCALE,
                           RecognizerProcess::MAX_LMSCALE),
                  ERROR_NORMAL);
      return false;
    }
  }

  // All values were ok, set the values.
  
  this->m_spectrogram->set_magnitude_exponent(exponent);
  this->m_spectrogram->set_magnitude_suppressor(suppressor);

  if (this->m_recognizer) {
    // Send parameter change messages to recognizer.
    this->m_recognizer->set_beam(beam);
    this->m_recognizer->set_lmscale(lmscale);
    try {
      this->m_recognizer->get_out_queue()->flush();
    }
    catch(msg::ExceptionBrokenPipe) {
      // -1 is a signal of a broken pipe!
      this->end_running(-1);
      return false;
    }
  }

  return true;
}

float
WindowSettings::read_float_value(PG_LineEdit *line_edit,
                                 float min,
                                 float max,
                                 bool *ok)
{
  std::string text = line_edit->GetText();
  float value = str::str2float(text);

  if (!ok)
    return 0;
    
  if (value <= min || value >= max) {
    *ok = false;
    return 0;
  }
  
  return value;
}

long
WindowSettings::read_long_value(PG_LineEdit *line_edit,
                                long min,
                                long max,
                                bool *ok)
{
  std::string text = line_edit->GetText();
  long value = str::str2long(text);

  if (!ok)
    return 0;
    
  if (value < min || value > max) {
    *ok = false;
    return 0;
  }
  
  return value;
}
