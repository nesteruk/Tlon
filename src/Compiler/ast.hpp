#pragma once

namespace tlön
{
  struct anonymous_function_signature;
  struct tuple_signature;
  struct tuple_signature_element;
  struct class_declaration;
  struct file;
  struct interface_declaration;
  struct function_signature;
  struct parameter_declaration;
  struct assignment_statement;
  struct property;
  struct function_body;
  struct basic_type;
  struct block;
  struct enum_declaration;

  struct ast_element_visitor
  {
    virtual ~ast_element_visitor() = default;

    virtual void visit(const assignment_statement& obj) = 0;
    virtual void visit(const parameter_declaration& obj) = 0;
    virtual void visit(const function_signature& obj) = 0;
    virtual void visit(const interface_declaration& obj) = 0;
    virtual void visit(const class_declaration& obj) = 0;
    virtual void visit(const file& obj) = 0;
    virtual void visit(const tuple_signature_element& obj) = 0;
    virtual void visit(const tuple_signature& obj) = 0;
    virtual void visit(const property& obj) = 0;
    virtual void visit(const function_body& obj) = 0;
    virtual void visit(const basic_type& obj) = 0;
    virtual void visit(const anonymous_function_signature& obj) = 0;
    virtual void visit(const block& obj) = 0;
    virtual void visit(const enum_declaration& obj) = 0;
  };

  struct ast_element
  {
    virtual ~ast_element() = default;
    virtual void accept(ast_element_visitor& visitor) = 0;
  };

  struct basic_type : ast_element
  {
    vector<wstring> path_to_type;
    wstring name;

    void accept(ast_element_visitor& visitor) override
    {
      visitor.visit(*this);
    }
  };

  struct tuple_signature_element : ast_element
  {
    optional<wstring> name;
    basic_type type;

    void accept(ast_element_visitor& visitor) override
    {
      visitor.visit(*this);
    }
  };

  struct tuple_signature : ast_element
  {
    vector<tuple_signature_element> elements;

    void accept(ast_element_visitor& visitor) override
    {
      visitor.visit(*this);
    }
  };

  typedef variant<basic_type, tuple_signature> type_specification;

  struct assignment_statement : ast_element
  {
    vector<wstring> names;
    wstring value;

    void accept(ast_element_visitor& visitor) override {
      visitor.visit(*this);
    }
  };

  typedef variant<assignment_statement> statement;

  struct block : ast_element
  {
    vector<statement> statements;
    spirit::qi::unused_type dummy;

    void accept(ast_element_visitor& visitor) override
    {
      visitor.visit(*this);
    }
  };

  struct parameter_declaration : ast_element
  {
    vector<wstring> names;
    type_specification type;
    wstring default_value;

    void accept(ast_element_visitor& visitor) override {
      visitor.visit(*this);
    }
  };

  struct anonymous_function_signature : ast_element
  {
    vector<parameter_declaration> parameters;
    type_specification return_type;

    void accept(ast_element_visitor& visitor) override {
      visitor.visit(*this);
    }
  };

  struct function_signature : ast_element
  {
    wstring name;
    anonymous_function_signature signature;

    void accept(ast_element_visitor& visitor) override {
      visitor.visit(*this);
    }
  };

  typedef variant<function_signature> interface_member;

  struct function_body : ast_element
  {
    wstring name;
    vector<parameter_declaration> parameters;
    type_specification return_type;
    block block;

    void accept(ast_element_visitor& visitor) override
    {
      visitor.visit(*this);
    }
  };

  struct interface_declaration : ast_element
  {
    vector<wstring> name;
    vector<basic_type> parents;
    vector<interface_member> members;

    void accept(ast_element_visitor& visitor) override
    {
      visitor.visit(*this);
    }
  };

  struct property : ast_element
  {
    vector<wstring> names;
    type_specification type;
    bool is_constant{ false };
    wstring default_value;
    
    void accept(ast_element_visitor& visitor) override
    {
      visitor.visit(*this);
    }
  };

  typedef variant<function_body, property, function_signature> class_member;

  struct class_declaration : ast_element
  {
    vector<wstring> name;
    vector<parameter_declaration> primary_constructor_parameters;
    vector<class_member> members;

    void accept(ast_element_visitor& visitor) override {
      visitor.visit(*this);
    }

    bool is_abstract() const
    {
      bool result = false;
      for (const auto& m : members)
        if (m.type() == typeid(function_signature))
          result = true;
      return result;
    }
  };

  struct enum_declaration : ast_element
  {
    vector<wstring> name;
    vector<wstring> members;

    void accept(ast_element_visitor& visitor) override {
      visitor.visit(*this);
    }
  };

  typedef variant<
    interface_declaration, 
    class_declaration, 
    enum_declaration> top_level_declaration;

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
  tlön::basic_type,
  (std::vector<std::wstring>, path_to_type),
  (std::wstring, name)
)

BOOST_FUSION_ADAPT_STRUCT(
  tlön::assignment_statement,
  (std::vector<std::wstring>, names),
  (std::wstring, value)
)

BOOST_FUSION_ADAPT_STRUCT(
  tlön::property,
  (std::vector<std::wstring>, names),
  (tlön::type_specification, type),
  (bool, is_constant),
  (std::wstring, default_value)
)

BOOST_FUSION_ADAPT_STRUCT(
  tlön::tuple_signature_element,
  (boost::optional<std::wstring>, name),
  (tlön::basic_type, type)
)

BOOST_FUSION_ADAPT_STRUCT(
  tlön::tuple_signature,
  (std::vector<tlön::tuple_signature_element>, elements)
)

BOOST_FUSION_ADAPT_STRUCT(
  tlön::parameter_declaration,
  (std::vector<std::wstring>, names),
  (tlön::type_specification, type),
  (std::wstring, default_value)
)

BOOST_FUSION_ADAPT_STRUCT(
  tlön::anonymous_function_signature,
  (std::vector<tlön::parameter_declaration>, parameters),
  (tlön::type_specification, return_type)
)

BOOST_FUSION_ADAPT_STRUCT(
  tlön::function_signature,
  (std::wstring, name),
  (tlön::anonymous_function_signature, signature)
)

BOOST_FUSION_ADAPT_STRUCT(
  tlön::function_body,
  (std::wstring, name),
  (std::vector<tlön::parameter_declaration>, parameters),
  (tlön::type_specification, return_type),
  (tlön::block, block)
)

BOOST_FUSION_ADAPT_STRUCT(
  tlön::class_declaration,
  (std::vector<std::wstring>, name),
  (std::vector<tlön::parameter_declaration>, primary_constructor_parameters),
  (std::vector<tlön::class_member>, members)
)

BOOST_FUSION_ADAPT_STRUCT(
  tlön::enum_declaration,
  (std::vector<std::wstring>, name),
  (std::vector<std::wstring>, members)
)

BOOST_FUSION_ADAPT_STRUCT(
  tlön::interface_declaration,
  (std::vector<std::wstring>, name),
  (std::vector<tlön::basic_type>, parents),
  (std::vector<tlön::interface_member>, members)
)

BOOST_FUSION_ADAPT_STRUCT(
  tlön::file,
  (std::vector<tlön::top_level_declaration>, declarations)
)

BOOST_FUSION_ADAPT_STRUCT(
  tlön::block,
  (std::vector<tlön::statement>, statements),
  (boost::spirit::qi::unused_type, dummy)
)