#ifndef __MISC_HH__
#define __MISC_HH__

#include<iostream>
#include<math.h>

namespace Dune {
/** @addtogroup Common

	@{
 */

template<class T>
T ABS (T t)
{
	if (t<0) return -t;
	return t;
}

template<class T>
T MAX (T t1, T t2)
{
	if (t1<t2) return t2;
	return t1;
}

template<class T>
T MIN (T t1, T t2)
{
	if (t1<t2) return t1;
	return t2;
}

/** @} */

}


#endif
