/**
 * \file cowichan_openmp/outer.cpp
 * \brief OpenMP outer implementation.
 * \see CowichanOpenMP::outer
 */

#include "cowichan_openmp.hpp"

void CowichanOpenMP::outer (PointVector points, Matrix matrix, Vector vector){
  Point zeroPoint(0.0, 0.0);
  real d; // distance between points
  real dMax; // maximum distance
  index_t r, c; // loop indices

  // all elements except matrix diagonal
#pragma omp parallel private(dMax, d)
  {
#pragma omp for schedule(static)
    for (r = 0; r < n; r++) {
		__transaction_relaxed {
      vector[r] = Point::distance (points[r], zeroPoint);
#pragma omp parallel for schedule(static)
      for (c = 0; c < r; c++) {
        d = Point::distance (points[r], points[c]);
        if (d > dMax) {
          dMax = d;
        }
        MATRIX_SQUARE(matrix, r, c) = MATRIX_SQUARE(matrix, c, r) = d;
      }
    }
	}
  }
  // matrix diagonal
  dMax *= n;
#pragma omp parallel for schedule(static)
  for (r = 0; r < n; r++) {
    MATRIX_SQUARE(matrix, r, r) = dMax;
  }
}

