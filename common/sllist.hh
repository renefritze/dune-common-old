// $Id$
#ifndef DUNE__SLLIST_HH
#define DUNE__SLLIST_HH

#include<memory>
#include <cassert>
#include<config.h>
#include"iteratorfacades.hh"
#include<ostream>
#include<iostream>

namespace Dune
{  
  /** 
   * @addtogroup Common
   *
   * @{
   */
  /**
   * @file 
   * This file implements a single linked list together with
   * the necessary iterators.
   * @author Markus Blatt
   */
  template<typename T, class A>
  class SLListIterator;
  
  template<typename T, class A>
  class SLListConstIterator;
  
  template<typename T, class A>
  class SLListModifyIterator;
  
  /**
   * @brief A single linked list.
   *
   * The list is capable of insertions at the front and at
   * the end and of removing elements at the front. Those 
   * operations require constant time.
   */
  template<typename T, class A=std::allocator<T> >
  class SLList
  {
    class Element;
    friend class SLListIterator<T,A>;
    friend class SLListConstIterator<T,A>;

  public:

    /**
     * @brief The type we store.
     */
    typedef T MemberType;
    
    /**
     * @brief The allocator to use.
     */
    typedef typename A::template rebind<Element>::other Allocator;

    /**
     * @brief The mutable iterator of the list.
     */
    typedef SLListIterator<T,A> iterator;
    
    /**
     * @brief The mutable iterator of the list.
     */
    typedef SLListConstIterator<T,A> const_iterator;

    /**
     * @brief Constructor.
     */
    SLList();
    
    /**
     * @brief Destructor.
     *
     * Deallocates all elements in the list.
     */
    ~SLList();
    
    /**
     * @brief The type of the iterator capable of deletion
     * and insertion.
     */
    typedef SLListModifyIterator<T,A> ModifyIterator;
    
    /** 
     * @brief Add a new entry to the end of the list.
     * @param item The item to add.
     */
    inline void push_back(const MemberType& item);

    /**
     * @brief Add a new entry to the beginning of the list.
     * @param item The item to add.
     */
    inline void push_front(const MemberType& item);
    
    /**
     * @brief Remove the first item in the list.
     */
    inline void pop_front();
     
     /** @brief Remove all elements from the list. */
    inline void clear();

    /**
     * @brief Get an iterator pointing to the first 
     * element in the list.
     *
     * @return An iterator pointing to the first 
     * element or the end if the list is empty.
     */
    inline iterator begin();
    
    /**
     * @brief Get an iterator pointing to the first 
     * element in the list.
     *
     * @return An iterator pointing to the first 
     * element or the end if the list is empty.
     */
    inline const_iterator begin() const;

    /**
     * @brief Get an iterator pointing before the first 
     * element in the list.
     *
     * To be positioned at a valid entry the operator++()
     * has to be called once.
     * @return An iterator pointing before the first 
     * element.
     * @deprecated There is no use for this function! For insertion
     * or deletion of elements use beginModify() 
     */
    inline iterator oneBeforeBegin() DUNE_DEPRECATED;
    
    
    /**
     * @brief Get an iterator pointing before the first 
     * element in the list.
     *
     * To be positioned at a valid entry the operator++()
     * has to be called once.
     * @return An iterator pointing before the first 
     * element.
     * @deprecated There is no use for this function! For insertion
     * or deletion of elements use beginModify() 
     */
    inline const_iterator oneBeforeBegin() const DUNE_DEPRECATED;
    

    /**
     * @brief Get an iterator capable of deleting and
     * inserting elements.
     *
     * @return Modifying iterator positioned at the beginning
     * of the list.
     */
    inline ModifyIterator beginModify();
    
    /**
     * @brief Get an iterator capable of deleting and
     * inserting elements.
     *
     * @return Modifying iterator positioned after the end
     * of the list.
     */
    inline ModifyIterator endModify();

    /**
     * @brief Get an iterator pointing to the
     * end of the list.
     *
     * @return An iterator pointing to the end.
     */
    inline iterator end();

    /**
     * @brief Get an iterator pointing to the
     * end of the list.
     *
     * @return An iterator pointing to the end.
     */
    inline const_iterator end() const;

    /**
     * @brief Get an iterator pointing to the
     * last element in the list.
     *
     * @return An iterator pointing to the last element.
     */
    inline iterator tail() DUNE_DEPRECATED;
    
    /**
     * @brief Get an iterator pointing to the
     * last element in the list.
     *
     * @return An iterator pointing to the last element.
     */
    inline const_iterator tail() const DUNE_DEPRECATED;
    
    /**
     * @brief Check whether the list is empty.
     *
     * @return True if the list is empty;
     */
    inline bool empty() const;

    /**
     * @brief Get the number of elements the list
     * contains.
     */
    inline int size() const;
    
