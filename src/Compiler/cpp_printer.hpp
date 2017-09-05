#pragma once
#include "printer.hpp"
#include "chars.h"

namespace tlön
{
  namespace printers
  {
    struct cpp_printer : printer
    {
      struct known_type_info
      {
        wchar_t* language_specific_name;
        wchar_t* default_value;
      };

      map<wstring, known_type_info> known_types
      {
        { L"i8", {L"int8_t", L"0"} },
        { L"u8", {L"uint8_t", L"0"} },
        { L"i16", {L"int16_t", L"0"} },
        { L"u16", {L"uint16_t", L"0"} },
        { L"i32", {L"int32_t", L"0"} },
        { L"u32", {L"uint32_t", L"0"} },
        { L"i64", {L"int64_t", L"0" } },
        { L"u64", {L"uint64_t", L"0"} },
        { L"f32", {L"float", L"0.0f"} },
        { L"f64", {L"double", L"0.0"} },
        { L"isize", {L"int", L"0"} },
        { L"usize", {L"unsigned int", L"0"} },
        { L"string", {L"std::wstring", L""} },

        // individual bit represented as a boolean
        { L"bit", {L"bool", L"false"} },
        // logical type; probably needs tweaking for HDLs
        { L"bool", {L"bool", L"false"} }
      };


    protected:
      wstring type_name(const wstring& tlon_type_name) override
      {
        auto it = known_types.find(tlon_type_name);
        return it == known_types.end() ? tlon_type_name : known_types[tlon_type_name].language_specific_name;
      }

      wstring default_value_for(const wstring& tlon_type_name) override
      {
        auto it = known_types.find(tlon_type_name);
        return it == known_types.end() ? wstring{} : known_types[tlon_type_name].default_value;
      }

      void emit_reflection_info(const property& p) 
      {
        buffer << "property_info{}";
      }

      void emit_reflection_info(const function_body& fb)
      {
        buffer << "method_info{}";
      }

      void emit_reflection_info(const function_signature& fs)
      {
        // also emit method_info ...
      }

    public:
      static wstring identifier(const wstring& name)
      {
        return identifier_helper::get(name);
      }


      

      void visit(const block& obj) override
      {
        renderer r{ *this };
        for (auto& s : obj.statements)
          apply_visitor(r, s);
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

          for (auto& st : obj.block.statements)
          {
            apply_visitor(renderer{ *this }, st);
            buffer << "," << nl;
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
          if (obj.is_constant)
            buffer << "const static "; // 
          apply_visitor(renderer{ *this }, obj.type);
          buffer << " " << identifier(name);

          // default value, if any
          if (obj.default_value.size() > 0)
            buffer << "{" << obj.default_value << "}";
          else
          {
            // todo: but we insist on each variable having a default, so...
            buffer << "{";
            apply_visitor(default_value_visitor{*this}, obj.type);
            buffer << "}";
          }

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
        // if the type has a qualified path, there isn't much we can do about it
        if (obj.path_to_type.size() > 0)
        {
          for (auto& t : obj.path_to_type)
          {
            buffer << t << "::";
          }
          buffer << obj.name;
        }
        else {
          // this can be a singular type so we do lookup
          buffer << type_name(obj.name);
        }
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

      struct default_value_visitor : static_visitor<>
      {
        cpp_printer& printer;

        explicit default_value_visitor(cpp_printer& printer)
          : printer{printer}
        {
        }

        void operator()(const basic_type& bt) const
        {
          // for a scalar value, we just dump its default
          printer.buffer << printer.default_value_for(bt.name);
        }

        void operator()(const tuple_signature& ts) const
        {
          for (auto& e : ts.elements)
          {
            this->operator()(e.type);
            printer.buffer << ", ";
          }
          printer.backtrack(2);
        }
      };

      //! Emits reflection data about members
      struct reflection_renderer : static_visitor<>
      {
        cpp_printer& printer;

        explicit reflection_renderer(cpp_printer& printer) : printer(printer)
        {
        }

        void operator()(const function_body& fb) const
        {
          printer.buffer << printer.indent << "method_info{}";
        }

        void operator()(const property& p) const
        {
          printer.buffer << printer.indent << "property_info{}";
        }

        void operator()(const function_signature& s) const {}
      };

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

          // default value, if any
          if (obj.default_value.length() > 0)
            buffer << " = " << obj.default_value;

          if (i + 1 != obj.names.size())
            buffer << ", ";
        }
      }

      void visit(const function_signature& obj) override
      {
        buffer << indent << "virtual ";
        visit(obj.signature, obj.name);
        buffer << " = 0;" << nl;
      }

