#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <codecvt>
#include <vector>
using namespace std;

#define BOOST_SPIRIT_UNICODE
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/phoenix/phoenix.hpp>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/variant.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/for_each.hpp>
using namespace boost;