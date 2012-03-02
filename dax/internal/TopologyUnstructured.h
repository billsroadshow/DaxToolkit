#ifndef __dax__internal__Unstructured_h
#define __dax__internal__Unstructured_h

#include <dax/Types.h>
#include <dax/internal/GridTopologys.h>
#include <dax/internal/DataArray.h>

namespace dax {
namespace internal {


template< typename T>
struct TopologyUnstructured
{
  typedef T CellType;

  TopologyUnstructured()
    {
    }

  TopologyUnstructured(dax::internal::DataArray<dax::Vector3>&points,
                   dax::internal::DataArray<dax::Id>& topology):    
    Points(points),
    Topology(topology)
    {
    }

  dax::internal::DataArray<dax::Vector3> Points;
  dax::internal::DataArray<dax::Id> Topology;
};

/// Returns the number of cells in a unstructured grid.
template<typename T>
DAX_EXEC_CONT_EXPORT
dax::Id numberOfCells(const TopologyUnstructured<T> &GridTopology)
{
  typedef typename TopologyUnstructured<T>::CellType CellType;
  return GridTopology.Topology.GetNumberOfEntries()/CellType::NUM_POINTS;
}

/// Returns the number of points in a unstructured grid.
template<typename T>
DAX_EXEC_CONT_EXPORT
dax::Id numberOfPoints(const TopologyUnstructured<T> &GridTopology)
{
  return GridTopology.Points.GetNumberOfEntries();
}

/// Returns the point position in a structured grid for a given index
/// which is represented by /c pointIndex
template<typename T>
DAX_EXEC_CONT_EXPORT
dax::Vector3 pointCoordiantes(const TopologyUnstructured<T> &grid,
                              dax::Id pointIndex)
{
  return grid.Points.GetValue(pointIndex);
}


} //internal
} //dax

#endif // __dax__internal__UnstructuredGrid_h