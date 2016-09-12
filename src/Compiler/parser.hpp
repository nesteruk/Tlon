#pragma once
#include "ast.hpp"
#include "cpp_printer.hpp"

namespace tlön
{
  namespace qi = spirit::qi;
  using namespace spirit::unicode;

  template<typename Iterator>
  struct file_parser : qi::grammar<Iterator, file(), space_type>
  {
    file_parser() : file_parser::base_type(file_rule)
    {
      using qi::lit;

      parameter_declaration_rule %=
        spirit::eps
        >> +(alnum) % ','
        >> lit(L":")
        >> +alnum;

      function_signature_rule %= spirit::eps
        >> +(alnum)
        >> lit(L":=")
        >> lit(L"(")
        >> parameter_declaration_rule % ','
        >> char_(L')')
        >> char_(L';')
        ;

      interface_declaration_rule %=
        lit(L"interface ") >> +(alnum) % '.'
        >> "{"
        >> *function_signature_rule
        >> "}";
      
      class_declaration_rule %=
        lit(L"class ") >> +(alnum) % '.'
        >> "{"

        >> "}";

      file_rule %= spirit::eps >> 
        *(interface_declaration_rule|class_declaration_rule);
    }

    qi::rule<Iterator, parameter_declaration(), space_type> parameter_declaration_rule;
    qi::rule<Iterator, interface_function_signature(), space_type> function_signature_rule;
    qi::rule<Iterator, interface_declaration(), space_type> interface_declaration_rule;
    qi::rule<Iterator, class_declaration(), space_type> class_declaration_rule;
    qi::rule<Iterator, file(), space_type> file_rule;
  };

  // relies on boost fusion also
  template<typename Iterator>
  wstring parse(Iterator first, Iterator last)
  {
    using spirit::qi::phrase_parse;

    file f;
    file_parser<wstring::const_iterator> fp{};
    auto b = phrase_parse(first, last, fp, space, f);
    if (b)
    {
      printers::cpp_printer p;
      return p.pretty_print(f);
    }
    return wstring(L"FAIL");
  }
}
