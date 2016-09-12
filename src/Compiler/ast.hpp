#pragma once

namespace tlön
{
  struct class_declaration;
  struct file;
  struct interface_declaration;
  struct interface_function_signature;
  struct parameter_declaration;
  struct assignment_statement;

  struct ast_element_visitor
  {
    virtual ~ast_element_visitor() = default;

    virtual void visit(const assignment_statement& obj) = 0;
    virtual void visit(const parameter_declaration& obj) = 0;
    virtual void visit(const interface_function_signature& obj) = 0;
    virtual void visit(const interface_declaration& obj) = 0;
    virtual void visit(const class_declaration& obj) = 0;
    virtual void visit(const file& obj) = 0;
  };

  struct integral_types_ : spirit::qi::symbols<wchar_t, wstring>
  {
    integral_types_()
    {
      add(L"i8", L"int8_t");
      add(L"u8", L"uint8_t");
      add(L"i16", L"int16_t");
      add(L"u16", L"uint16_t");
      add(L"i32", L"int32_t");
      add(L"u32", L"uint32_t");
      add(L"i64", L"int64_t");
      add(L"u64", L"uint64_t");
      add(L"f32", L"float");
      add(L"f64", L"double");
    }
  } integral_types;

  struct ast_element
  {
    virtual ~ast_element() = default;
    virtual void accept(ast_element_visitor& visitor) = 0;
  };

  struct assignment_statement : ast_element
  {
    vector<wstring> names;
    wstring value;

    void accept(ast_element_visitor& visitor) override {
      visitor.visit(*this);
    }
  };

  typedef variant<assignment_statement> statement;

  struct parameter_declaration : ast_element
  {
    vector<wstring> names;
    wstring type, default_value;

    void accept(ast_element_visitor& visitor) override {
      visitor.visit(*this);
    }
  };

  struct interface_function_signature : ast_element
  {
    wstring name;
    vector<parameter_declaration> parameters;
    wstring return_type;

    void accept(ast_element_visitor& visitor) override {
      visitor.visit(*this);
    }
  };

  typedef variant<interface_function_signature> interface_member;

  struct interface_declaration : ast_element
  {
    vector<wstring> name;
    vector<interface_member> members;


    void accept(ast_element_visitor& visitor) override
    {
      visitor.visit(*this);
    }
  };

  struct class_declaration : ast_element
  {
    vector<wstring> name;
    vector<parameter_declaration> primary_constructor_parameters;

    void accept(ast_element_visitor& visitor) override{
      visitor.visit(*this);
    }
  };

  typedef variant<interface_declaration,class_declaration> top_level_declaration;

  struct file : ast_element
  {
    vector<top_level_declaration> declarations;

    void accept(ast_element_visitor& visitor) override
    {
      visitor.visit(*this);
    }
  };

}

BOOST_FUSION_ADAPT_STRUCT(
  tlön::parameter_declaration,
  (std::vector<std::wstring>, names),
  (std::wstring, type),
  (std::wstring, default_value)
)

BOOST_FUSION_ADAPT_STRUCT(
  tlön::interface_function_signature,
  (std::wstring, name),
  (std::vector<tlön::parameter_declaration>, parameters)
  (std::wstring, return_type)
)

BOOST_FUSION_ADAPT_STRUCT(
  tlön::class_declaration,
  (std::vector<wstring>, name),
  (std::vector<tlön::parameter_declaration>, primary_constructor_parameters)
)

BOOST_FUSION_ADAPT_STRUCT(
  tlön::interface_declaration,
  (std::vector<wstring>, name),
  (std::vector<tlön::interface_member>, members)
)

BOOST_FUSION_ADAPT_STRUCT(
  tlön::file,
  (std::vector<tlön::top_level_declaration>, declarations)
)