/**
  ******************************************************************************
  * @file           : Matrix.h
  * @brief          : Header for the Matrix module.
  ******************************************************************************
  ** This module contains matrix and vector math functions. Matrices contain r
	* by c arrays of doubles.
	*
	* Created by Galen Savidge. Edited 2/23/2019.
  ******************************************************************************
  */

#ifndef MATRIX_H
#define	MATRIX_H

/* Public types ------------------------------------------------------------*/
typedef struct _Matrix* Matrix;

/* Public functions prototypes ---------------------------------------------*/

/** 
 * @brief  Allocates a new Matrix
 * @param  Number of rows and columns
 * @return Pointer to the new Matrix
*/
Matrix newMatrix(int r, int c);

/** 
 * @brief  Copies a Matrix to another allocated Matrix
 * @param  Matrix to be copied, Matrix to hold the copy
 * @return None
*/
Matrix matrixCopy(Matrix m, Matrix copy);

/** 
 * @brief  Returns the number of rows in a Matrix
 * @param  A Matrix
 * @return Integer number of rows
*/
int matrixGetRows(Matrix m);

/** 
 * @brief  Returns the number of columns in a Matrix
 * @param  A Matrix
 * @return Integer number of columns
*/
int matrixGetCol(Matrix m);

/** 
 * @brief  Returns an element in a Matrix
 * @param  A Matrix, row of the element, column of the element (indexed starting at 1)
 * @return Element (r, c) of the Matrix
*/
double matrixGetElement(Matrix m, int r, int c);

/** 
 * @brief  Sets an element in a Matrix
 * @param  A Matrix, row of the element, column of the element (indexed starting at 1), double value for the element
 * @return None
*/
void matrixSet(Matrix m, int r, int c, double val);

/** 
 * @brief  Multiplies every element of a Matrix by x
 * @param  A Matrix and a double x
 * @return None
*/
void matrixScale(Matrix m, double x);

/** 
 * @brief  Adds x to every element in a Matrix
 * @param  A Matrix and a double x
 * @return None
*/
void matrixAddScalar(Matrix m, double x);

/** 
 * @brief  Adds two Matrix objects
 * @param  Two Matrix objects to be added, a third allocated Matrix in which to store the result
 * @return None
*/
void matrixAdd(Matrix m1, Matrix m2, Matrix sum);

/** 
 * @brief  Subtracts two Matrix objects
 * @param  Two Matrix objects to be subtracted, a third allocated Matrix in which to store the result
 * @return None
*/
void matrixSubtract(Matrix m1, Matrix m2, Matrix diff);

/** 
 * @brief  Multiplies two Matrix objects
 * @param  Two Matrix objects to be multiplied, a third allocated Matrix in which to store the result
 * @return None
*/
void matrixMult(Matrix m1, Matrix m2, Matrix prod);

/** 
 * @brief  Copies and transposes a Matrix
 * @param  A Matrix, an allocated Matrix in which to store the transpose
 * @return None
*/
void matrixTranspose(Matrix m, Matrix mt);

/** 
 * @brief  Creates the skew symmetric (cross product) matrix from a vector
 * @param  A 3x1 column vector Matrix, an allocated 3x3 Matrix in which to store the result
 * @return None
*/
void vectorRcross(Matrix v, Matrix vcross);

/** 
 * @brief  Calculates the norm of a vector
 * @param  A column vector Matrix
 * @return The norm of v
*/
float vectorNorm(Matrix v);

/** 
 * @brief  Prints a Matrix to a string in a format suitable for printing over serial
 * @param  A Matrix and a char pointer with enough room to hold the printed Matrix
 * @return None
*/
void printMatrix(Matrix m, char* string);

/** 
 * @brief  Frees a Matrix from memory
 * @param  A Matrix
 * @return None
 */
void freeMatrix(Matrix *mp);

#endif /* MATRIX_H */
