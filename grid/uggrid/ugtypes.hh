#ifndef DUNE_UGTYPES_HH
#define DUNE_UGTYPES_HH

template <int codim, int dim>
class TargetType
{
public:
    typedef void T;

};

template <>
class TargetType<0,3>
{
public:
    typedef UG3d::ELEMENT T;
};

template <>
class TargetType<3,3>
{
public:
    typedef UG3d::NODE T;
};


#endif
