// $Id$
#ifndef __DUNE_POOLALLOCATOR_HH__
#define __DUNE_POOLALLOCATOR_HH__

#include<dune/common/alignment.hh>
#include<iostream>

namespace Dune
{
  
  /**
   * @file 
   * This file implements the class ArrayList which behaves like
   * dynamically growing array together with 
   * the class ArrayListIterator which is random access iterator as needed
   * by the stl for sorting and other algorithms.
   * @author Markus Blatt
   */
    /** 
     * @addtogroup Common
     *
     * @{
     */
  /**
   * @brief A memory pool of objects.
   * 
   * The memory for the objects is organized in chunks.
   * Each chunks is capable of holding a specified number of 
   * objects. The allocated objects will be properly aligned
   * for fast access.
   * Deallocated objects are cached for reuse to prevent memory
   * fragmentation.
   */
  template<class T, int s>
  class Pool
  {
    public:
    /** @brief The type of object we allocate memory for. */
    typedef T MemberType;
    enum 
      {
	/**
	 * @brief size requirement. At least one object has to
	 * stored.
	 */
	size = (sizeof(MemberType)>s)? sizeof(MemberType): s,
	
	/**
	 * @brief The aligned size of the type.
	 *
	 * This size is bigger than sizeof of the type and multiple of
	 * the algnment requirement.
	 */
	alignedSize = (sizeof(MemberType) % AlignmentOf<MemberType>::value == 0) ?
	sizeof(MemberType) : 
	(sizeof(MemberType) / AlignmentOf<MemberType>::value + 1) 
	* AlignmentOf<MemberType>::value,

	/** 
	 * @brief The size of each chunk memory chunk. 
	 *
	 * Will be adapted to be a multiple of the alignment plus
	 * an offset to handle the case that the pointer to the memory
	 * does not satisfy the alignment requirements.
	 */
	chunkSize = ((size % AlignmentOf<MemberType>::value == 0)? size
	: (size / AlignmentOf<MemberType>::value + 1) 
	* AlignmentOf<MemberType>::value) + AlignmentOf<MemberType>::value - 1,
	/**
	 * @brief The number of element each chunk can hold.
	 */
	elements = (chunkSize / alignedSize)
      };
  private:
    struct Reference
    {
      Reference *next_;
    };
    
    struct Chunk
    {
      /** @brief The memory we hold. */
      char chunk_[chunkSize];

      /** 
       * @brief Adress the first properly aligned
       * position in the chunk.
       */
      unsigned long memory_;

      /** @brief The next element */
      Chunk *next_;
      
      /** 
       * @brief Constructor.
       */
      Chunk()
      {
	memory_ = reinterpret_cast<unsigned long>(chunk_);
	if(memory_ % AlignmentOf<MemberType>::value != 0)
	  memory_ = (memory_ / AlignmentOf<MemberType>::value + 1)
	    /AlignmentOf<MemberType>::value;
      }
    };
  
  public:
    /** @brief The type of object we allocate memory for. */
    typedef T MemberType;
    
    /** @brief Constructor. */
    inline Pool();
    /** @brief Destructor. */
    inline ~Pool();
    /** 
     * @brief Get a new or recycled object 
     * @return A pointer to the object memory.
    */
    inline void *allocate();
    /** 
     * @brief Free an object.
     * @param o The pointer to memory block of the object.
     */
    inline void free(void* o);

  private:
  
    // Prevent Copying!
    Pool(const Pool<MemberType,s>&);
    void operator=(const Pool<MemberType,s>& pool) const;
    /** @brief Grow our pool.*/
    inline inline void grow();
    /** @brief The first free element. */
    Reference *head_;
    /** @brief Our memory chunks. */
    Chunk *chunks_;
  };

  /**
   * @brief An allocator managing a pool of objects for reuse.
   *
   * This allocator is specifically useful for small data types
   * where new and delete are too expensive. 
   *
   * @warning It is not suitable
   * for the use in standard containers as it cannot allocate
   * arrays of arbituary size
   */
  template<class T, int s>
  class PoolAllocator
  {
  public:
    /**
     * @brief Type of the values we construct and allocate.
     */
    typedef T value_type;

    enum
      {
	/**
	 * @brief The size in bytes to use for every memory chunk
	 * allocated.
	 */
	size=s
      };

    /**
     * @brief The pointer type.
     */
    typedef T* pointer;

