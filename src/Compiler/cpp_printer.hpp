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

      void visit(const interface_function_signature& obj) override
      {
        buffer << indent << "void " << obj.name << "(";

        // process arguments
        for (int pi = 0; pi < obj.parameters.size(); ++pi)
        {
          auto& p = obj.parameters[pi];
          for (int ni = 0; ni < p.names.size(); ++ni)
          {
            auto& n = p.names[ni];
            buffer << p.type << " " << n;
            if (!(pi + 1 == obj.parameters.size() && ni + 1 == p.names.size()))
              buffer << ", ";
          }
        }

        buffer << ")" << nl;
        const auto& body_scope = scope();

      }

      void visit(const class_declaration& obj) override 
      {
        auto ns = name_space(obj.name);
        buffer << indent << "class " << *obj.name.rbegin() << nl;
        {
          auto s = scope(true);
        }
      }

      void visit(const interface_declaration& obj) override
      {
        auto ns = name_space(obj.name);
        buffer << indent << "class " << *obj.name.rbegin() << nl;
        {
          auto s = scope(true);

          buffer << indent << "virtual ~" << *obj.name.rbegin() << "() = default;" << nl;

          for (auto& item : obj.members)
            apply_visitor(renderer{ *this }, item);
        }
      }

      void visit(const file& obj) override 
      {
        auto r = renderer{*this};
        for (auto& item : obj.declarations)
        {
          apply_visitor(r, item);

          // put a blank line between declarations
          buffer << nl;
        }
      }
    };
  }
}
