// $Id$

/*

  Implements a generic grid check

*/

template <class Grid>
void gridcheck (Grid &g) {
  // internal check if interface is complete
  g.checkIF();
};
