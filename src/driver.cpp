#include "driver.h"
#include "parser.h"

int Driver::parse (const std::string &f)
{
  file = f;
  location.initialize (&file);
  scan_begin ();
  yy::parser parse(*this);
  parse.set_debug_level(0);
  int res = parse();
  scan_end ();
  return res;
}