      void emit_automatic_class_members(const class_declaration& obj)
      {
        // emit reflection
        buffer << reduced_indent() << "public:" << nl;

        buffer << indent << 
          "const tlön::reflection::type_info& get_type_info() const override"
          << nl << "{ return this->type_info; }" << nl;

        buffer << indent <<
          "static tlön::reflection::type_info type_info;" << nl;
      }

      void visit(const enum_declaration& obj) override
      {
        auto ns = name_space(obj.name);
        auto enum_name = identifier(*obj.name.rbegin());

        buffer << indent << "enum class " << enum_name << nl;
        {
          const auto& s = scope(true);
          for (auto i = 0u; i < obj.members.size(); ++i)
          {
            const auto& member = obj.members[i];
            buffer << indent << member; 
            if (i+1 != obj.members.size()) buffer << ",";
            buffer << nl;
          }
        }
        buffer << nl;

        // dump the stream output operator
        buffer << indent << "std::ostream& operator<<(std::ostream& os, const "
          << enum_name << "& obj)" << nl;
        {
          const auto& s = scope(false);
          buffer << indent << "switch (obj)" << nl;
          buffer << indent << "{" << nl;
          for (const auto& member : obj.members)
          {
            buffer << indent << "case " << enum_name << "::" << member << ": "
              << "return os << \"" << member << "\";" << nl;
          }
          buffer << indent << "default: return os << \"Unknown case of "
            << enum_name << "\";" << nl;
          buffer << indent << "}" << nl;
        }
      }

      void visit(const class_declaration& obj) override
      {
        auto ns = name_space(obj.name);
        auto class_name = identifier(*obj.name.rbegin());

        // prepended by the abstract comment if necessary
        buffer << indent << (obj.is_abstract() ? "/* abstract */ " : "") << "class " << class_name;

        // process inheritors; at the very least, there's tlön::object
        buffer << " : public tlön::object<" << class_name << ">";

        buffer << nl;
        {
          const auto& s = scope(true);
          buffer << reduced_indent() << "public:" << nl;

          auto param_count = obj.primary_constructor_parameters.size();
          // any ctor declarations?
          if (param_count > 0)
          {
            buffer << indent << class_name << "(";
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

            // generate a property for each primary constructor parameter
            for (auto& p : obj.primary_constructor_parameters)
            {
              property prop;
              prop.names = p.names;
              prop.type = p.type;
              visit(prop);
            }

            buffer << nl;
          }

          // members
          for (auto& m : obj.members)
            apply_visitor(renderer{ *this }, m);

          // emit automatic class members
          emit_automatic_class_members(obj);
        } // end of class scope
        buffer << " /* " << class_name << " */" << nl << nl;

        // emit out-of-class reflection members
        buffer << indent << L"tlön::reflection::type_info ChangeMe::type_info ="
          << L" tlön::reflection::type_info" << nl;
        {
          const auto& _ = scope(true);

          // namespace name
          buffer << L"L\"" << ns.str() << L"\"," << nl;

          // name
          buffer << L"L\"" << class_name << L"\"," << nl;

          
          {
            // scope for methods
            const auto& ms = scope();
            for (auto t : obj.members)
              if (t.type() == typeid(function_body)) {
                apply_visitor(reflection_renderer{ *this }, t);
                buffer << "," << nl;
              }
            backtrack(1, true);
          }
          buffer << ",";
          {
            // scope for properties
            const auto& ps = scope();
          
            for (auto t : obj.members)
              if (t.type() == typeid(property)) {
                apply_visitor(reflection_renderer{ *this }, t);
                buffer << "," << nl;
              }
            backtrack(1, true);
          }
        }

        // emit out-of-class const/static initializers
        for (auto c : obj.members)
        {
          if (property* pp = get<property>(&c))
          {
            if (pp->is_constant)
            {
              for (auto& prop_name : pp->names)
              {
                buffer << indent << L"const ";
                apply_visitor(renderer{ *this }, pp->type);
                buffer << L" " << class_name << L"::" << prop_name << L" = " << pp->default_value << L";" << nl;
              }
            }
          }
        }
      }

      void visit(const interface_declaration& obj) override
      {
        auto ns = name_space(obj.name);
        auto name = identifier(*obj.name.rbegin());
        buffer << indent << "class " << name << " : public tlön::object<" << name << ">";

        for (auto& parent : obj.parents)
        {
          buffer << ", ";
          visit(parent);
        }

        buffer << nl;
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
