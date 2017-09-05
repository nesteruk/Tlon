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

      identifier_rule %=
        +(alnum
          | char_(L'_')
          | char_(L'₀')
          | char_(L'₁')
          | char_(L'₂')
          | char_(L'₃')
          | char_(L'₄')
          | char_(L'₅')
          | char_(L'₆')
          | char_(L'₇')
          | char_(L'₈')
          | char_(L'₉')
          | char_(L'₊')
          | char_(L'₋')
          | char_(L'₌')
          | char_(L'₍')
          | char_(L'₎')
          | char_(L'⨧')
          | char_(L'ᵢ')
          | char_(L'ⱼ')
          | char_(L'ₖ')
          | char_(L'⁰'));

      basic_type_rule %=
        -qi::hold[identifier_rule >> lit(L".")]
        // it was a nice idea, but having a c++ specific table is stupid
        >> identifier_rule;

      enum_declaration_rule %=
        -qi::hold[identifier_rule >> lit(L".")]
        >> identifier_rule;

      type_specification_rule %=
        basic_type_rule | tuple_signature_rule;

      tuple_signature_element_rule %=
        -qi::hold[identifier_rule >> lit(":")]
        >> basic_type_rule;

      property_rule %=
        identifier_rule % ','
        >> lit(L":")
        >> type_specification_rule
        >> -(
        (L":=" >> spirit::attr(false)
          |
          L"≡" >> spirit::attr(true)
          ) >> +(alnum - ';')) // value on the right
        >> lit(L";");

      tuple_signature_rule %=
        spirit::eps >>
        lit(L"(")
        >> tuple_signature_element_rule % ','
        >> lit(L")");

      parameter_declaration_rule %=
        spirit::eps
        >> identifier_rule % ','
        >> lit(L":")
        >> type_specification_rule
        >> -(L":=" >> +(alnum));

      block_rule %=
        statement_rule % (qi::eol | L';')
        >> qi::attr(42);

      function_body_rule %=
        identifier_rule
        >> lit(L":=")
        >> lit(L"(")
        >> -parameter_declaration_rule % ','
        >> lit(L")")
        >> lit(L"=>")
        >> type_specification_rule
        >> lit(L"{")
        //>> block_rule
        >> lit(L"}");

      anonymous_function_signature_rule %=
        L"("
        >> -parameter_declaration_rule % ','
        >> lit(L")")
        >> lit(L"=>") // note the minus here
        >> type_specification_rule
        >> char_(L';');

      function_signature_rule %=
        identifier_rule
        >> lit(L":=")
        >> anonymous_function_signature_rule;

      interface_declaration_rule %=
        lit(L"interface ") >> +(alnum) % '.'
        >> -(':' >> basic_type_rule % ',')
        >> "{"
        >> *function_signature_rule
        >> "}";

      class_declaration_rule %=
        lit(L"class ") >> +(alnum) % '.'
        >> -(lit(L"(") >> -parameter_declaration_rule % ',' >> lit(")"))
        >> "{"
        >> *(function_body_rule | property_rule | function_signature_rule)
        >> "}";

      enum_declaration_rule %=
        lit(L"enum") >> +(alnum) % '.'
        >> "{"
        >> *(+(alnum) % ',')
        >> "}";

      assignment_statement_rule %=
        identifier_rule % ","
        >> lit(L"<-")
        >> identifier_rule;

      statement_rule %=
        assignment_statement_rule;

      file_rule %= spirit::eps >>
        *(interface_declaration_rule | class_declaration_rule | enum_declaration_rule);

      statement_rule.name("statement");
      block_rule.name("block");
      function_signature_rule.name("function signature");
      function_body_rule.name("function body");
      property_rule.name("property");
      
      debug(class_declaration_rule);
      debug(function_signature_rule);
      debug(block_rule);
    }

    qi::rule<Iterator, block(), space_type> block_rule;
    qi::rule<Iterator, anonymous_function_signature(), space_type> anonymous_function_signature_rule;
    qi::rule<Iterator, statement(), space_type> statement_rule;
    qi::rule<Iterator, assignment_statement(), space_type> assignment_statement_rule;
    qi::rule<Iterator, function_body(), space_type> function_body_rule;
    qi::rule<Iterator, property(), space_type> property_rule;
    qi::rule<Iterator, type_specification(), space_type> type_specification_rule;
    qi::rule<Iterator, wstring(), space_type> identifier_rule;
    qi::rule<Iterator, basic_type(), space_type> basic_type_rule;
    qi::rule<Iterator, tuple_signature_element(), space_type> tuple_signature_element_rule;
    qi::rule<Iterator, tuple_signature(), space_type> tuple_signature_rule;
    qi::rule<Iterator, parameter_declaration(), space_type> parameter_declaration_rule;
    qi::rule<Iterator, function_signature(), space_type> function_signature_rule;
    qi::rule<Iterator, interface_declaration(), space_type> interface_declaration_rule;
    qi::rule<Iterator, class_declaration(), space_type> class_declaration_rule;
    qi::rule<Iterator, enum_declaration(), space_type> enum_declaration_rule;
    qi::rule<Iterator, file(), space_type> file_rule;

  };

  // relies on boost fusion also
  template<typename TLanguagePrinter, typename Iterator>
  wstring parse(Iterator first, Iterator last)
  {
    using spirit::qi::phrase_parse;

    file f;
    file_parser<wstring::const_iterator> fp{};
    auto b = phrase_parse(first, last, fp, space, f);
    if (b)
    {
      return TLanguagePrinter{}.pretty_print(f);
    }
    return wstring(L"FAIL");
  }
}
