//=============================================================================
//
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//
//  Copyright 2012 Sandia Corporation.
//  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
//  the U.S. Government retains certain rights in this software.
//
//=============================================================================

#ifndef __Threshold_worklet_
#define __Threshold_worklet_

#include <dax/exec/CellField.h>
#include <dax/exec/CellVertices.h>
#include <dax/exec/VectorOperations.h>
#include <dax/exec/WorkletMapCell.h>
#include <dax/exec/WorkletGenerateTopology.h>
#include <dax/VectorTraits.h>

namespace dax {
namespace worklet {

template<typename T>

struct ThresholdFunction {
  const T Min;
  const T Max;
  int valid;

  DAX_EXEC_EXPORT ThresholdFunction(const T& min, const T&max):
    Min(min),Max(max),valid(1)
    {
    }

  DAX_EXEC_EXPORT void operator()(T value)
  {
    valid &= value >= this->Min && value <= this->Max;
  }
};


template<typename ValueType>
class ThresholdClassify : public dax::exec::WorkletMapCell
{
public:
  typedef void ControlSignature(Topology,Field(Point), Field(Out));
  typedef _3 ExecutionSignature(_2);



  DAX_CONT_EXPORT
  ThresholdClassify(ValueType thresholdMin, ValueType thresholdMax)
    : ThresholdMin(thresholdMin), ThresholdMax(thresholdMax) {  }

  template<class CellTag>
  DAX_EXEC_EXPORT
  dax::Id operator()(
      const dax::exec::CellField<ValueType,CellTag> &values) const
  {
    ThresholdFunction<ValueType> threshold(this->ThresholdMin,
                                           this->ThresholdMax);
    dax::exec::VectorForEach(values, threshold);
    return threshold.valid;
  }
private:
  ValueType ThresholdMin;
  ValueType ThresholdMax;
};

class ThresholdTopology : public dax::exec::WorkletGenerateTopology
{
public:
  typedef void ControlSignature(Topology, Topology(Out));
  typedef void ExecutionSignature(Vertices(_1),Vertices(_2));

  template<typename InputCellTag, typename OutputCellTag>
  DAX_EXEC_EXPORT
  void operator()(const dax::exec::CellVertices<InputCellTag> &inVertices,
                  dax::exec::CellVertices<OutputCellTag> &outVertices) const
  {
    outVertices.SetFromTuple(inVertices.GetAsTuple());
  }
};


}
} //dax::worklet

#endif