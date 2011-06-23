/*=========================================================================

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// Functor that computes cell-scalars based on point-scalars.
void CellAverage(const daxWork* work,
  const daxArray* __positions__ in_positions,
  const daxArray* __and__(__connections__, __ref__(in_positions)) in_connections,
  const daxArray* __dep__(in_positions) inputArray,
  daxArray* __dep__(in_connections) outputArray)
{
  // Get the connected-components using the connections array.
  daxConnectedComponent cell;
  daxGetConnectedComponent(work, in_connections, &cell);

  daxFloat sum_value = 0.0;
  daxIdType num_elements = daxGetNumberOfElements(&cell);
  daxWork point_work;
  for (daxIdType cc=0; cc < num_elements; cc++)
    {
    // Generate a "work" for the point of interest.
    daxGetWorkForElement(&cell, cc, &point_work);
    sum_value += daxGetArrayValue(&point_work, inputArray);
    //printf("input value %d : %f\n",cc, val);
    //sum_value += val;
    }
  sum_value /= num_elements;
  daxSetArrayValue(work, outputArray, sum_value);
  printf("Cell Average : %f\n", sum_value);
}