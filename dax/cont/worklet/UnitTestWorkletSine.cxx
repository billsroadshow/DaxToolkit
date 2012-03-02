/*=========================================================================

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <dax/cont/DeviceAdapterDebug.h>
#include <dax/cont/internal/DeviceAdapterError.h>

#include <dax/cont/worklet/Sine.h>

#include <dax/VectorTraits.h>
#include <dax/cont/ArrayHandle.h>
#include <dax/cont/UniformGrid.h>

#include <dax/cont/internal/Testing.h>

namespace {

const dax::Id DIM = 64;

//-----------------------------------------------------------------------------
static void TestSine()
{
  dax::cont::UniformGrid grid;
  grid.SetExtent(dax::make_Id3(0, 0, 0), dax::make_Id3(DIM-1, DIM-1, DIM-1));

  dax::Vector3 trueGradient = dax::make_Vector3(1.0, 1.0, 1.0);

  std::vector<dax::Scalar> field(grid.GetNumberOfPoints());
  for (dax::Id pointIndex = 0;
       pointIndex < grid.GetNumberOfPoints();
       pointIndex++)
    {
    field[pointIndex]
        = dax::dot(grid.GetPointCoordinates(pointIndex), trueGradient);
    }
  dax::cont::ArrayHandle<dax::Scalar, dax::cont::DeviceAdapterDebug>
      fieldHandle(field.begin(), field.end());

  std::vector<dax::Scalar> sine(grid.GetNumberOfPoints());
  dax::cont::ArrayHandle<dax::Scalar, dax::cont::DeviceAdapterDebug>
      sineHandle(sine.begin(), sine.end());

  std::cout << "Running Sine worklet" << std::endl;
  dax::cont::worklet::Sine(grid, fieldHandle, sineHandle);

  std::cout << "Checking result" << std::endl;
  for (dax::Id pointIndex = 0;
       pointIndex < grid.GetNumberOfPoints();
       pointIndex++)
    {
    dax::Scalar sineValue = sine[pointIndex];
    dax::Scalar sineTrue = sinf(field[pointIndex]);
    DAX_TEST_ASSERT(test_equal(sineValue, sineTrue),
                    "Got bad sine");
    }
}

} // Anonymous namespace

//-----------------------------------------------------------------------------
int UnitTestWorkletSine(int, char *[])
{
  return dax::cont::internal::Testing::Run(TestSine);
}