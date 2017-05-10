#include "tlön.h"
namespace Demo
{
  class ChangeMe : public tlön::object<ChangeMe>
  {
  public:
    ChangeMe(int16_t x_0, int16_t y) :
      x_0{ x_0 },
      y{ y } {}
  protected:
    int16_t x_0;
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
    int16_t y;
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
    int32_t z_this_fails;
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
    const tlön::reflection::type_info& get_type_info() const override { return {}; }
    static tlön::reflection::type_info type_info;
  }; /* ChangeMe */
  tlön::reflection::type_info ChangeMe::type_info =
    tlön::reflection::type_info
  {
    L"Demo",
    L"ChangeMe",
    {
    },{
    } };
} /* Demo */

namespace Demo
{
  class SomeInterface : public tlön::object<SomeInterface>
  {
    virtual ~SomeInterface() = default;
    virtual int32_t add(int32_t a, int32_t b, std::wstring c) = 0;
    virtual std::tuple<std::wstring, uint8_t> tupledemo(std::tuple<int8_t, std::wstring> x) = 0;
  }; /* SomeInterface */
} /* Demo */

