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



#include <stdio.h>
#include <iostream>

#include <dax/cont/ArrayHandle.h>
#include <dax/cont/Scheduler.h>
#include <dax/cont/Timer.h>
#include <dax/cont/UniformGrid.h>
#include <dax/worklet/Magnitude.h>

#include <vector>

#include <piston/piston_math.h>
#include <piston/choose_container.h>
#include <piston/image3d.h>
#include <piston/vtk_image3d.h>
#include <piston/threshold_geometry.h>

namespace
{
struct piston_scalar_image3d : piston::image3d< >
{
  typedef ::thrust::device_vector<dax::Scalar> PointDataContainer;
  PointDataContainer point_data_vector;
  typedef PointDataContainer::iterator PointDataIterator;

  piston_scalar_image3d(dax::Id xsize, dax::Id ysize, dax::Id zsize,
                        const std::vector<dax::Scalar> &data)
    : piston::image3d< >(xsize, ysize, zsize),
      point_data_vector(data)
  {
    assert(this->NPoints == this->point_data_vector.size());
  }

  DAX_EXEC_CONT_EXPORT
  PointDataIterator point_data_begin() {
    return this->point_data_vector.begin();
  }

  DAX_EXEC_CONT_EXPORT
  PointDataIterator point_data_end() {
    return this->point_data_vector.end();
  }
};

void PrintResults(int pipeline, double time, const char* name)
{
  std::cout << "Elapsed time: " << time << " seconds." << std::endl;
  std::cout << "CSV, " << name <<" ,"
            << pipeline << "," << time << std::endl;
}

void RunPISTONPipeline(const dax::cont::UniformGrid<> &dgrid)
{
  std::cout << "Running pipeline 1: Elevation -> Threshold" << std::endl;

  std::vector<dax::Scalar> elev(dgrid.GetNumberOfPoints());
  dax::cont::ArrayHandle<dax::Scalar> elevHandle = dax::cont::make_ArrayHandle(elev);

  //use dax to compute the magnitude
  dax::cont::Scheduler<> scheduler;
  scheduler.Invoke(dax::worklet::Magnitude(),
            dgrid.GetPointCoordinates(),
            elevHandle);

  //return results to host
  elevHandle.CopyInto(elev.begin());

  //now that the results are back on the cpu, do threshold with piston
  dax::Id3 dims = dax::extentCellDimensions(grid.GetExtent());
  piston_scalar_image3d image(dims[0], dims[1], dims[2], elev);


  typedef piston::threshold_geometry<piston_scalar_image3d> TG;
  TG threshold(image,0,100);

  dax::cont::Timer<> timer;
  threshold();
  double time = timer.GetElapsedTime();

  std::cout << "original GetNumberOfCells: " << dgrid.GetNumberOfCells() << std::endl;
  std::cout << "threshold GetNumberOfCells: " << threshold.valid_cell_indices.size() << std::endl;
  PrintResults(1, time, "Piston");
}




} // Anonymous namespace

