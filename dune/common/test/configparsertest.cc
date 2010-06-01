// -*- tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi: set et ts=8 sw=2 sts=2:
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <string>

#include <dune/common/configparser.hh>
#include <dune/common/exceptions.hh>

int main ()
{
  try {
    Dune::ConfigParser parameterSet;

    parameterSet["testDouble"] = "3.14";
    parameterSet["testInt"]    = "42";
    parameterSet["testString"] = "Hallo Welt!";

    double testDouble      = parameterSet.get<double>("testDouble");
    int testInt            = parameterSet.get<int>("testInt");
    std::string testString = parameterSet.get<std::string>("testString");
  }
  catch(const Dune::Exception& e) {
    std::cerr << "Exception thrown: " << e << std::endl;
    throw;
  }
}



