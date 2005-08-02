
namespace Dune {

  //- Trilinear mapping (from alu3dmappings.hh)
  inline TrilinearMapping :: 
  TrilinearMapping (const coord_t& x0, const coord_t& x1,
                    const coord_t& x2, const coord_t& x3, 
                    const coord_t& x4, const coord_t& x5, 
                    const coord_t& x6, const coord_t& x7) 
    : p0(x0), p1(x1), p2(x2), p3(x3), p4(x4), p5(x5), p6(x6), p7(x7) {
    a [0][0] = p0 [0] ;
    a [0][1] = p0 [1] ;
    a [0][2] = p0 [2] ;
    a [1][0] = p1 [0] - p0 [0] ;
    a [1][1] = p1 [1] - p0 [1] ;
    a [1][2] = p1 [2] - p0 [2] ;
    a [2][0] = p2 [0] - p0 [0] ;
    a [2][1] = p2 [1] - p0 [1] ;
    a [2][2] = p2 [2] - p0 [2] ;
    a [3][0] = p4 [0] - p0 [0] ;
    a [3][1] = p4 [1] - p0 [1] ;
    a [3][2] = p4 [2] - p0 [2] ;
    a [4][0] = p3 [0] - p2 [0] - a [1][0] ;
    a [4][1] = p3 [1] - p2 [1] - a [1][1] ;
    a [4][2] = p3 [2] - p2 [2] - a [1][2] ;
    a [5][0] = p6 [0] - p4 [0] - a [2][0] ;
    a [5][1] = p6 [1] - p4 [1] - a [2][1] ;
    a [5][2] = p6 [2] - p4 [2] - a [2][2] ;
    a [6][0] = p5 [0] - p1 [0] - a [3][0] ;
    a [6][1] = p5 [1] - p1 [1] - a [3][1] ;
    a [6][2] = p5 [2] - p1 [2] - a [3][2] ;
    a [7][0] = p7 [0] - p5 [0] + p4 [0] - p6 [0] - p3 [0] + p1 [0] + a [2][0] ;
    a [7][1] = p7 [1] - p5 [1] + p4 [1] - p6 [1] - p3 [1] + p1 [1] + a [2][1] ;
    a [7][2] = p7 [2] - p5 [2] + p4 [2] - p6 [2] - p3 [2] + p1 [2] + a [2][2] ;
    return ;   
  }

  inline TrilinearMapping :: TrilinearMapping (const TrilinearMapping & map)
    : p0(map.p0), p1(map.p1), p2(map.p2), p3(map.p3),
      p4(map.p4), p5(map.p5), p6(map.p6), p7(map.p7) { 
    for (int i = 0 ; i < 8 ; i ++)
      for (int j = 0 ; j < 3 ; j ++) 
        a [i][j] = map.a [i][j] ;
    return ;
  }

  inline FieldMatrix<double, 3, 3> 
  TrilinearMapping::jacobianInverse(const coord_t& p) {
    inverse (p);
    return Dfi;
  } 

  inline void TrilinearMapping :: 
  map2world(const coord_t& p, coord_t& world) const {
    double x = p [0];
    double y = p [1];
    double z = p [2];
    double t3 = y * z ;
    double t8 = x * z ;
    double t13 = x * y ;
    double t123 = x * t3 ;
    world [0] = a [0][0] + a [1][0] * x + a [2][0] * y + a [3][0] * z + a [4][0] * t13 + a [5][0] * t3 + a [6][0] * t8 + a [7][0] * t123 ;
    world [1] = a [0][1] + a [1][1] * x + a [2][1] * y + a [3][1] * z + a [4][1] * t13 + a [5][1] * t3 + a [6][1] * t8 + a [7][1] * t123 ;
    world [2] = a [0][2] + a [1][2] * x + a [2][2] * y + a [3][2] * z + a [4][2] * t13 + a [5][2] * t3 + a [6][2] * t8 + a [7][2] * t123 ;
    return ;
  }

  inline void TrilinearMapping :: 
  map2world(const double x1, const double x2, 
            const double x3, coord_t& world ) const {
    coord_t map ;
    map [0] = x1 ;
    map [1] = x2 ;
    map [2] = x3 ;
    map2world (map, world) ;
    return ;
  }