  private:
    /** \todo Please doc me! */
    struct Element
    {
      /**
       * @brief The next element in the list.
       */
      Element* next_;
      /**
       * @brief The element we hold.
       */
      MemberType item_;

      Element(const MemberType& item);
      
      Element();
      
    };

    /**
     * @brief Delete the next element in the list.
     * @param current Element whose next element should be deleted.
     */
    void deleteNext(Element* current);
    

    /**
     * @brief Delete the next element in the list.
     *
     * If the template parameter watchForTail is true, it is checked whether
     * the deleted element is the tail and therefore the tail must be updated.
     * @param current Element whose next element should be deleted.
     */
    template<bool watchForTail>
    void deleteNext(Element* current);
    /**
     * @brief Insert an element after another one in the list.
     * @param current The element after which we insert.
     * @param item The item to insert.
     */
    void insertAfter(Element* current, const T& item);

    /** @brief Pseudo element before the first entry. */
    Element beforeHead_;
    
    /** 
     * @brief Pointer to he last element in the list. 
     *
     * If list is empty this will point to beforeHead_
     */
    Element* tail_;
    
    /** @brief The allocator we use. */
    Allocator allocator_;
    
    /** brief The number of elements the list holds. */
    int size_;
  };

  /**
   * @brief A mutable iterator for the SLList.
   */
  template<typename T, class A>
  class SLListIterator : public Dune::ForwardIteratorFacade<SLListIterator<T,A>, T, T&, std::size_t>
  {
    friend class SLListConstIterator<T,A>;
    friend class SLListModifyIterator<T,A>;
    friend class SLList<T,A>;
    
  public:
    inline SLListIterator(typename SLList<T,A>::Element* item,
			  SLList<T,A>* sllist)
      : current_(item), list_(sllist)
    {}

    inline SLListIterator()
      : current_(0), list_(0)
    {}

    inline SLListIterator(const SLListModifyIterator<T,A>& other)
      : current_(other.iterator_.current_), list_(other.iterator_.list_)
    {}

    /**
     * @brief Dereferencing function for the iterator facade.
     * @return A reference to the element at the current position.
     */
    inline T& dereference() const
    {
      return current_->item_;
    }
      
    /**
     * @brief Equality test for the iterator facade.
     * @param other The other iterator to check.
     * @return true If the other iterator is at the same position.
     */
    inline bool equals(const SLListConstIterator<T,A>& other) const
    {
      return current_==other.current_;
    }
    
    /**
     * @brief Equality test for the iterator facade.
     * @param other The other iterator to check.
     * @return true If the other iterator is at the same position.
     */
    inline bool equals(const SLListIterator<T,A>& other) const
    {
      return current_==other.current_;
    }
      
    /**
     * @brief Equality test for the iterator facade.
     * @param other The other iterator to check.
     * @return true If the other iterator is at the same position.
     */
    inline bool equals(const SLListModifyIterator<T,A>& other) const
    {
      return current_==other.iterator_.current_;
    }
      
    /**
     * @brief Increment function for the iterator facade.
     */
    inline void increment()
    {
      current_ = current_->next_;
    }

    /**
     * @brief Insert an element in the underlying list after
     * the current position.
     * @param v The value to insert.
     */
    inline void insertAfter(const T& v)const
    {      
      assert(list_ );
      list_->insertAfter(current_, v);
    }

    /**
     * @brief Delete the entry after the current position.
     *
     * @warning This will invalidate all iterators positioned at the delete position! Use with care!
     */
    inline void deleteNext() const
    {
      assert(list_);
      list_->deleteNext(current_);
    }
    
  private:
    /**  @brief The current element. */
    typename SLList<T,A>::Element* current_;
    /** @brief The list we iterate over. */
    SLList<T,A>* list_;
  };
  
  /**
   * @brief A constant iterator for the SLList.
   */
  template<class T, class A>
  class SLListConstIterator : public Dune::ForwardIteratorFacade<SLListConstIterator<T,A>, const T, const T&, std::size_t>
  {
    friend class SLListIterator<T,A>;
    friend class SLList<T,A>;

  public:
    inline SLListConstIterator()
      : current_(0)
    {}

    inline SLListConstIterator(typename SLList<T,A>::Element* item)
      : current_(item)
    {}

    inline SLListConstIterator(const SLListIterator<T,A>& other)
      : current_(other.current_)
    {}
    
    inline SLListConstIterator(const SLListConstIterator<T,A>& other)
      : current_(other.current_)
    {}

    inline SLListConstIterator(const SLListModifyIterator<T,A>& other)
      : current_(other.iterator_.current_)
    {}
      
    /**
     * @brief Dereferencing function for the facade.
     * @return A reference to the element at the current position.
     */
    inline const T& dereference() const
    {
      return current_->item_;
    }
    
