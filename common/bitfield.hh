#ifndef DUNE_BITFIELD_HH
#define DUNE_BITFIELD_HH

#include <vector>

namespace Dune {

    /** \brief A dynamic array of booleans
     * \ingroup Common
     *
     * This class is basically std::vector<bool>, but with a few added
     * methods.
     */
    class BitField : public std::vector<bool> {

    public:

        //! Sets all entries to <tt> true </tt>
        void setAll() {
            for (int i=0; i<size(); i++)
                (*this)[i] = true;
        }

        //! Sets all entries to <tt> false </tt>
        void unsetAll() {
            for (int i=0; i<size(); i++)
                (*this)[i] = false;
        }

        //! Returns the number of set bits
        int nSetBits() const {
            int n = 0;
            for (int i=0; i<size(); i++)
                n += ((*this)[i]) ? 1 : 0;

            return n;
        }

        //! Send bitfield to an output stream
        friend std::ostream& operator<< (std::ostream& s, const BitField& v)
        {
            for (int i=0; i<v.size(); i++)
                s << v[i] << "  ";
            
            s << std::endl;
            return s;
        }

    };

}

#endif
