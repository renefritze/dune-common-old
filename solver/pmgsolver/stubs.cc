#include "stubs.hh"

namespace Dune {
  namespace PMGStubs {
    /** 
        one Gauss-Seidel step
    */
    template <class GRID, int SMOOTHER, int TYP>
    class GaussSeidel {
      typedef typename GRID::level level;
      enum { DIM = GRID::griddim };
      const pmgsolver<GRID,SMOOTHER> & solver;
      Dune::Vector<GRID> & x;
      Dune::Vector<GRID> & b;
      array<DIM> add;
    public:
      GaussSeidel(const pmgsolver<GRID,SMOOTHER> & solver_,
                  level l) :
        solver(solver_), x(solver.x), b(solver.b),
        add(solver.g.init_add(l))
        {}
      void evaluate(int l, const array<DIM> & coord, int i) {
        // something to store the coeffs
        static coefflist cl = solver.discrete.newcoefflist();
        switch(TYP) {
        case Inner: {
          /* Coeffs */
          solver.discrete.coeffs(cl, solver.g, l, add, coord, i);          
#ifndef NDEBUG
          coefflist cl2 = cl;
          typename GRID::iterator it(i,solver.g);
          solver.discrete.coeffs(cl2, it);
          assert(cl.size == cl2.size);
          for (int n=0; n<cl.size; n++) {
            assert(cl.j[n] == cl2.j[n]);
            assert(cl.aij[n] == cl2.aij[n]);
          }        
          assert(cl.aii == cl2.aii);
#endif
          break;
        } // end Inner
        case Border: {
          /* Boundry */
          typename GRID::iterator it(i,solver.g);
          if (! it->owner() ) return;
          if ( solver.discrete.bc.isdirichlet(it) ) return;
          /* Coeffs */
          solver.discrete.coeffs(cl, it);
          break;
        } // end Border
        } // end sitch

        x[i]=b[i];
        for (int n=0; n<cl.size; n++) {
          int j = cl.j[n];
          x[i] -= cl.aij[n] * x[j];
        }
        x[i] /= cl.aii;
      } // end evaluate
    };

    /**
       the local defect
     */
    template <class GRID, int SMOOTHER, int TYP>
    class Defect {
      typedef typename GRID::level level;
      enum { DIM = GRID::griddim };
      const pmgsolver<GRID,SMOOTHER> & solver;
      Dune::Vector<GRID> & x;
      Dune::Vector<GRID> & b;
      array<DIM> add;
    public:
      array<2,double> defect_array;
      Defect(const pmgsolver<GRID,SMOOTHER> & solver_,
             level l) :
        solver(solver_), x(solver.x), b(solver.b),
        add(solver.g.init_add(l)), defect_array(0)
        {}
      void evaluate(level l, const array<DIM> & coord, int i) {
        /* do something */
        double defect = localdefect(l, coord, i);
        // update process defect
        defect_array[0] += defect*defect;
        defect_array[1] ++;
      }
      double localdefect(level l, const array<DIM> & coord, int i) const {
        double defect=b[i];          
        // something to store the coeffs
        static coefflist cl = solver.discrete.newcoefflist();
        switch(TYP) {
        case Inner: {
          /* Coeffs */
          solver.discrete.coeffs(cl, solver.g, l, add, coord, i);          
          break;
        }
        case Border: {
          /* Boundry */
          typename GRID::iterator it(i,solver.g);
          if (! it->owner() ) return 0;
          if ( solver.discrete.bc.isdirichlet(it) ) return 0;
          /* Coeffs */
          solver.discrete.coeffs(cl, it);
          break;
        } // end Border
        } // end sitch
        /* Calc the defect */
        defect -= cl.aii * solver.x[i];        
        for (int n=0; n<cl.size; n++) {
          int j = cl.j[n];
          defect -= cl.aij[n] * solver.x[j];
        } // end Inner
#ifndef NDEBUG
        if (!finite(defect))
          std::cerr << "DEFECT ERROR Element " << i
                    << coord << std::endl;
        assert(finite(defect));
#endif
        solver.d[i]=defect;          
        return defect;
      } // end evaluate
    };

    template <class GRID, int SMOOTHER, int TYP>
    class Restrict {
      typedef typename GRID::level level;
      enum { DIM = GRID::griddim };
      pmgsolver<GRID,SMOOTHER> & solver;
      GRID & g;
      array<DIM> coord_shift;
      level L;
    public:
      Restrict(pmgsolver<GRID,SMOOTHER> & solver_, level l) :
        solver(solver_), g(solver.g), L(l)
        {
          for(int d=0; d<DIM; d++)
            coord_shift[d] = solver.g.has_coord_shift(l,d);
        }
      void evaluate(level l, const array<DIM> & coord, int i) {
        switch (TYP) {
        case Inner: {
          adddefect(solver.d[i], DIM, l, coord);
          break;
        }
        case Border: {
          typename GRID::iterator it(i,g);
          adddefect(solver.d[i], DIM, it);
          break;
        }
        }
      }
      void adddefect(double d, int dir,
                     typename GRID::level l, array<DIM> coord){
        dir--;
		
        if (dir<0) {
          int f = g.father_id(l,coord);
          solver.b[f] += d;
          return;
        }
		
        if (coord[dir]%2==coord_shift[dir]) {
          adddefect(d,dir,l,coord);
        }
        else {
          array<DIM> shiftl=coord;
          array<DIM> shiftr=coord;
          shiftl[dir]-=1;    
          shiftr[dir]+=1;    
          if (! (!g.do_end_share(dir) &&
                 coord[dir] ==
                 g.size(l,dir)
                 +g.end_overlap(l,dir) +g.front_overlap(l,dir)-1)) {
            adddefect(d/2.0,dir,l,shiftr);
          }
          if (! (!g.do_front_share(dir) &&
                 coord[dir]==0)) {
            adddefect(d/2.0,dir,l,shiftl);
          }
        }
      }	    
      void adddefect(double d, int dir,
                     typename GRID::iterator it){
        dir--;
		
        if (dir<0) {
          typename GRID::iterator f = it.father();
          solver.b[f.id()] += d;
          return;
        }
        if (it.coord(dir)%2==coord_shift[dir]) {
          adddefect(d,dir,it);
        }
        else {
          typename GRID::iterator left = it.left(dir);
          typename GRID::iterator right = it.right(dir);
          if (it!=left)
            adddefect(d/2.0,dir,left);
          if (it!=right)
            adddefect(d/2.0,dir,right);
        }
      }	    
    };

