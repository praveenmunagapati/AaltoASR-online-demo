
#ifndef WINDOWCHILD_HH_
#define WINDOWCHILD_HH_

#include <pgbutton.h>
#include "Window.hh"

class WindowChild  :  public Window
{
  
public:

  WindowChild(const PG_Widget *parent, const std::string &title, Uint16 width, Uint16 height, bool close);
  WindowChild(const PG_Widget *parent, const std::string &title, Uint16 width, Uint16 height, bool close, const std::string &ok_text);
  WindowChild(const PG_Widget *parent, const std::string &title, Uint16 width, Uint16 height, bool close, const std::string &ok_text, const std::string &cancel_text);
  virtual ~WindowChild();

  virtual void initialize();
  
protected:

  virtual void do_running();

  bool handle_button(PG_Button *button);
  bool handle_close();

  virtual bool do_ok() { return true; }
  
  virtual PG_Widget* create_window();
    
private:

  const PG_Widget *m_parent;
  std::string m_title;
  const Uint16 m_width;
  const Uint16 m_height;
  bool m_close;
  
  int m_button_count;
  std::string m_ok_text;
  std::string m_cancel_text;
  PG_Button *m_ok_button;
  PG_Button *m_cancel_button;
  bool m_ok_pressed;
};

#endif /*WINDOWCHILD_HH_*/
