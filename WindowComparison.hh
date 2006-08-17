
#ifndef WINDOWTEXTCOMPARISON_HH_
#define WINDOWTEXTCOMPARISON_HH_

#include <pglabel.h>
#include "WindowChild.hh"
#include "comparison.hh"

class WindowComparison  :  public WindowChild
{
  
public:

  WindowComparison(PG_Widget *parent,
                   const std::string &reference,
                   const std::string &hypothesis);
  virtual ~WindowComparison();
  
  virtual void initialize();
  
protected:

  virtual void do_opening();
  virtual void do_running();
  virtual void do_closing(int return_value);
  
  void construct_result_array(const TextComparisonResult &result);
  
  PG_Label* construct_numeric_label(unsigned int column,
                                    unsigned int row,
                                    unsigned int value);
  
  PG_Label* construct_percentage_label(unsigned int column,
                                       unsigned int row,
                                       long value,
                                       unsigned int total);

  PG_Label* construct_label(unsigned int column,
                            unsigned int row,
                            const std::string &text,
                            PG_Label::TextAlign align);

private:
  
  const std::string m_reference;
  const std::string m_hypothesis;

  PG_Label *m_waiting_label;
  TextComparer *m_comparer;
};

#endif /*WINDOWTEXTCOMPARISON_HH_*/
