/*=========================================================================

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __dax_cont_internal_ArrayContainerExecutionCPU_h
#define __dax_cont_internal_ArrayContainerExecutionCPU_h

#include <dax/Types.h>

#include <dax/internal/DataArray.h>

#include <dax/cont/Assert.h>
#include <dax/cont/ErrorControlOutOfMemory.h>
#include <dax/cont/internal/IteratorContainer.h>

#include <vector>

namespace dax {
namespace cont {
namespace internal {

/// Manages an execution environment array. In the case of this class, the
/// execution uses the same memory heap as the control.
///
template<typename T>
class ArrayContainerExecutionCPU
{
public:
  typedef T ValueType;

  /// On inital creation, no memory is allocated on the device.
  ///
  ArrayContainerExecutionCPU() { }

  /// Allocates an array on the device large enough to hold the given number of
  /// entries.
  ///
  void Allocate(dax::Id numEntries) {
    try
      {
      this->DeviceArray.resize(numEntries);
      }
    catch (...)
      {
      throw dax::cont::ErrorControlOutOfMemory(
          "Failed to allocate execution array on CPU.");
      }
  }

  /// Copies the data pointed to by the passed in \c iterators (assumed to be
  /// in the control environment), into the array in the execution environment
  /// managed by this class.
  ///
  template<class IteratorType>
  void CopyFromControlToExecution(
      const dax::cont::internal::IteratorContainer<IteratorType> &);

  /// Copies the data from the array in the execution environment managed by
  /// this class into the memory passed in the \c iterators (assumed to be in
  /// the control environment).
  ///
  template<class IteratorType>
  void CopyFromExecutionToControl(
      const dax::cont::internal::IteratorContainer<IteratorType> &);

  /// Frees any resources (i.e. memory) on the device.
  ///
  void ReleaseResources() { this->Allocate(0); }

  /// Gets a DataArray that is valid in the execution environment.
  dax::internal::DataArray<ValueType> GetExecutionArray();

private:
  ArrayContainerExecutionCPU(const ArrayContainerExecutionCPU &); // Not implemented
  void operator=(const ArrayContainerExecutionCPU &);        // Not implemented

  std::vector<ValueType> DeviceArray;
};

//-----------------------------------------------------------------------------
template<class T>
template<class IteratorType>
inline void ArrayContainerExecutionCPU<T>::CopyFromControlToExecution(
    const dax::cont::internal::IteratorContainer<IteratorType> &iterators)
{
  DAX_ASSERT_CONT(iterators.IsValid());
  DAX_ASSERT_CONT(iterators.GetNumberOfEntries()
                  == static_cast<dax::Id>(this->DeviceArray.size()));
  std::copy(iterators.GetBeginIterator(),
            iterators.GetEndIterator(),
            this->DeviceArray.begin());
}

//-----------------------------------------------------------------------------
template<class T>
template<class IteratorType>
inline void ArrayContainerExecutionCPU<T>::CopyFromExecutionToControl(
    const dax::cont::internal::IteratorContainer<IteratorType> &iterators)
{
  DAX_ASSERT_CONT(iterators.IsValid());
  DAX_ASSERT_CONT(iterators.GetNumberOfEntries()
                  == static_cast<dax::Id>(this->DeviceArray.size()));
  std::copy(this->DeviceArray.begin(),
            this->DeviceArray.end(),
            iterators.GetBeginIterator());
}

//-----------------------------------------------------------------------------
template<class T>
inline dax::internal::DataArray<T>
ArrayContainerExecutionCPU<T>::GetExecutionArray()
{
  ValueType *rawPointer = &this->DeviceArray[0];
  dax::Id numEntries = this->DeviceArray.size();
  return dax::internal::DataArray<ValueType>(rawPointer, numEntries);
}

}
}
}

#endif //__dax_cont_internal_ArrayContainerExecutionCPU_h