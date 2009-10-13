// $Id: smartpointer.hh 5504 2009-04-08 13:35:31Z christi $

#ifndef DUNE_SHARED_PTR_HH
#define DUNE_SHARED_PTR_HH

/**
 * @file 
 * @brief This file implements the class shared_ptr (a reference counting
 * pointer), for those systems that don't have it in the standard library.
 * @author Markus Blatt
 */
namespace Dune
{
    /** @addtogroup Common
     *
     * @{
     */
    /**
     * @brief A reference counting smart pointer.
     *
     * It is designed such that it is usable within a std::vector.
     * The contained object is destroyed only if there are no more
     * references to it.
     */
    template<class T>
    class shared_ptr
    {
    public:
        /**
         * @brief The data type we are a pointer for.
         *
         * This has to have a parameterless constructor.
         */
        typedef T element_type;

        /** 
         * @brief Constructs a new smart pointer and allocates the referenced Object.
         */
        inline shared_ptr();
        
        /** 
         * @brief Constructs a new smart pointer from a preallocated Object.
         *
         * note: the object must be allocated on the heap and after handing the pointer to
         * shared_ptr the ownership of the pointer is also handed to the shared_ptr.
         */
        inline shared_ptr(T * pointer);
        
        /**
         * @brief Copy constructor.
         * @param pointer The object to copy.
         */
        inline shared_ptr(const shared_ptr<T>& pointer);
        
        /**
         * @brief Destructor.
         */
        inline ~shared_ptr();

        /** \brief Assignment operator */
        inline shared_ptr& operator=(const shared_ptr<T>& pointer);
        
        /** \brief Dereference as object */
        inline element_type& operator*();
        
        /** \brief Dereference as pointer */
        inline element_type* operator->();
      
        /** \brief Dereference as const object */
        inline const element_type& operator*() const;
        
        /** \brief Dereference as const pointer */
        inline const element_type* operator->() const;

        /**
         * @brief Deallocates the references object if no other
         * pointers reference it.
         */
        inline void deallocate();

        /** \brief The number of shared_ptrs pointing to the object we point to */
        int use_count() const;

    private:
        /** @brief The object we reference. */  
        class PointerRep
        {
            friend class shared_ptr<element_type>;
            /** @brief The number of references. */
            int count_;
            /** @brief The representative. */
            element_type * rep_;
            /** @brief Default Constructor. */
            PointerRep() : count_(1), rep_(new element_type) {}
            /** @brief Constructor from existing Pointer. */
            PointerRep(element_type * p) : count_(1), rep_(p) {}
            /** @brief Destructor, deletes element_type* rep_. */
            ~PointerRep() { delete rep_; }
        } *rep_;
    };

    template<class T>
    inline shared_ptr<T>::shared_ptr(T * p)
    {
        rep_ = new PointerRep(p);
    }

    template<class T>
    inline shared_ptr<T>::shared_ptr()
    {
        rep_ = new PointerRep;
    }

    template<class T>
    inline shared_ptr<T>::shared_ptr(const shared_ptr<T>& other) : rep_(other.rep_)
    {
        ++(rep_->count_);
    }

    template<class T>
    inline shared_ptr<T>& shared_ptr<T>::operator=(const shared_ptr<T>& other)
    {
        (other.rep_->count_)++;
        if(rep_!=0 && --(rep_->count_)<=0) delete rep_;
        rep_ = other.rep_;
        return *this;
    }

    template<class T>
    inline shared_ptr<T>::~shared_ptr()
    {
        if(rep_!=0 && --(rep_->count_)==0){
            delete rep_;
            rep_=0;
        }
    }

    template<class T>
    inline T& shared_ptr<T>::operator*()
    {
        return *(rep_->rep_);
    }

    template<class T>
    inline T *shared_ptr<T>::operator->()
    {
        return rep_->rep_;
    }

    template<class T>
    inline const T& shared_ptr<T>::operator*() const
    {
        return *(rep_->rep_);
    }

    template<class T>
    inline const T *shared_ptr<T>::operator->() const
    {
        return rep_->rep_;
    }
    
    template<class T>
    inline int shared_ptr<T>::count() const
    {
        return rep_->count_;
    }

    template<class T>
    inline void shared_ptr<T>::deallocate()
    {
        assert(rep_!=0 && rep_->count_==1);
        delete rep_;
        rep_=0;
    }
    /** @} */
}
#endif
