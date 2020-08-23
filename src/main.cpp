#include "includes.h"
#include "driver.h"
#include "assembler.h"
#include "asm_exception.h"

using namespace std;

int main(int argc, char *argv[]) {
  try {
    Driver drv;
    bool next_out = false;
    string in_file_name;
    string out_file_name;

    for (int i = 1; i < argc; ++i) {
      if (string(argv[i]) == "-o") {
        next_out = true;
        continue;
      }

      if (next_out) {
        if (out_file_name.length() != 0) {
          throw AssemblerException(ERR_OUT_FILE_NAME_ALREADY_SET);
        }
        out_file_name = argv[i];
      } else {
        if (in_file_name.length() != 0) {
          throw AssemblerException(ERR_IN_FILE_NAME_ALREADY_SET);
        }
        in_file_name = argv[i];
      }
    }

    if (out_file_name.length() == 0) {
      auto d0 = in_file_name.find_last_of('/');
      string search_string;

      if (d0 != string::npos) {
        search_string = in_file_name.substr(d0 + 1, in_file_name.size() - d0 - 1);
      } else {
        d0 = 0;
        search_string = in_file_name;
      }

      auto d1 = search_string.find('.');
      if (d1 != string::npos) {
        out_file_name = in_file_name.substr(0, d0 + d1 + 1) + ".o";
      } else {
        out_file_name = in_file_name + ".o";
      }
    }

    if (drv.parse(in_file_name) == 0) {
      Assembler::get_instance().finalize_assembling(out_file_name);
    } else {
      
    }

  } catch (AssemblerException& e) {
    cout << "ERROR: " << e << endl;
  }
}