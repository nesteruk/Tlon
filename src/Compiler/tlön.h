#pragma once
#include <vector>
#include <string>
#include <boost/functional/hash.hpp>

namespace tlön
{
  namespace reflection
  {
    struct method_info
    {

    };

    struct property_info
    {

    };

    struct type_info
    {
      const wchar_t* namespace_name;
      const wchar_t* name;

      std::vector<method_info> methods;
      std::vector<property_info> properties;
    };
  }

  template <typename Underlying>
  struct object
  {
    //! String representation of this object.
    virtual std::wstring str() const
    {
      return get_type_info().name;
    };

    //! Compares this object to some other object
    //! \param other the object to compare with
    virtual bool equals(const Underlying& other)
    {
      return str() == other.str();
    }

    //! Default hashcode implementation takes the hash code of the string representation.
    virtual size_t hash() const
    {
      static boost::hash<std::wstring> hash;
      return hash(str());
    }

    virtual const reflection::type_info& get_type_info() const = 0;
  };
}