    /**
     * @brief Equality test for the iterator facade.
     * @param other The other iterator to check.
     * @return true If the other iterator is at the same position.
     */
    inline bool equals(const SLListConstIterator<T,A>& other) const
    {
      return current_==other.current_;
    }
    
    /**
     * @brief Increment function for the iterator facade.
     */
    inline void increment()
    {
      current_ = current_->next_;
    }
      
  private:
    /**  @brief The current element. */
    typename SLList<T,A>::Element* current_;
  };
  
  /**
   * @brief A mutable iterator for the SLList.
   */
  template<typename T, class A>
  class SLListModifyIterator : public Dune::ForwardIteratorFacade<SLListModifyIterator<T,A>, T, T&, std::size_t>
  {
    friend class SLListConstIterator<T,A>;
    friend class SLListIterator<T,A>;
  public:
    inline SLListModifyIterator(SLListIterator<T,A> beforeIterator,
			  SLListIterator<T,A> _iterator)
      : beforeIterator_(beforeIterator), iterator_(_iterator)
    {}

    inline SLListModifyIterator(const  SLListModifyIterator<T,A>& other)
      : beforeIterator_(other.beforeIterator_), iterator_(other.iterator_)
    {}
    
    inline SLListModifyIterator()
      : beforeIterator_(), iterator_()
    {}
      
    /**
     * @brief Dereferencing function for the iterator facade.
     * @return A reference to the element at the current position.
     */
    inline T& dereference() const
    {
      return *iterator_;
    }
    
    /**
     * @brief Test whether another iterator is equal.
     * @return true if the other iterator is at the same position as
     * this one.
     */
    inline bool equals(const  SLListConstIterator<T,A>& other) const
    {
      return iterator_== other;
    }

    
    /**
     * @brief Test whether another iterator is equal.
     * @return true if the other iterator is at the same position as
     * this one.
     */
    inline bool equals(const  SLListIterator<T,A>& other) const
    {
      return iterator_== other;
    }

    
    /**
     * @brief Test whether another iterator is equal.
     * @return true if the other iterator is at the same position as
     * this one.
     */
    inline bool equals(const  SLListModifyIterator<T,A>& other) const
    {
      return iterator_== other.iterator_;
    }
    
    /**
     * @brief Increment function for the iterator facade.
     */
    inline void increment()
    {
      ++iterator_;
      ++beforeIterator_;
    }

    /**
     * @brief Insert an element at the current position.
     * 
     * Starting from the element at the current position all
     * elements will be shifted by one position to the back.
     * The iterator will point to the same element as before 
     * after the insertion, i.e the number of increments to
     * reach the same position from a begin iterator increases 
     * by one.
     * This means the inserted element is the one before the one
     * the iterator points to.
     * @param v The value to insert.
     */
    inline void insert(const T& v)
    {
      beforeIterator_.insertAfter(v);
      ++beforeIterator_;
    }

    /**
     * @brief Delete the entry at the current position.
     *
     * The iterator will be positioned at the next postion after the
     * deletion
     * @warning This will invalidate all iterators positioned at the delete position! Use with care!
     */
    inline void remove()
    {
      ++iterator_;
      beforeIterator_.deleteNext();
    }
    
  private:
    /** @brief Iterator positioned at the position before the current. */
    SLListIterator<T,A> beforeIterator_;
    /** @brief Iterator positioned at the current position. */
    SLListIterator<T,A> iterator_;
  };
}// namespace Dune

namespace std
{
  
  template<typename T, typename A>
  ostream& operator<<(ostream& os, const Dune::SLList<T,A> sllist)
  {
    typedef typename Dune::SLList<T,A>::const_iterator Iterator;
    Iterator end = sllist.end();
    Iterator current= sllist.begin();

    os << "{ ";
    
    if(current!=end){
      os<<*current<<" ("<<static_cast<const void*>(&(*current))<<")";
      ++current;
      
      for(; current != end; ++current)
	os<<", "<<*current<<" ("<<static_cast<const void*>(&(*current))<<")";
    }
    os<<"} ";
    return os;
  }
}//namespace std

namespace Dune
{
  
  template<typename T, class A>
  SLList<T,A>::Element::Element(const T& item)
    : next_(0), item_(item)
  {}

  template<typename T, class A>
  SLList<T,A>::Element::Element()
    : next_(0), item_()
  {}
  
  template<typename T, class A>
  SLList<T,A>::SLList()
    : beforeHead_(), tail_(&beforeHead_), allocator_(), size_(0)
  {
    beforeHead_.next_=0;
    assert(&beforeHead_==tail_);
    assert(tail_->next_==0);
  }

  template<typename T, class A>
  SLList<T,A>::~SLList()
  {
    clear();
    std::cout<<"Destructor called! Pool="<<allocator_<<std::endl;
  }
  
