/**
 * \file cowichan_openmp/sor.cpp
 * \brief OpenMP sor implementation.
 * \see CowichanOpenMP::sor
 */

#include "cowichan_openmp.hpp"

void CowichanOpenMP::sor (Matrix matrix, Vector target, Vector solution)
{
  index_t r, c;
  index_t t;
  real sum;
  real oldSolution;
  real diff, maxDiff;

  // initialize
  for (r = 0; r < n; r++) {
    solution[r] = 1.0;
  }
  maxDiff = (real)(2 * SOR_TOLERANCE); // to forestall early exit

  for (t = 0; (t < SOR_MAX_ITERS) && (maxDiff >= SOR_TOLERANCE); t++) {

    maxDiff = 0.0;

#pragma omp parallel private(oldSolution, diff, sum, c) firstprivate(maxDiff)
    {
#pragma omp for schedule(static)
      for (r = 0; r < n; r++) {
		__transaction_atomic {
			// compute sum
			sum = 0.0;
			for (c = 0; c < r; c++) {
			  sum += MATRIX_SQUARE(matrix, r, c) * solution[c];
			}
			for (c = r + 1; c < n; c++) {
			  sum += MATRIX_SQUARE(matrix, r, c) * solution[c];
			}

			// calculate new solution
			oldSolution = solution[r];
			solution[r] = (real)((1.0 - SOR_OMEGA) * oldSolution + SOR_OMEGA *
				(target[r] - sum) / MATRIX_SQUARE(matrix, r, r));

			// compute difference
			diff = (real)fabs((double)(oldSolution - solution[r]));
			if (diff > maxDiff){
			  maxDiff = diff;
			}
		  }

		}

	  }
  }

}

