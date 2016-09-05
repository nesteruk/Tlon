#include "headers.hpp"
#include "parser.hpp"
using namespace tlön;

wstring read_file(const wstring filename)
{
  wifstream fs(filename);
  fs.imbue(locale{ locale::empty(), new codecvt_utf8<wchar_t>{} });
  wstringstream ss;
  ss << fs.rdbuf();
  return ss.str();
}

int main(int argc, char* argv[])
{
  
}
