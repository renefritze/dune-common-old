#ifndef DUNE_OBJPOINTER_HH
#define DUNE_OBJPOINTER_HH

namespace Dune {

//! storage class for newly generated operators during operator + on
//! DiscreteOperator
template <class ObjType>
struct ObjPointer
{
  typedef ObjPointer<ObjType> ObjPointerType;
  //! remember object item and next pointer 
  ObjType * item;
  ObjPointerType *next;

  //! new ObjPointer is only created with pointer for item 
  ObjPointer () : item (0) , next (0) {}

  //! new ObjPointer is only created with pointer for item 
  ObjPointer (ObjType  *t) : item (t) , next (0) {}

  //! delete the next ObjPointer and the item 
  //! need virtual desctructor for deleting the real objects. 
  virtual ~ObjPointer ()
  {
    if(next) delete next; next = 0;
    if(item) delete item; item = 0;
  }
};

//! stores the new created objects when DiscreteOperatorDefault 
//! operator + or operator * is called 
class ObjPointerStorage
{
  typedef ObjPointerStorage MyType;
public:
  //! make new operator with item points to null 
  ObjPointerStorage () : item_ (0) {}

  //! need virtual desctructor for deleting the real objects. 
  virtual ~ObjPointerStorage ()
  {
    //std::cout << "delete " << item_ << "\n";
    if(item_) delete item_; item_ = 0;
  }

  //! Store new generated DiscreteOperator Pointer 
  template <class DiscrOpType>
  void saveObjPointer ( DiscrOpType * discrOp )
  {
    ObjPointerType *next = new ObjPointerType ( discrOp );
    next->next = item_;
    item_ = next;
  }

  //! Store new generated DiscreteOperator Pointer and the LocalOperator
  //! pointer
  template <class DiscrOpType, class LocalOpType >
  void saveObjPointer ( DiscrOpType * discrOp , LocalOpType * lop )
  {
    saveObjPointer( discrOp );
    saveObjPointer( lop     );  
  }

private:
  // store the objects created by operator + in here 
  typedef ObjPointer<MyType> ObjPointerType;
  ObjPointerType * item_;

};

} // end namespace Dune

#endif

