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

      template<typename T>
      struct SymbolSearcher
      {
        SymbolSearcher(T searchFor, wstring& result) 
          : _sought(searchFor), _found(result)
        {
        }

        void operator() (wstring s, T ct)
        {
          if (_sought == ct)
          {
            _found = s;
          }
        }

        wstring found() const { return _found; }

      private:
        T _sought;
        wstring& _found;
      };

      void visit(const assignment_statement& obj) override
      {
      }

      void visit(const parameter_declaration& obj) override
      {
        bool use_const_ref = true;
        wstring result;
        SymbolSearcher<wstring> ss{ obj.type, result };
        if (ss.found().size() > 0) use_const_ref = false;
        for (int i = 0; i < obj.names.size(); ++i)
        {
          auto& name = obj.names[i];

          if (use_const_ref) buffer << "const ";
          buffer << obj.type;
          if (use_const_ref) buffer << "&";
          buffer << " " << name;
          if (i + 1 != obj.names.size())
            buffer << ", ";
        }
      }

      void visit(const interface_function_signature& obj) override
      {
        buffer << indent << "virtual " << obj.return_type << " " << obj.name << "(";

        // process arguments
        for (int pi = 0; pi < obj.parameters.size(); ++pi)
        {
          auto& p = obj.parameters[pi];

          // process this parameter
          visit(p);

          // comma required unless this is last
          if (pi + 1 != obj.parameters.size())
            buffer << ", ";
        }

        buffer << ") = 0;" << nl;
      }

      void visit(const class_declaration& obj) override 
      {
        auto ns = name_space(obj.name);
        buffer << indent << "class " << *obj.name.rbegin() << nl;
        {
          const auto& s = scope(true);
          buffer << reduced_indent() << "public:" << nl;

          // any ctor declarations?
          if (obj.primary_constructor_parameters.size() > 0)
          {
            buffer << indent << *obj.name.rbegin() << "(";
            for (int i = 0; i < obj.primary_constructor_parameters.size(); ++i)
            {
              auto& p = obj.primary_constructor_parameters[i];
              visit(p);
              if (i + 1 != obj.primary_constructor_parameters.size())
                buffer << ", ";
            }
            buffer << ") /* todo initializers */ {}";

            buffer << nl;
          }
        }
        buffer << "/* " << *obj.name.rbegin() << "*/" << nl;
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
        buffer << " /* " << *obj.name.rbegin() << "*/" << nl;
      }

      void visit(const file& obj) override 
      {
        buffer << indent << "#include \"tlön.h\"" << nl;
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