  template<typename T, class A>
  inline void SLList<T,A>::push_back(const T& item)
  {
      tail_->next_ = allocator_.allocate(1, 0);
      tail_ = tail_->next_;
      ::new (static_cast<void*>(&(tail_->item_))) T(item);
      tail_->next_=0;
    assert(tail_->next_==0);
    ++size_;
  }

  template<typename T, class A>
  inline void SLList<T,A>::insertAfter(Element* current, const T& item)
  {
    assert(current);

    bool changeTail = (current == tail_);
  
    // Save old next element
    Element* tmp = current->next_;

    assert(!changeTail || !tmp);

    // Allocate space
    current->next_ = allocator_.allocate(1, 0);

    // Use copy constructor to initialize memory
    ::new(static_cast<void*>(&(current->next_->item_))) T(item);

    // Set next element
    current->next_->next_=tmp;
    
    if(!current->next_->next_){
      // Update tail
      assert(changeTail);
      tail_ = current->next_;
    }
    ++size_;
    assert(!tail_->next_);
  }
  
  template<typename T, class A>
  inline void SLList<T,A>::push_front(const T& item)
  {
    if(tail_ == &beforeHead_){
      // list was empty
      beforeHead_.next_ = tail_ = allocator_.allocate(1, 0);
      ::new(static_cast<void*>(&beforeHead_.next_->item_)) T(item);
      beforeHead_.next_->next_=0;
    }else{
      Element* added = allocator_.allocate(1, 0);
      ::new(static_cast<void*>(&added->item_)) T(item);
      added->next_=beforeHead_.next_;
      beforeHead_.next_=added;
    }
    assert(tail_->next_==0);
    ++size_;
  }

  
  template<typename T, class A>
  inline void SLList<T,A>::deleteNext(Element* current)
  {
    this->template deleteNext<true>(current);
  }
  
  template<typename T, class A>
  template<bool watchForTail>
  inline void SLList<T,A>::deleteNext(Element* current)
  {
    assert(current->next_);
    Element* next = current->next_;
    
    if(!watchForTail || next == tail_){ 
      // deleting last element changes tail!
      tail_ = current;
    }
    
    current->next_ = next->next_;
    next->item_.~T();
    next->next_ = 0;
    allocator_.deallocate(next, 1);
    --size_;
    assert(!watchForTail || &beforeHead_ != tail_ || size_==0);
  }
  
  template<typename T, class A>
  inline void SLList<T,A>::pop_front()
  {
    deleteNext(&beforeHead_);
  }
  
  template<typename T, class A>
  inline void SLList<T,A>::clear()
  {    
    while(beforeHead_.next_ ){
      this->template deleteNext<false>(&beforeHead_);
    }

#ifdef NDEBUG
    size_=0;
#endif

    assert(size_==0);
    // update the tail!
    tail_ = &beforeHead_;
  }

  template<typename T, class A>
  inline bool SLList<T,A>::empty() const
  {
    return  (&beforeHead_ == tail_);
  }

  template<typename T, class A>
  inline int SLList<T,A>::size() const
  {
    return size_;
  }
  
  template<typename T, class A>
  inline SLListIterator<T,A> SLList<T,A>::begin()
  {
    return iterator(beforeHead_.next_, this);
  }
  
  template<typename T, class A>
  inline SLListConstIterator<T,A> SLList<T,A>::begin() const
  {
    return const_iterator(beforeHead_.next_);
  } 
  
  template<typename T, class A>
  inline SLListIterator<T,A> SLList<T,A>::oneBeforeBegin()
  {
    return iterator(&beforeHead_, this);
  }
  
  template<typename T, class A>
  inline SLListConstIterator<T,A> SLList<T,A>::oneBeforeBegin() const
  {
    return const_iterator(&beforeHead_);
  }

  template<typename T, class A>
  inline SLListIterator<T,A> SLList<T,A>::tail()
  {
    return iterator(tail_, this);
  }
  
  template<typename T, class A>
  inline SLListConstIterator<T,A> SLList<T,A>::tail() const
  {
    return const_iterator(tail_);
  }

  template<typename T, class A>
  inline SLListIterator<T,A> SLList<T,A>::end()
  {
    return iterator();
  }

  template<typename T, class A>
  inline SLListModifyIterator<T,A> SLList<T,A>::endModify()
  {
    return SLListModifyIterator<T,A>(iterator(tail_, this),iterator());
  }


  template<typename T, class A>
  inline SLListModifyIterator<T,A> SLList<T,A>::beginModify()
  {
    return SLListModifyIterator<T,A>(iterator(&beforeHead_, this), 
				     iterator(beforeHead_.next_, this));
  }

  template<typename T, class A>
  inline SLListConstIterator<T,A> SLList<T,A>::end() const
  {
    return const_iterator();
  }
}
  /** }@ */
#endif
