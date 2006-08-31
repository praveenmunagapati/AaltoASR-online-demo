
#ifndef WINDOWCHILD_HH_
#define WINDOWCHILD_HH_

#include <pgbutton.h>
#include "Window.hh"

/** A class for pop-up child windows. The size, title and buttons may be
 * adjusted. Also whether to show close button in top-right corner can be
 * selected. */
class WindowChild  :  public Window
{
  
public:

  WindowChild(const PG_Widget *parent,
              const std::string &title,
              Uint16 width,
              Uint16 height,
              bool close);
  WindowChild(const PG_Widget *parent,
              const std::string &title,
              Uint16 width,
              Uint16 height,
              bool close,
              bool enter_as_ok,
              const std::string &ok_text);
  /** Creates a pop up child window.
   * \param parent Parent widget.
   * \param title Title of the window.
   * \param width Width of the window.
   * \param height Height of the window.
   * \param close true if close box and close on ESC should be enabled.
   * \param enter_as_ok true if Pressing enter means the same as pressing the
   *                    ok button.
   * \param ok_text Text label for OK button.
   * \param cancel_text Text label for Cancel button. */ 
  WindowChild(const PG_Widget *parent,
              const std::string &title,
              Uint16 width,
              Uint16 height,
              bool close,
              bool enter_as_ok,
              const std::string &ok_text,
              const std::string &cancel_text);
              
  virtual ~WindowChild() {}
  
  virtual void initialize();
  
protected:

  // Callback functions for ParaGUI.
  bool handle_ok_button();
  bool handle_cancel_button();
  bool handle_close();

  virtual bool handle_key_pressed(const SDL_KeyboardEvent *key);

  /** Called when ok button is pressed. If returns true, the window is closed,
   * otherwise it won't close. This makes it possible for windows to e.g.
   * do the settings when ok is pressed.
   * \return true for success and to close the window. */
  virtual bool do_ok() { return true; }
  
  virtual PG_Widget* create_window();
    
private:

  const PG_Widget *m_parent;
  std::string m_title;
  const Uint16 m_width;
  const Uint16 m_height;
  bool m_close;
  bool m_enter_as_ok;
    
  int m_button_count;
  std::string m_ok_text;
  std::string m_cancel_text;
};

#endif /*WINDOWCHILD_HH_*/
