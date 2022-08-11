#include <stdio.h>
#include <stdlib.h>  /* malloc() */

 
/* Multiply (block)submatrices */
void blkmul(double *ablk, double *bblk, double *cblk, int n, int bs)
{
  int i, j, k;    /* Guess what... again... */

  for (i = 0; i < bs; i++)
  {
    for (j = 0; j < bs; j++)
    {
      for  (k = 0; k < bs; k++)
      {
        cblk[i*n + j] += ablk[i*n + k] * bblk[j*n + k];
      }
    }
  }
}
    
/*****************************************************************/

/* Multiply square matrices, blocked version */
void matmulblks(double *A, double *B, double *C, int n, int bs, int inicio, int fin)
{
  int i, j, k;    /* Guess what... */
  double * a = A;
  double * b = B;
  double * c = C;

  for (i = inicio; i < fin; i += bs)
  {
    for (j = 0; j < n; j += bs)
    {
      for  (k = 0; k < n; k += bs)
      {
        blkmul(&a[i*n + k], &b[j*n + k], &c[i*n + j], n, bs);
      }
    }
  }
}

/*****************************************************************/


