#ifndef RESERVEDVECTOR_HH
#define RESERVEDVECTOR_HH

#include <iostream>
#include <dune/common/genericiterator.hh>

#ifdef CHECK_RESERVEDVECTOR
#define CHECKSIZE(X) assert(X)
#else
#define CHECKSIZE(X) {}
#endif

namespace Dune
{
    /**
       \brief A Vector class with statically reserved memory.

       ReservedVector is something between Dune::array and std::vector.
       You have vector which can be extended and shrunk using methods like
       push_back and pop_back, but reserved memory is predefined.

       This implies that the vector can not grow bigger than the predefined
       maximum size.

       \tparam T The data type ReservedVector stores.
       \tparam n The maximum number of objects the ReservedVector can store.
       
     */
    template<class T, int n>
    class ReservedVector
    {
    public:

        /** @{ Typedefs */
        
        //! The type of object, T, stored in the vector.
        typedef T value_type;
        //! Pointer to T.
        typedef T* pointer;
        //! Reference to T
        typedef T& reference;
        //! Const reference to T
        typedef const T& const_reference;
        //! An unsigned integral type.
        typedef size_t size_type;
        //! A signed integral type.
        typedef std::ptrdiff_t difference_type;
        //! Iterator used to iterate through a vector.
        typedef Dune::GenericIterator<ReservedVector, value_type> iterator;
        //! Const iterator used to iterate through a vector.
        typedef Dune::GenericIterator<const ReservedVector, const value_type> const_iterator;

        /** @} */
        
        /** @{ Constructors */

        //! Constructor
        ReservedVector() : sz(0) {}
        
        /** @} */

        /** @{ Data access operations */

        //! Erases all elements.
        void clear()
        {
            sz = 0;
        }

        //! Specifies a new size for the vector.
        void resize(size_t s)
        {
            CHECKSIZE(s<=n);
            sz = s;
        }
        
        //! Appends an element to the end of a vector, up to the maximum size n, O(1) time.
        void push_back(const T& t)
        {
            CHECKSIZE(sz<n);
            data[sz++] = t;
        }
        
        //! Erases the last element of the vector, O(1) time.
        void pop_back()
        {
            if (! empty()) sz--;
        }

        //! Returns a iterator pointing to the beginning of the vector.
        iterator begin(){
            return iterator(*this, 0);
        }

        //! Returns a const_iterator pointing to the beginning of the vector.
        const_iterator begin() const{    
            return const_iterator(*this, 0);
        }

        //! Returns an iterator pointing to the end of the vector.
        iterator end(){
            return iterator(*this, sz);
        }

        //! Returns a const_iterator pointing to the end of the vector.
        const_iterator end() const{    
            return const_iterator(*this, sz);
        }
  
        //! Returns reference to the i'th element. 
        reference operator[] (size_type i)
        {
            CHECKSIZE(sz>i);
            return data[i];
        }

        //! Returns a const reference to the i'th element. 
        const_reference operator[] (size_type i) const
        {
            CHECKSIZE(sz>i);
            return data[i];
        }

        //! Returns reference to first element of vector.
        reference front()
        {
            CHECKSIZE(sz>0);
            return data[0];
        }

        //! Returns const reference to first element of vector.
        const_reference front() const
        {
            CHECKSIZE(sz>0);
            return data[0];
        }

        //! Returns reference to last element of vector.
        reference back()
        {
            CHECKSIZE(sz>0);
            return data[sz-1];
        }

        //! Returns const reference to last element of vector.
        const_reference back() const
        {
            CHECKSIZE(sz>0);
            return data[sz-1];
        }

        /** @} */

        /** @{ Informative Methods */
        
        //! Returns number of elements in the vector.
        size_type size () const
        {
            return sz;
        }

        //! Returns true if vector has no elements.
        bool empty() const
        {
            return sz==0;
        }

        //! Returns current capacity (allocated memory) of the vector.
        static size_type capacity()
        {
            return n;
        }

        //! Returns the maximum length of the vector.
        static size_type max_size()
        {
            return n;
        }

        /** @} */
        
        //! Send ReservedVector to an output stream
        friend std::ostream& operator<< (std::ostream& s, const ReservedVector& v)
        {
            for (size_t i=0; i<v.size(); i++)
                s << v[i] << "  ";
            return s;
        }

    private:
        T data[n];
        size_type sz;
    };
    
}

#undef CHECKSIZE

#endif // RESERVEDVECTOR_HH