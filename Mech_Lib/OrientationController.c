/**
  ******************************************************************************
  * @file           OrientationController.c
  * @brief          Implementation for the reorientation state of the ACS
  ******************************************************************************
  **
	* 
	* Created by Galen Savidge. Edited 5/12/2019.
  ******************************************************************************
  */

#include "FeedbackControl.h"


// Feedback constants
#define K 1
#define KP (K*0.05)
#define KD (K*0.1)

#define ORIENTATION_W_MAG -0.00872665 // 0.5 deg/s in rad/s


void runOrientationController(ACSType* acs, int first_step) {
	static int init_run = 0;
	static Matrix w_err, last_w_err, P, D, wdot_desired;
	
	if(init_run == 0) {
		w_err = newMatrix(3, 1);
		last_w_err = newMatrix(3, 1);
		P = newMatrix(3, 1);
		D = newMatrix(3, 1);
		wdot_desired = newMatrix(3, 1);
		init_run = 1;
	}
	
	if(acs->dt == 0) {
		return;
	}
	
	// Find error in rotation speed
	matrixCopy(acs->z_err, w_err);
	float z_err_norm = vectorNorm(w_err);
	matrixScale(w_err, ORIENTATION_W_MAG/z_err_norm);
	matrixSubtract(acs->w, w_err, w_err); // w_err = w - (-0.017*z_err/norm(z_err))
	
	if(first_step) {
		vectorSetXYZ(wdot_desired, 0, 0, 0);
	}
	else {
		// Proportional component
		matrixCopy(w_err, P);
		matrixScale(P, KP);
		
		// Derivative component
		matrixCopy(w_err, D);
		matrixSubtract(D, last_w_err, D);
		matrixScale(D, KD/acs->dt); // D = (w_err - last_w_err)*KD/dt
		
		// Sum P and D
		matrixAdd(P, D, wdot_desired);
		matrixScale(wdot_desired, -1);
	}
	
	// Record last error
	matrixCopy(w_err, last_w_err);
	
	// Find reaction wheel PWM
	wdot2rw_pwm(acs, wdot_desired);
}
