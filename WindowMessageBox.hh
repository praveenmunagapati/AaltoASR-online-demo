
#ifndef WINDOWMESSAGEBOX_HH_
#define WINDOWMESSAGEBOX_HH_

//#include <pglabel.h>
#include "undefine.hh"
#include <pgrichedit.h>
#include <pgbutton.h>
#include "WindowChild.hh"

class WindowMessageBox  :  public WindowChild
{
public:

  WindowMessageBox(const PG_Widget *parent, const std::string &title, const std::string &message, const std::string &button);
  WindowMessageBox(const PG_Widget *parent, const std::string &title, const std::string &message, const std::string &button1, const std::string &button2);
  virtual ~WindowMessageBox() { }

  virtual void initialize();

private:

  std::string m_message;

};

#endif /*WINDOWMESSAGEBOX_HH_*/