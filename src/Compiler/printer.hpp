#pragma once
#include <sstream>
#include "ast.hpp"

namespace tlön
{
  namespace printers
  {
    struct printer : ast_element_visitor
    {
      virtual ~printer() = default;
      virtual wstring pretty_print(const file& f)
      {
        visit(f);
        return buffer.str();
      }
    protected:
      wostringstream buffer;
      const wchar_t* nl = L"\r\n";
    };
  }
}
