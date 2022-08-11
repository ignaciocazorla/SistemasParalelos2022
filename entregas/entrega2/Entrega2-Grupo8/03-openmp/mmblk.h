#ifndef MMBLK_H_
#define MMBLK_H_

/* Multiply (block)submatrices */
void blkmul(double *ablk, double *bblk, double *cblk, int n, int bs);

/* Multiply square matrices, blocked version */
void matmulblks(double *a, double *b, double *c, int n, int bs);

#endif