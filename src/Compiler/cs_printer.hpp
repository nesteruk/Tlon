#pragma once
#include "printer.hpp"
#include "chars.h"

namespace tlön
{
  namespace printers
  {
    struct cs_printer : printer
    {
      // as always, a mapping of known tlon types to C#
      map<wstring,wstring> known_types
      {
        {L"i8", L"sbyte"},
        {L"u8", L"byte"},
        {L"i16", L"short"},
        {L"u16", L"ushort"},
        {L"i32", L"int"},
        {L"u32", L"uint"},
        {L"i64", L"long"},
        {L"u32", L"ulong"},
        {L"f32", L"float"},
        {L"f64", L"double"},
        {L"isize", L"long"}, // speculative since we don't really know
        {L"usize", L"ulong"},

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

      void visit(const assignment_statement& obj) override {
      }
      void visit(const parameter_declaration& obj) override{
      }
      void visit(const interface_function_signature& obj) override{
      }
      void visit(const interface_declaration& obj) override{
      }
      void visit(const class_declaration& obj) override{
      }
      void visit(const file& obj) override
      {
        vector<wstring> usings
        {
          L"System", 
          L"System.Collections.Generic",
          L"System.Linq"
        };
        auto r = renderer{ *this };

        // dump all usingsf
        // the more the merrier
        for (const auto& u : usings)
          buffer << indent << u << nl;

        for (auto& item : obj.declarations)
        {
          apply_visitor(r, item);

          buffer << nl;
        }
      }
      void visit(const tuple_signature_element& obj) override{
      }
      void visit(const tuple_signature& obj) override{
      }
      void visit(const property& obj) override{
      }
      void visit(const function_body& obj) override 
      {
        buffer << indent;

        apply_visitor(renderer{ *this }, obj.return_type);
        buffer << " " << identifier(obj.name) << "(";

        // parameters
        for (int pi = 0; pi < obj.parameters.size(); ++pi)
        {
          auto& p = obj.parameters[pi];
          visit(p);
          if (pi + 1 != obj.parameters.size())
            buffer << ", ";
        }
        buffer << ")"; 

        {
          const auto& _ = scope();

          for (auto& st : obj.statements)
          {
            apply_visitor(renderer{ *this }, st);
            buffer << nl;
          }
        }
      }
      void visit(const basic_type& obj) override{
      }
      void visit(const anonymous_function_signature& obj) override
      {
        visit(obj, {});
      }
      
      void visit(const anonymous_function_signature& obj, optional<wstring> name)
      {
        
      }

      struct renderer : static_visitor<>
      {
        cs_printer& printer;

        explicit renderer(cs_printer& printer)
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

  }
}