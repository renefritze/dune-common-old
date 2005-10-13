#include <config.h>

/*

  Instantiate Alberta-Grid and feed it to the generic gridcheck()

  Note: Albert needs the defines DIM and DIM_OF_WORLD on the
  commandline anyway thus we can use them to select the correct class

*/

#include <iostream>
#include <sstream>

#include <dune/grid/albertagrid.hh>

#include "gridcheck.cc"

int main () {
  try {
    /* use grid-file appropriate for dimensions */
    std::ostringstream filename;
    filename << "alberta-testgrid-" << DUNE_PROBLEM_DIM
             << "-" << DUNE_WORLD_DIM << ".al";

    std::cout << std::endl << "AlbertaGrid<" << DUNE_PROBLEM_DIM
              << "," << DUNE_WORLD_DIM 
              << "> with grid file: " << filename.str() 
              << std::endl << std::endl;

    // extra-environment to check destruction
    {
      factorEpsilon = 500.0;
      Dune::AlbertaGrid<DUNE_PROBLEM_DIM,DUNE_WORLD_DIM>
        grid(filename.str());
      
      gridcheck(grid); // check macro grid
      for(int i=0; i<3; i++)
      {
        grid.globalRefine(1);
        gridcheck(grid);
      }
    };

  } catch (Dune::Exception &e) {
    std::cerr << e << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Generic exception!" << std::endl;
    return 2;
  }

  return 0;
};