    template <class GRID, int SMOOTHER, int TYP>
    class Prolongate {
      typedef typename GRID::level level;
      enum { DIM = GRID::griddim };
      pmgsolver<GRID,SMOOTHER> & solver;
      GRID & g;
      array<DIM> coord_shift;
      Dune::Vector<GRID> & x;
    public:
      Prolongate(pmgsolver<GRID,SMOOTHER> & solver_, level l) :
        solver(solver_), g(solver.g), x(solver.x)
        {
          for(int d=0; d<DIM; d++)
            coord_shift[d] = solver.g.has_coord_shift(l,d);    
        }
      void evaluate(level l, const array<DIM> & coord, int i) {
        switch (TYP) {
        case Inner: {
          x[i] += correction(DIM, l, coord);
          break;
        }
        case Border: {
          typename GRID::iterator it(i,g);
          if (!it->owner()) return;
          x[i] += correction(DIM, it);
          break;
        }
        }
      }
      double correction(int dir, typename GRID::level l,
                        array<DIM> coord) {
        dir--;
        
        if (dir<0) {
          int f=g.father_id(l, coord);
          return x[f];
        }
        
        if (coord[dir]%2==coord_shift[dir])
          return correction(dir,l,coord);
        else {
          array<DIM> shiftl=coord;
          array<DIM> shiftr=coord;
          shiftl[dir]-=1;    
          shiftr[dir]+=1;    
          return 0.5*correction(dir,l,shiftl) +
            0.5*correction(dir,l,shiftr);
        }
      }
      double correction(int dir, typename GRID::iterator it) {
        dir--;
        
        if (dir<0) {
          typename GRID::iterator f=it.father();
          return x[f.id()];
        }
        
        if (it.coord(dir)%2==coord_shift[dir])
          return correction(dir,it);
        else {
          typename GRID::iterator left = it.left(dir);
          typename GRID::iterator right = it.right(dir);
          return 0.5*correction(dir,left) +
            0.5*correction(dir,right);
        }
      }
    };

    template <class GRID, int SMOOTHER>
    class InitExchange {
      typedef typename GRID::level level;
      enum { DIM = GRID::griddim };
      pmgsolver<GRID,SMOOTHER> & solver;
    public:
      InitExchange(pmgsolver<GRID,SMOOTHER> & solver_) :
        solver(solver_) {}
      void evaluate(level l, const array<DIM> & coord, int id) {
        typename GRID::iterator it(id,solver.g);
        typename GRID::index i=*it;
        typename GRID::remotelist remote=i.remote();
        /* if i own the data, I'll search all processes
           to receive the data */
        if(i.owner()) {
          for (int r=0; r<remote.size; r++) {
            int & size = solver.exchange_data_to[l][remote.list[r].process()].size;
            realloc<int>(solver.exchange_data_to[l][remote.list[r].process()].id, size + 1);
            solver.exchange_data_to[l][remote.list[r].process()].id[size] = it.id();
            size ++;
          }
        }
        /* if I share the data, find the owner-processes */
        else {
          for (int r=0; r<remote.size; r++) {
            if (remote.list[r].owner()) {
              int & size = solver.exchange_data_from[l][remote.list[r].process()].size;
              realloc<int>(solver.exchange_data_from[l][remote.list[r].process()].id, size + 1);
              solver.exchange_data_from[l][remote.list[r].process()].id[size] = it.id();
              size ++;
              continue;
            }
          }
        }
      }
    };
	
    template <class GRID>
    class InitIterator {
      enum { DIM = GRID::griddim };
      typedef typename GRID::level level;
      Dune::Vector<GRID> & b;
      Dune::Vector<GRID> & x;
      Discrete<GRID> & discrete;
      GRID & g;
    public:
      InitIterator(Dune::Vector<GRID> & B, Dune::Vector<GRID> & X,
                           Discrete<GRID> & D, GRID & G) :
        b(B), x(X), discrete(D), g(G) {};
      void evaluate(level l, const array<DIM> & coord, int i) {
        typename GRID::iterator it(i,g);
        b[i] = discrete.rhs(it);
        if (discrete.bc.isdirichlet(it))
          for (int d=0; d<DIM; d++) {
            for (Dune::side s = Dune::left; s <= Dune::right; s++) {
              Boundry bd = discrete.bc.boundry(it,d,s);
              if (bd.typ == dirichlet) x[i]=bd.value;
            }
          }
      }
    };

  } // PMGStubs
} // Dune
