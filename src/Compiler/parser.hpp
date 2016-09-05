#pragma once
#include "ast.hpp"
#include "cpp_printer.hpp"

namespace tlön
{
  namespace qi = boost::spirit::qi;
  using namespace boost::spirit::unicode;

  template<typename Iterator>
  struct file_parser : qi::grammar<Iterator, file(), space_type>
  {
    file_parser() : file_parser::base_type(file_rule)
    {
      using qi::lit;

      function_signature_rule %= boost::spirit::eps >> lit(L"fn");

      interface_declaration_rule %=
        lit(L"interface ") >> +(alnum) % '.'
        >> "{"
        >> *function_signature_rule
        >> "}";

      file_rule %= boost::spirit::eps >> 
        *(interface_declaration_rule);
    }

    qi::rule<Iterator, function_signature(), space_type> function_signature_rule;
    qi::rule<Iterator, interface_declaration(), space_type> interface_declaration_rule;
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
