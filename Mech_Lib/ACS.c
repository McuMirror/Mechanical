/**
  ******************************************************************************
  * @file           : AttitudeEstimation.h
  * @brief          : Header file for the Attitude Control System (ACS).
  ******************************************************************************
  ** 
	* 
	* Created by Galen Savidge. Edited 4/21/2019.
  ******************************************************************************
  */


#include <ACS.h>
#include <AttitudeEstimation.h>
#include <math.h>


// Reaction wheel properties
#define RW_D 2700
#define RW_R 0.01265
#define RW_H 0.005

// Body inertia matrices
#define JB_11 0.60579
#define JB_22 0.01330
#define JB_33 0.59753


// Helper functions
Matrix makeJrw(void) {
	float m = PI*RW_R*RW_R*RW_H*RW_D; // Mass of the flywheel (kg)
	float x = 0.5*m*RW_R*RW_R;
	
	Matrix J = newMatrix(3, 3);
	matrixSet(J, 1, 1, x);
	matrixSet(J, 2, 2, x);
	matrixSet(J, 3, 3, x);
	return J;
}

Matrix makeJrwInv(void) {
	float m = PI*RW_R*RW_R*RW_H*RW_D; // Mass of the flywheel
	float x = 2.0/(m*RW_R*RW_R);
	
	Matrix J = newMatrix(3, 3);
	matrixSet(J, 1, 1, x);
	matrixSet(J, 2, 2, x);
	matrixSet(J, 3, 3, x);
	return J;
}

Matrix makeJbody(void) {
	Matrix J = newMatrix(3, 3);
	matrixSet(J, 1, 1, JB_11);
	matrixSet(J, 2, 2, JB_22);
	matrixSet(J, 3, 3, JB_33);
	return J;
}

Matrix makeJbodyInv(void) {
	Matrix J = newMatrix(3, 3);
	matrixSet(J, 1, 1, 1.0/JB_11);
	matrixSet(J, 2, 2, 1.0/JB_22);
	matrixSet(J, 3, 3, 1.0/JB_33);
	return J;
}


// Public functions
void initializeACS(ACSType* acs) {
	// Allocate Matrix structs
	acs->R = initializeDCM(0, 0, 0);
	acs->Rt = newMatrix(3, 3);
	matrixTranspose(acs->R, acs->Rt);
	
	acs->gyro_vector = newMatrix(3, 1);
	acs->mag_vector = newMatrix(3, 1);
	acs->solar_vector = newMatrix(3, 1);
	
	acs->gyro_bias = newMatrix(3, 1);
	
	acs->sv_inertial = newMatrix(3, 1);
	acs->mag_inertial = newMatrix(3, 1);
	acs->craft_inertial = newMatrix(3, 1);
	
	acs->z_err = newMatrix(3, 1);
	acs->n_err = newMatrix(3, 1);
	acs->err = newMatrix(3, 1);
	
	acs->w_rw = make3x1Vector(0, 0, 0);
	acs->J_rw = makeJrw();
	acs->J_rw_inv = makeJrwInv();
	acs->A_rw = initializeDCM(0, 0, 0); // I3 (identity matrix)
	acs->J_body = makeJbody();
	acs->J_body_inv = makeJbodyInv();
	
	acs->rw_PWM = newMatrix(3, 1);
	acs->tr_PWM = newMatrix(3, 1);
}

void initializeACSSerial(ACSType* acs, UART_HandleTypeDef* huart) {
	acs->huart = huart;
}