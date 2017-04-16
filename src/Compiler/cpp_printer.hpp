#pragma once
#include "printer.hpp"
#include "chars.h"

namespace tlön
{
  namespace printers
  {
    struct cpp_printer : printer
    {
      map<wstring, wstring> known_types{
        {L"i8", L"int8_t"},
        {L"u8", L"uint8_t"},
        {L"i16", L"int16_t"},
        {L"u16", L"uint16_t"},
        {L"i32", L"int32_t"},
        {L"u32", L"uint32_t"},
        {L"i64", L"int64_t"},
        {L"u64", L"uint64_t"},
        {L"f32", L"float"},
        {L"f64", L"double"},
        {L"isize", L"int"},
        {L"usize", L"unsigned int"},
        {L"string", L"std::wstring"},

        // individual bit represented as a boolean
        {L"bit", L"bool"}
      };


    protected:
      wstring type_name(const wstring& tlon_type_name) override
      {
        auto it = known_types.find(tlon_type_name);
        return it == known_types.end() ? tlon_type_name : known_types[tlon_type_name];
      }

    public:
      static wstring identifier(const wstring& name)
      {
        return identifier_helper::get(name);
      }

      void visit(const function_body& obj) override
      {
        buffer << indent;

        apply_visitor(renderer{ *this }, obj.return_type);
        buffer << " " << identifier(obj.name) << "(";

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

        buffer << ")" << nl;
        {
          const auto& _ = scope();

          for (auto& st : obj.statements)
          {
            apply_visitor(renderer{ *this }, st);
            buffer << nl;
          }
        }
      }
      
      void visit(const anonymous_function_signature& obj) override
      {
        visit(obj, {});
      }

      void visit(const anonymous_function_signature& obj, optional<wstring> name)
      {
        apply_visitor(renderer{ *this }, obj.return_type);

        if (name)
          buffer << " " << name.get();
        
        buffer << "(";

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

        buffer << ")";
      }

      void visit(const property& obj) override
      {
        for (const auto& raw_name : obj.names)
        {
          auto name = identifier(raw_name);
          // property backing field is protected
          // getters and setters are public
          buffer << reduced_indent() << "protected:" << nl;
          buffer << indent;
          apply_visitor(renderer{ *this }, obj.type);
          buffer << " " << identifier(name);

          // default value, if any
          if (obj.default_value.size() > 0)
            buffer << "{" << obj.default_value << "}";

          buffer << ";" << nl;
          buffer << reduced_indent() << "public:" << nl;

          buffer << indent << "void set_" << name << "(const ";
          apply_visitor(renderer{ *this }, obj.type);
          buffer << " " << name << ")" << nl;
          {
            const auto& _ = scope();
            buffer << indent << "this->" << name << " = " << name << ";" << nl;
          }
          buffer << nl;

          buffer << indent;
          apply_visitor(renderer{ *this }, obj.type);
          buffer << " get_" << name << "() const" << nl;
          {
            const auto& _ = scope();
            buffer << indent << "return " << name << ";" << nl;
          }
          buffer << nl;
        }
      }


      void visit(const basic_type& obj) override
      {
        buffer << type_name(obj.name);
      }

      void visit(const tuple_signature_element& obj) override
      {

        // if the tuple has a name, emit it in comments
        if (obj.name)
        {
          buffer << "/*" << obj.name.value() << "*/";
        }
        visit(obj.type);
      }

      void visit(const tuple_signature& obj) override
      {
        buffer << "std::tuple<";
        for (int i = 0; i < obj.elements.size(); ++i)
        {
          auto& e = obj.elements[i];
          visit(e);
          if (i + 1 != obj.elements.size())
            buffer << ",";
        }
        buffer << ">";
      }

      struct renderer : static_visitor<>
      {
        cpp_printer& printer;

        explicit renderer(cpp_printer& printer)
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
        for (const auto& name : obj.names)
        {
          buffer << indent << name << " = " << obj.value << ";";
        }
      }

      void visit(const parameter_declaration& obj) override
      {
        bool use_const_ref = false; // disable for now
        /*wstring result;
        SymbolSearcher<wstring> ss(obj.type, result);
        if (ss.found().size() > 0) use_const_ref = false;*/
        for (int i = 0; i < obj.names.size(); ++i)
        {
          auto name = identifier(obj.names[i]);

          if (use_const_ref) buffer << "const ";

          apply_visitor(renderer{ *this }, obj.type);

          if (use_const_ref) buffer << "&";
          buffer << " " << name;
          if (i + 1 != obj.names.size())
            buffer << ", ";
        }
      }

      void visit(const interface_function_signature& obj) override
      {
        buffer << indent << "virtual ";
        visit(obj.signature, obj.name);
        buffer << " = 0;" << nl;
      }

      void visit(const class_declaration& obj) override
      {
        auto ns = name_space(obj.name);
        auto name = identifier(*obj.name.rbegin());
        buffer << indent << "class " << name << nl;
        {
          const auto& s = scope(true);
          buffer << reduced_indent() << "public:" << nl;

          auto param_count = obj.primary_constructor_parameters.size();
          // any ctor declarations?
          if (param_count > 0)
          {
            buffer << indent << name << "(";
            for (int i = 0; i < param_count; ++i)
            {
              auto& p = obj.primary_constructor_parameters[i];
              visit(p);
              if (i + 1 != param_count)
                buffer << ", ";
            }
            buffer << ") : " << nl;
            for (int i = 0; i < param_count; ++i)
            {
              auto& p = obj.primary_constructor_parameters[i];
              auto& names = p.names;

              for (int j = 0; j < names.size(); ++j)
              {
                buffer << indent << indent_char << wformat(L"%1%{%1%}") % identifier_helper::get(names[j]);
                if (!(i + 1 == param_count && j + 1 == names.size()))
                  buffer << ", " << nl;
              }
            }
            buffer << " {}" << nl;

            buffer << nl;
          }

          // members
          for (auto& m : obj.members)
            apply_visitor(renderer{ *this }, m);
        }
        buffer << "/* " << name << " */" << nl;
      }

      void visit(const interface_declaration& obj) override
      {
        auto ns = name_space(obj.name);
        auto name = identifier(*obj.name.rbegin());
        buffer << indent << "class " << name << nl;
        {
          auto s = scope(true);

          buffer << indent << "virtual ~" << name
            << "() = default;" << nl;

          for (auto& item : obj.members)
            apply_visitor(renderer{ *this }, item);
        }
        buffer << " /* " << name << " */" << nl;
      }

      void visit(const file& obj) override
      {
        buffer << indent << "#include \"tlön.h\"" << nl;
        auto r = renderer{ *this };
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
