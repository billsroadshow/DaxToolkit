/*=========================================================================

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef dax_cont_internal_ArrayContainer_h
#define dax_cont_internal_ArrayContainer_h

#include "Object.h"
#include <boost/shared_ptr.hpp>

namespace dax { namespace cont { namespace internal {

// Holds a shared_pointer for the array in Control and Excution enviornment
// It currently only manages ownership and locality it doesn't
// try to keep the contents identical between the arrays.
template<typename Type>
class ArrayContainer
{
public:
  typedef Type ValueType;

  ArrayContainer();

  ArrayContainer(const ArrayContainer&);
  ArrayContainer(ArrayContainer&);
  void operator=(const ArrayContainer&);

  bool hasControlArray() const;
  bool hasExecutionArray() const;

  template<typename T>
  void setArrayControl(boost::shared_ptr<T> array);

  template<typename T>
  void setArrayExecution(boost::shared_ptr<T> array);

  template<typename T>
  boost::shared_ptr<T> arrayControl() const;

  template<typename T>
  boost::shared_ptr<T> arrayExecution( ) const;

private:
  //see the documentation of shared pointer that it can be
  //used to hold arbitrary datra
  //the issue is that is type erasure so assignment is a real big pain
  mutable boost::shared_ptr<void> ControlArray;
  mutable boost::shared_ptr<void> ExecutionArray;

};

//------------------------------------------------------------------------------
template<typename Type>
ArrayContainer<Type>::ArrayContainer():
  ControlArray(),ExecutionArray()
{

}

//------------------------------------------------------------------------------
template<typename Type>
ArrayContainer<Type>::ArrayContainer(const ArrayContainer& copy_from_me):
    ControlArray(copy_from_me.ControlArray),
    ExecutionArray(copy_from_me.ExecutionArray)
{

}

//------------------------------------------------------------------------------
template<typename Type>
ArrayContainer<Type>::ArrayContainer(ArrayContainer& copy_from_me):
  ControlArray(copy_from_me.ControlArray),
  ExecutionArray(copy_from_me.ExecutionArray)
{

}


//------------------------------------------------------------------------------
template<typename Type>
void ArrayContainer<Type>::operator=(const ArrayContainer& copy_from_me)
{
  this->ControlArray = copy_from_me.ControlArray;
  this->ExecutionArray = copy_from_me.ExecutionArray;
}

//------------------------------------------------------------------------------
template<typename Type>
inline bool ArrayContainer<Type>::hasControlArray() const
{
  return (this->ControlArray != NULL);
}

//------------------------------------------------------------------------------
template<typename Type>
inline bool ArrayContainer<Type>::hasExecutionArray() const
{
return (this->ExecutionArray != NULL);
}

//------------------------------------------------------------------------------
template<typename Type> template<typename T>
inline void ArrayContainer<Type>::setArrayControl(boost::shared_ptr<T> array)
{
  this->ControlArray = array;
}

//------------------------------------------------------------------------------
template<typename Type> template<typename T>
inline void ArrayContainer<Type>::setArrayExecution(boost::shared_ptr<T> array)
{
  this->ExecutionArray = array;
}

//------------------------------------------------------------------------------
template<typename Type> template<typename T>
boost::shared_ptr<T> ArrayContainer<Type>::arrayControl() const
{
  if(this->ControlArray)
    {
    return boost::static_pointer_cast<T>(this->ControlArray);
    }

  boost::shared_ptr<T> controlArray(new T());
  if(this->ExecutionArray)
    {
    //copy the array from control to execution
    //this is a trick to get around the type erasure on control array
    controlArray = controlArray->convert(this->ExecutionArray);
    }
  this->ControlArray = controlArray;
  return boost::static_pointer_cast<T>(this->ControlArray);
}

//------------------------------------------------------------------------------
template<typename Type> template<typename T>
boost::shared_ptr<T> ArrayContainer<Type>::arrayExecution( ) const
{
  if(this->ExecutionArray)
    {
    return boost::static_pointer_cast<T>(this->ExecutionArray);
    }
  boost::shared_ptr<T> executionArray(new T());
  if(this->ControlArray)
    {
    //copy the array from control to execution
    //this is a trick to get around the type erasure on control array
    executionArray = executionArray->convert(this->ControlArray);
    }
  this->ExecutionArray = executionArray;
  return boost::static_pointer_cast<T>(this->ExecutionArray);
}

} } }

#endif dax_cont_internal_ArrayContainer_h
