#include "logger.h"

using namespace std;

void Logger::write_log(string text) {
    cout << "[Log]\t" << text << endl;
}