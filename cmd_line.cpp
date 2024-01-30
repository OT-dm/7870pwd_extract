#include <stdexcept>
#include <string>
#include <iostream>

#include "cmd_line.h"

using namespace std;

namespace
{
   struct cmd_line_params run_params;

   string usage()
   {
      return "Usage:\n\ 
                     7870pwd_extract -z <path_to_AllAppUpdateA13.bin> -l <path_to_lsec6318update> [-v]\n\
              -v - enables verbose mode.";
   }

};//namespace

const cmd_line_params& get_cmd_line_params(void)
{
   return run_params;
}

void process_cmd_line(int argc, const char* argv[])
{
   bool state_zip=false, state_lsec=false;
   for (int i=1; i<argc; i++)
   {
      string curr_param=argv[i];
      if(curr_param.length()<1) throw std::runtime_error("Command line is not valid.\n" + usage());
      switch(curr_param[0])
      {
         case '-':  //param switch
                    if(state_zip) throw std::runtime_error("Command line arg " + curr_param + " is not valid. Expected path to AllAppUpdateA13.bin zip file");                 
                    if(state_lsec) throw std::runtime_error("Command line arg " + curr_param + " is not valid. Expected path to lsec6318lsec file");                 
                    if(curr_param.length()<2) throw std::runtime_error("Command line is not valid.\n" + usage());
                    switch(curr_param[1])
                    {
                        case 'v': run_params.verbose=true; break;
                        case 'z': state_zip=true; break;
                        case 'l': state_lsec=true; break;
                        default: throw std::runtime_error("Command line is not valid.\n" + usage()); break;
                    };
                    break;
         default:   //param value
                    if(state_zip) 
                    {
                       run_params.zip_path=curr_param;state_zip=false;
                    }
                    else 
                    {
                       if(state_lsec) 
                       {
                          run_params.lsec6318_path=curr_param;state_lsec=false;
                       }
                       else throw std::runtime_error("Command line is not valid.\n" + usage()); 
                    }
                    break;                   
      }
   }
   if(get_cmd_line_params().zip_path=="" || get_cmd_line_params().lsec6318_path=="") throw std::runtime_error("Command line is not valid.\n" + usage()); 
   if(get_cmd_line_params().verbose)
   {
      cout <<  "-v verbose:" << get_cmd_line_params().verbose << endl;
      cout <<  "-z zip_path:" << get_cmd_line_params().zip_path << endl;
      cout <<  "-l lsec6318_path:" << get_cmd_line_params().lsec6318_path << endl;
   }
}