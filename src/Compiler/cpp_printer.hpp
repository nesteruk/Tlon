#pragma once
#include "printer.hpp"

namespace tlön
{
  namespace printers
  {
    struct cpp_printer : printer
    {
      struct renderer : static_visitor<>
      {
        cpp_printer& printer;

        explicit renderer(cpp_printer& printer)
          : printer(printer)
        {
        }

        template <typename T> void operator()(T& t) const {
          printer.visit(t);
        }
      };

      void visit(const assignment_statement& obj) override
      {
      }

      void visit(const parameter_declaration& obj) override
      {
      }

      void visit(const function_signature& obj) override
      {
      }

      void visit(const interface_declaration& obj) override
      {
        buffer << "class " << *obj.name.rbegin() << nl
          << "{" << nl
          << "}" << nl;
      }

      void visit(const file& obj) override 
      {
        auto r = renderer{*this};
        for (auto& item : obj.declarations)
          apply_visitor(r, item);
      }
    };
  }
}