    /**
     * @brief The constant pointer type.
     */
    typedef const T* const_pointer;

    /**
     * @brief The reference type.
     */
    typedef T& reference;

    /**
     * @brief The constant reference type.
     */
    typedef const T& const_reference;

    /**
     * @brief Constructor.
     */
    inline PoolAllocator();

    /**
     * @brief Allocates objects.
     * @param n The number of objects to allocate. Has to be less
     * than Pool<T,s>::elements!
     * @param hint Ignored hint.
     * @return A pointer tp the allocated elements.
     */
    inline pointer allocate(size_t n, const_pointer hint);
    
    /**
     * @brief Free objects.
     *
     * Does not call the contructor!
     * @param n The number of object to free. Has to be one!
     * @parma p Pointer to the first object.
     */
    inline void deallocate(pointer p, std::size_t n);

    /**
     * @brief Construct an object.
     * @param p Pointer to the object.
     * @param value The value to initialize it to.
     */
    inline void construct(pointer p, const_reference value);

    /**
     * @brief Destroy an object without freeing memory.
     * @param p Pointer to the object.
     */
    inline void destroy(pointer p);

    /**
     * @brief Convert a reference to a pointer.
     */
    inline pointer  address(reference x) const { return &x; }

    
    /**
     * @brief Convert a reference to a pointer.
     */
    inline const_pointer address(const_reference x) const { return &x; }

    /**
     * @brief Not correctly implemented, yet!
     */
    inline int max_size(){ return 1;}

    /**
     * @brief Rebind the allocator to another type.
     */
    template<class U>
    struct rebind
    {
      typedef PoolAllocator<U,s> other;
    };

  private:
    /**
     * @brief The underlying memory pool.
     */
    static Pool<T,s> memoryPool_;
  };

  template<class T, int S>
  inline Pool<T,S>::Pool()
  {
    head_ = 0;
    chunks_ = 0;    
    std::cout<<"sizeof="<<sizeof(T)<<" alignment="<<AlignmentOf<T>::value
	     <<" chunkSize="<<chunkSize<<" elements="<<elements<<std::endl;
  }
  
  template<class T, int S>
  inline Pool<T,S>::~Pool()
  {
    // delete the allocated chunks.
    Chunk *current=chunks_;
    while(current)
      {
	Chunk *tmp = current;
	current = current->next_;
	delete tmp;
      }
  }

  template<class T, int S>
  inline void Pool<T,S>::grow()
  {
    Chunk *newChunk = new Chunk;
    newChunk->next_ = chunks_;
    chunks_ = newChunk;
    
    char* start = reinterpret_cast<char *>(chunks_->memory_);
    char* last  = &start[elements-1];

    for(char* element=start; element<last; element+=AlignmentOf<T>::value)
      reinterpret_cast<Reference*>(element)->next_
	= reinterpret_cast<Reference*>(element+AlignmentOf<T>::value);
      
    reinterpret_cast<Reference*>(last)->next_=0;
    
    head_ = reinterpret_cast<Reference*>(start);
  }

  template<class T, int S>
  inline void Pool<T,S>::free(void* b)
  {
    Reference* freed = static_cast<Reference*>(b);
    freed->next_ = head_;
    head_ = freed;
  }

  template<class T, int S>
  inline void* Pool<T,S>::allocate()
  {
    if(head_==0)
      grow();

    Reference* p = head_;
    head_ = p->next_;
    return p;
  }

  template<class T, int s> 
  Pool<T,s> PoolAllocator<T,s>::memoryPool_;

  template<class T, int s> 
  inline PoolAllocator<T,s>::PoolAllocator()
  { }

  template<class T, int s>
  inline T* PoolAllocator<T,s>::allocate(std::size_t n, const T* hint=0)
  {
    std::cout<<n<<" "<<Pool<T,s>::elements<<std::endl;
    assert(n<=(Pool<T,s>::elements));
    return static_cast<T*>(memoryPool_.allocate());
  }

  template<class T, int s>
  inline void PoolAllocator<T,s>::deallocate(T* p, std::size_t n)
  {
    assert(n==1);
    memoryPool_.free(p);
  }
  
  template<class T, int s>
  inline void PoolAllocator<T,s>::construct(T* p, const T& value)
  {
    new (p) T(value);
  }

  template<class T, int s>
  inline void PoolAllocator<T,s>::destroy(T* p)
  {
    p->~T();
  }

  /** @} */
}
#endif
