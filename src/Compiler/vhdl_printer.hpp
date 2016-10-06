#pragma once
#include "printer.hpp"
#include "chars.h"

namespace tlön
{
  namespace printers
  {
    struct vhdl_printer : printer
    {
      void visit(const assignment_statement& obj) override;
      void visit(const parameter_declaration& obj) override;
      void visit(const interface_function_signature& obj) override;
      void visit(const interface_declaration& obj) override;
      void visit(const class_declaration& obj) override;
      void visit(const file& obj) override;
      void visit(const tuple_signature_element& obj) override;
      void visit(const tuple_signature& obj) override;
      void visit(const property& obj) override;
      void visit(const function_body& obj) override;
    };

    inline void vhdl_printer::visit(const assignment_statement& obj)
    {
    }

    inline void vhdl_printer::visit(const parameter_declaration& obj)
    {
    }

    inline void vhdl_printer::visit(const interface_function_signature& obj)
    {
    }

    inline void vhdl_printer::visit(const interface_declaration& obj)
    {
    }

    inline void vhdl_printer::visit(const class_declaration& obj)
    {
    }

    inline void vhdl_printer::visit(const file& obj)
    {
    }

    inline void vhdl_printer::visit(const tuple_signature_element& obj)
    {
    }

    inline void vhdl_printer::visit(const tuple_signature& obj)
    {
    }

    inline void vhdl_printer::visit(const property& obj)
    {
    }

    inline void vhdl_printer::visit(const function_body& obj)
    {
    }
  }
}
