#pragma once
#include <map>
#include <sstream>
#include <string>

struct identifier_helper
{
  static std::map<wchar_t, std::wstring> subscript;
  static std::map<wchar_t, std::wstring> superscript;

  static std::wstring get(const std::wstring& identifier)
  {
    std::wostringstream s;
    for (const wchar_t c : identifier)
    {
      auto it = subscript.find(c);
      
      if (it != subscript.end())
        s << L'_' << it->second;
      else if ((it = superscript.find(c)) != superscript.end())
        s << L"__" << it->second;
      else
        s << c;
    }
    return s.str();
  }
};

std::map<wchar_t, std::wstring> identifier_helper::subscript = {
  { L'₀', L"0" }
};

std::map<wchar_t, std::wstring> identifier_helper::superscript = {
  { L'⁰', L"0" }
};