  inline void TrilinearMapping :: linear(const coord_t& p ) {
    double x = p[0];
    double y = p[1];
    double z = p[2];
    double t3 = y * z ;
    double t8 = x * z ;
    double t13 = x * y ;
    Df[2][0] = a[1][2] + y * a[4][2] + z * a[6][2] + t3 * a[7][2] ;
    Df[2][1] = a[2][2] + x * a[4][2] + z * a[5][2] + t8 * a[7][2] ;
    Df[1][2] = a[3][1] + y * a[5][1] + x * a[6][1] + t13 * a[7][1] ;
    Df[2][2] = a[3][2] + y * a[5][2] + x * a[6][2] + t13 * a[7][2] ;
    Df[0][0] = a[1][0] + y * a[4][0] + z * a[6][0] + t3 * a[7][0] ;
    Df[0][2] = a[3][0] + y * a[5][0] + x * a[6][0] + t13 * a[7][0] ;
    Df[1][0] = a[1][1] + y * a[4][1] + z * a[6][1] + t3 * a[7][1] ;
    Df[0][1] = a[2][0] + x * a[4][0] + z * a[5][0] + t8 * a[7][0] ;
    Df[1][1] = a[2][1] + x * a[4][1] + z * a[5][1] + t8 * a[7][1] ;

  }

  inline double TrilinearMapping :: det(const coord_t& point ) {
    //  Determinante der Abbildung f:[-1,1]^3 -> Hexaeder im Punkt point.
    linear (point) ;
    return (DetDf = Df.determinant());

  }

  inline void TrilinearMapping :: inverse(const coord_t& p ) {
    //  Kramer - Regel, det() rechnet Df und DetDf neu aus.
    double val = 1.0 / det(p) ;
    Dfi[0][0] = ( Df[1][1] * Df[2][2] - Df[1][2] * Df[2][1] ) * val ;
    Dfi[0][1] = ( Df[0][2] * Df[2][1] - Df[0][1] * Df[2][2] ) * val ;
    Dfi[0][2] = ( Df[0][1] * Df[1][2] - Df[0][2] * Df[1][1] ) * val ;
    Dfi[1][0] = ( Df[1][2] * Df[2][0] - Df[1][0] * Df[2][2] ) * val ;
    Dfi[1][1] = ( Df[0][0] * Df[2][2] - Df[0][2] * Df[2][0] ) * val ;
    Dfi[1][2] = ( Df[0][2] * Df[1][0] - Df[0][0] * Df[1][2] ) * val ;
    Dfi[2][0] = ( Df[1][0] * Df[2][1] - Df[1][1] * Df[2][0] ) * val ;
    Dfi[2][1] = ( Df[0][1] * Df[2][0] - Df[0][0] * Df[2][1] ) * val ;
    Dfi[2][2] = ( Df[0][0] * Df[1][1] - Df[0][1] * Df[1][0] ) * val ;
    return ;
  }

  inline void TrilinearMapping::world2map (const coord_t& wld , coord_t& map ) 
  {
    //  Newton - Iteration zum Invertieren der Abbildung f.
    double err = 10.0 * _epsilon ;
#ifndef NDEBUG
    int count = 0 ;
#endif
    map [0] = map [1] = map [2] = .0 ;
    do {
      coord_t upd ;
      map2world (map, upd) ;
      inverse (map) ;
      double u0 = upd [0] - wld [0] ;
      double u1 = upd [1] - wld [1] ;
      double u2 = upd [2] - wld [2] ;
      double c0 = Dfi [0][0] * u0 + Dfi [0][1] * u1 + Dfi [0][2] * u2 ;
      double c1 = Dfi [1][0] * u0 + Dfi [1][1] * u1 + Dfi [1][2] * u2 ;
      double c2 = Dfi [2][0] * u0 + Dfi [2][1] * u1 + Dfi [2][2] * u2 ;
      map [0] -= c0 ;
      map [1] -= c1 ;
      map [2] -= c2 ;
      err = fabs (c0) + fabs (c1) + fabs (c2) ;
      assert (count ++ < 1000) ;
    } while (err > _epsilon) ;
    return ;
  }

