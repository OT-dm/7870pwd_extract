#ifndef _cmd_line_h_
#define _cmd_line_h_

#include <string>

using namespace std;

struct cmd_line_params
{
  bool verbose;
  string zip_path,lsec6318_path;
  cmd_line_params():verbose(false){};
};

const cmd_line_params& get_cmd_line_params(void);
void process_cmd_line(int argc, const char* argv[]);

#endif //_cmd_line_h_
