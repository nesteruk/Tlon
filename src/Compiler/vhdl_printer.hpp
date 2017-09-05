#pragma once
#include "printer.hpp"
#include "chars.h"

namespace tlön
{
  namespace printers
  {
    struct vhdl_printer : printer
    {
      void visit(const assignment_statement& obj) override;
      void visit(const parameter_declaration& obj) override;
      void visit(const function_signature& obj) override;
      void visit(const interface_declaration& obj) override;
      void visit(const class_declaration& obj) override;
      void visit(const file& obj) override;
      void visit(const tuple_signature_element& obj) override;
      void visit(const tuple_signature& obj) override;
      void visit(const property& obj) override;
      void visit(const function_body& obj) override;
      void visit(const enum_declaration& obj) override;
      void visit(const anonymous_function_signature& obj) override;
      void visit(const basic_type& obj) override;
      map<wstring, wstring> known_types{
        {L"i32", L"integer"}
      };

      void visit(const block& obj) override
      {
        renderer r{ *this };
        for (auto& s : obj.statements)
          apply_visitor(r, s);
      }

    protected:
      wstring default_value_for(const wstring& tlon_type_name) override 
      {
        return L""s;
      }

      /**
       * \brief Gets you the VHDL type, supporting arbitrary bitness.
       * \param tlon_type_name 
       * \return 
       */
      wstring type_name(const wstring& tlon_type_name) override
      {
        auto it = known_types.find(tlon_type_name);
        if (it != known_types.end())
          return known_types[tlon_type_name];
        
        if (tlon_type_name.length() > 1 &&
          (tlon_type_name[0] == L'u' || tlon_type_name[0] == L'i'))
        {
          try
          {
            auto count = lexical_cast<int>(tlon_type_name.substr(1));
            wostringstream os;
            os << "integer range ";
            if (tlon_type_name[0] == L'u')
            {
              // 0 to 2^n
              os << "0 to " << (1 << count);
            } 
            else
            {
              // -2^(n-1) to 2^(n-1) - 1
              os << "-" << (1 << (count - 1)) << " to "
                << (1 << (count - 1)) - 1;
            }
            return os.str();
          } 
          catch (bad_cast)
          {
          }
        }
        return tlon_type_name;
      }

    public:
      struct renderer : static_visitor<>
      {
        vhdl_printer& printer;

        explicit renderer(vhdl_printer& printer)
          : printer(printer)
        {
        }

        void operator()(const wstring& s) const
        {
          printer.buffer << s;
        }

        template <typename T> void operator()(T& t) const {
          printer.visit(t);
        }
      };
    };

    inline void vhdl_printer::visit(const assignment_statement& obj)
    {
    }

    inline void vhdl_printer::visit(const parameter_declaration& obj)
    {
    }

    inline void vhdl_printer::visit(const function_signature& obj)
    {
    }

    inline void vhdl_printer::visit(const interface_declaration& obj)
    {
    }

    inline void vhdl_printer::visit(const class_declaration& obj)
    {
      buffer << indent << L"architecture " << *obj.name.rbegin() << L" of ??? is" << nl;
      {
        for (auto& item : obj.members)
        {
          apply_visitor(renderer{ *this }, item);
          buffer << nl;
        }
      }
      buffer << indent << L"begin" << nl;
      buffer << indent << L"end " << *obj.name.rbegin() << ";" << nl;
    }

    inline void vhdl_printer::visit(const file& obj)
    {
      buffer << indent << L"library ieee;" << nl << L"use ieee.std_arith_1164.all;" << nl;
      auto r = renderer{ *this };
      for (auto& item : obj.declarations)
      {
        //
        apply_visitor(r, item);

        // put a blank line between declarations
        buffer << nl;
      }
    }

    inline void vhdl_printer::visit(const tuple_signature_element& obj)
    {
    }

    inline void vhdl_printer::visit(const tuple_signature& obj)
    {
    }

    

    inline void vhdl_printer::visit(const property& obj)
    {
      // he-hey, vhdl also supports collated names
      buffer << "signal ";
      for (auto i = 0; i < obj.names.size(); ++i)
      {
        auto name = obj.names[i];
        buffer << name;
        if (i + 1 != obj.names.size())
          buffer << ",";
      }
      buffer << " : ";
      apply_visitor(renderer{ *this }, obj.type);
      buffer << ";";
    }

    inline void vhdl_printer::visit(const function_body& obj)
    {
    }

    inline void vhdl_printer::visit(const enum_declaration& obj)
    {
      // does vhdl even have enums?
    }

    inline void vhdl_printer::visit(const anonymous_function_signature& obj)
    {
    }

    inline void vhdl_printer::visit(const basic_type& obj)
    {
      buffer << type_name(obj.name);
    }
  }
}
