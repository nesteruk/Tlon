#include "tlön.h"
namespace ActiveMesa
{
  enum class Color
  {
    red,
    green,
    blue
  };

  std::ostream& operator<<(std::ostream& os, const Color& obj)
  {
    switch (obj)
    {
    case Color::red: return os << "red";
    case Color::green: return os << "green";
    case Color::blue: return os << "blue";
    default: return os << "Unknown case of Color";
    }
  }
} /* ActiveMesa */

namespace Demo
{
  class ChangeMe : public tlön::object<ChangeMe>
  {
  public:
    ChangeMe(int16_t x_0, int16_t y) :
      x_0{ x_0 },
      y{ y } {}
  protected:
    int16_t x_0{ 0 };
  public:
    void set_x_0(const int16_t x_0)
    {
      this->x_0 = x_0;
    }

    int16_t get_x_0() const
    {
      return x_0;
    }

  protected:
    int16_t y{ 0 };
  public:
    void set_y(const int16_t y)
    {
      this->y = y;
    }

    int16_t get_y() const
    {
      return y;
    }


  protected:
    int32_t z_this_fails{ 0 };
  public:
    void set_z_this_fails(const int32_t z_this_fails)
    {
      this->z_this_fails = z_this_fails;
    }

    int32_t get_z_this_fails() const
    {
      return z_this_fails;
    }

  public:
    const tlön::reflection::type_info& get_type_info() const override
    {
      return this->type_info;
    }
    static tlön::reflection::type_info type_info;
  };
  /* ChangeMe */
};
} /* Demo */

namespace Demo
{
  class SomeInterface : public tlön::object<SomeInterface>
  {
    virtual ~SomeInterface() = default;
    virtual int32_t add(int32_t a, int32_t b, std::wstring c) = 0;
    virtual std::tuple<std::wstring, uint8_t> tupledemo(std::tuple<int8_t, std::wstring> x) = 0;
  };
  /* SomeInterface */
} /* Demo */

