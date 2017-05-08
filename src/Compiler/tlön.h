#pragma once
#include <vector>
#include <boost/functional/hash_fwd.hpp>

namespace tlön
{
  template <typename Underlying>
  struct object
  {
    virtual std::wstring str() = 0;
    virtual bool equals(const Underlying& other) = 0;
    virtual size_t hash() const
    {
      static boost::hash<Underlying> h;
      return h(*this);
    }
  };

  namespace reflection
  {
    struct method_info
    {
      
    };

    struct property_info
    {

    };

    struct class_info
    {
      const wchar_t* namespace_name;
      const wchar_t* name;

      std::vector<method_info> methods;
      std::vector<property_info> properties;
    };

  }
}
