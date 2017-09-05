#include <iostream>
#include <string>
using namespace std;
#include "sandbox.hpp"

int main(int argc, char* argv[])
{
  auto red = ActiveMesa::Color::red;
  cout << red << endl;

  //Demo::ChangeMe cm{0,0};
  //cout << cm.get_type_info().name << endl;
}
