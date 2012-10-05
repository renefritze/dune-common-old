#ifndef DUNE_MALLOC_ALLOCATOR_HH
#define DUNE_MALLOC_ALLOCATOR_HH

#include <exception>
#include <cstdlib>
#include <new>

namespace Dune
{
    /**
       @ingroup Allocators
       @brief Allocators implementation which simply calls malloc/free
    */
    template <class T>
    class MallocAllocator {
    public:
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T value_type;
        template <class U> struct rebind {
            typedef MallocAllocator<U> other;
        };

        //! create a new MallocAllocator
        MallocAllocator() throw() {}
        //! copy construct from an other MallocAllocator, possibly for a different result type
        template <class U>
        MallocAllocator(const MallocAllocator<U>&) throw() {}
        //! cleanup this allocator
        ~MallocAllocator() throw() {}
        
        pointer address(reference x) const
        {
            return &x;
        }
        const_pointer address(const_reference x) const
        {
            return &x;
        }

        //! allocate n objects of type T
        pointer allocate(size_type n,
            const void* hint = 0)
        {
            if (n > this->max_size())
                throw std::bad_alloc();

            pointer ret = static_cast<pointer>(std::malloc(n * sizeof(T)));
            if (!ret)
                throw std::bad_alloc();
            return ret;
        }

        //! deallocate n objects of type T at address p
        void deallocate(pointer p, size_type n)
        {
            std::free(p);
        }

        //! max size for allocate
        size_type max_size() const throw()
        {
            return size_type(-1) / sizeof(T);
        }
        
        //! copy-construct an object of type T (i.e. make a placement new on p)
        void construct(pointer p, const T& val)
        {
            ::new((void*)p) T(val);
        }
#if HAVE_VARIADIC_TEMPLATES || DOXYGEN
        //! construct an object of type T from variadic parameters
        //! \note works only with newer C++ compilers
        template<typename... _Args>
        void construct(pointer p, _Args&&... __args)
        {
            ::new((void *)p) _Tp(std::forward<_Args>(__args)...);
        }
#endif
        //! destroy an object of type T (i.e. call the Destructor)
        void destroy(pointer p)
        {
            p->~T();
        }
    };
}

#endif // DUNE_MALLOC_ALLOCATOR_HH
