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
#ifndef __dax__cuda__cont__ChooseCudaDevice_h
#define __dax__cuda__cont__ChooseCudaDevice_h

#include <cuda.h>
#include <algorithm>
#include <vector>

namespace dax{
namespace cuda{
namespace cont {

namespace {
struct compute_info
{
compute_info(cudaDeviceProp prop, int index)
{
this->Index = index;
this->Major = prop.major;

this->MemorySize = prop.totalGlobalMem;
this->Performance = prop.multiProcessorCount *
                    prop.maxThreadsPerMultiProcessor *
                    prop.clockRate;

//9999 is equal to emulation make sure it is a super bad device
if(this->Major >= 9999)
  {
  this->Major = -1;
  this->Performance = -1;
  }
}

//sort from fastest to slowest
bool operator<(const compute_info other) const
{
//if we are both SM2 or greater check performance
//if we both the same SM level check performance
if( (this->Major >= 2 && other.Major >= 2) ||
    (this->Major == other.Major) )
  {
  return betterPerfomance(other);
  }
//prefer the greater SM otherwise
return this->Major > other.Major;
}

bool betterPerfomance(const compute_info other) const
{
  if ( this->Performance == other.Performance)
  {
    if( this->MemorySize == other.MemorySize )
      {
      //prefer first device over second device
      //this will be subjective I bet
      return this->Index < other.Index;
      }
    return this->MemorySize > other.MemorySize;
  }
  return this->Performance > other.Performance;
}

int GetIndex() const { return Index; }

private:
int Index;
int Major;
int MemorySize;
int Performance;
};

}

///Returns the fastest cuda device id that the current system has
///A result of zero means no cuda device has been found
int FindFastestDeviceId()
{
  //get the number of devices and store information
  int numberOfDevices;
  cudaGetDeviceCount(&numberOfDevices);

  std::vector<compute_info> devices;
  for(int i=0; i < numberOfDevices; ++i)
    {
    cudaDeviceProp properties;
    cudaGetDeviceProperties(&properties, i);
    if(properties.computeMode != cudaComputeModeProhibited)
      {
      //only add devices that have compute mode allowed
      devices.push_back( compute_info(properties,i) );
      }
    }

  //sort from fastest to slowest
  std::sort(devices.begin(),devices.end());

  int device=0;
  if(devices.size()> 0)
    {
    device = devices.front().GetIndex();
    }
  return device;
}


void SetCudaDevice(int id)
{
  cudaSetDevice(id);
}


}
}
} //namespace

#endif