  //- Bilinear surface mapping
  inline BilinearSurfaceMapping :: 
  BilinearSurfaceMapping (const coord3_t& x0, const coord3_t& x1, 
                          const coord3_t& x2, const coord3_t& x3) 
    : _p0 (x0), _p1 (x1), _p2 (x2), _p3 (x3) {
    _b [0][0] = _p0 [0] ;
    _b [0][1] = _p0 [1] ;
    _b [0][2] = _p0 [2] ;
    _b [1][0] = _p1 [0] - _p0 [0] ;
    _b [1][1] = _p1 [1] - _p0 [1] ;
    _b [1][2] = _p1 [2] - _p0 [2] ;
    _b [2][0] = _p2 [0] - _p0 [0] ;
    _b [2][1] = _p2 [1] - _p0 [1] ;
    _b [2][2] = _p2 [2] - _p0 [2] ;
    _b [3][0] = _p3 [0] - _p2 [0] - _b [1][0] ;
    _b [3][1] = _p3 [1] - _p2 [1] - _b [1][1] ;
    _b [3][2] = _p3 [2] - _p2 [2] - _b [1][2] ;
    _n [0][0] = _b [1][1] * _b [2][2] - _b [1][2] * _b [2][1] ;
    _n [0][1] = _b [1][2] * _b [2][0] - _b [1][0] * _b [2][2] ;
    _n [0][2] = _b [1][0] * _b [2][1] - _b [1][1] * _b [2][0] ;
    _n [1][0] = _b [1][1] * _b [3][2] - _b [1][2] * _b [3][1] ;
    _n [1][1] = _b [1][2] * _b [3][0] - _b [1][0] * _b [3][2] ;
    _n [1][2] = _b [1][0] * _b [3][1] - _b [1][1] * _b [3][0] ;
    _n [2][0] = _b [3][1] * _b [2][2] - _b [3][2] * _b [2][1] ;
    _n [2][1] = _b [3][2] * _b [2][0] - _b [3][0] * _b [2][2] ; 
    _n [2][2] = _b [3][0] * _b [2][1] - _b [3][1] * _b [2][0] ;
    return ;
  }

  inline BilinearSurfaceMapping :: 
  BilinearSurfaceMapping (const BilinearSurfaceMapping & m) 
    : _p0(m._p0), _p1(m._p1), _p2(m._p2), _p3(m._p3) {
    {for (int i = 0 ; i < 4 ; i ++)
      for (int j = 0 ; j < 3 ; j ++ )
        _b [i][j] = m._b [i][j] ;
    }
    {for (int i = 0 ; i < 3 ; i ++)
      for (int j = 0 ; j < 3 ; j ++ )
        _n [i][j] = m._n [i][j] ;
    }
    return ;
  }

  inline void BilinearSurfaceMapping :: 
  map2world (const coord2_t& map, coord3_t& wld) const {
    double x = map [0];
    double y = map [1];
    double xy = x * y ;
    wld[0] = _b [0][0] + x * _b [1][0] + y * _b [2][0] + xy * _b [3][0] ;
    wld[1] = _b [0][1] + x * _b [1][1] + y * _b [2][1] + xy * _b [3][1] ;
    wld[2] = _b [0][2] + x * _b [1][2] + y * _b [2][2] + xy * _b [3][2] ;
    return ;
  }

  inline void BilinearSurfaceMapping :: 
  map2world (double x, double y, coord3_t& w) const {
    coord2_t p ;
    p [0] = x ;
    p [1] = y ;
    map2world (p,w) ;
    return ;
  }

  inline void BilinearSurfaceMapping :: 
  normal (const coord2_t& map, coord3_t& normal) const {
    double x = map [0];
    double y = map [1];
    normal [0] = -(_n [0][0] + _n [1][0] * x + _n [2][0] * y);
    normal [1] = -(_n [0][1] + _n [1][1] * x + _n [2][1] * y);
    normal [2] = -(_n [0][2] + _n [1][2] * x + _n [2][2] * y);
    return ;
  }

} // end namespace Dune
