#pragma once
#include <sstream>
#include "ast.hpp"
#include <boost/format.hpp>

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
      void backtrack(size_t char_count, bool remove_newline = false)
      {
        int total_count = char_count + (remove_newline ? wcslen(nl) : 0);
        // hack: rather crude way of doing things
        buffer.seekp(-total_count, ios_base::end);
      }
    protected:
      wostringstream buffer;
      wstring indent;
      const wchar_t* nl{ L"\r\n" };

      virtual wstring type_name(const wstring& tlon_type_name) = 0;
      virtual wstring default_value_for(const wstring& tlon_type_name) = 0;

      /// Reduced indent, usable for public: and similar.
      wstring reduced_indent() const
      {
        if (indent.size() < indent_char.size())
          return indent;
        return indent.substr(0, indent.size() - indent_char.size());
      }

      struct scope_token
      {
        printer& p;
        bool semicolon;

        explicit scope_token(printer& p, bool semicolon = false)
          : p{ p }, semicolon{ semicolon }
        {
          p.buffer << p.indent << L"{" << p.nl;
          p.indent += p.indent_char;
        }

        ~scope_token()
        {
          p.indent = p.indent.substr(0, p.indent.length() - p.indent_char.length());
          p.buffer << p.indent << L"}" << (semicolon ? L";" : L"") << p.nl;
        }
      };

      struct namespace_token
      {
        printer& p;
        const vector<wstring>& name;

        namespace_token(printer& p, const vector<wstring>& name)
          : p(p),
            name(name)
        {
          if (name.size() < 2) return;

          for (int i = 0; i < (name.size()-1); ++i)
          {
            auto& n = name[i];
            p.buffer << p.indent << L"namespace " << n << p.nl << p.indent << L"{" << p.nl;
            p.indent += p.indent_char;
          }
        }

        ~namespace_token()
        {
          if (name.size() < 2) return;
          for (int i = 0; i < (name.size() - 1); ++i)
          {
            p.indent = p.indent.substr(0, p.indent.length() - p.indent_char.length());
            p.buffer << p.indent << L"} /* " << name[i] << L" */" << p.nl;
          }
        }

        wstring str() const
        {
          wostringstream oss;
          for (int i = 0; i < name.size()-1; ++i)
          {
            auto& n = name[i];
            oss << n;
            if (i + 1 != name.size() - 1)
              oss << L"."; // confusing what to put here
          }
          return oss.str();
        }
      };

      namespace_token name_space(const vector<wstring>& name)
      {
        return namespace_token{ *this, name };
      }

      scope_token scope(bool semicolon = false)
      {
        return scope_token{ *this, semicolon };
      }
    protected:
      const wstring indent_char{ L"  " };
    };
  }
}
