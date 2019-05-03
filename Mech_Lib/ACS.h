/**
  ******************************************************************************
  * @file           : ACS.h
  * @brief          : Header file for the Attitude Control System (ACS).
  ******************************************************************************
	* Created by Galen Savidge. Edited 4/21/2019.
  ******************************************************************************
  */

#ifndef ACS_H
#define ACS_H

/* Includes ------------------------------------------------------------------*/
#include <Matrix.h>
#include <DigitalFilters.h>
#include <ReferenceFrames.h>
#include <main.h>
#include <stdint.h>
#include <string.h>


/* Constants -----------------------------------------------------------------*/
#define NUM_SOLAR_PANELS 6
#define MAX_MOMENT 2.0

 
/* Datatypes -----------------------------------------------------------------*/
typedef enum {
	SV_FOUND,
	SV_NOTFOUND,
	SV_DARK
}SV_Status;

typedef struct {
	// Current time
	double julian_date;
	float t; // In seconds
	float dt;
	
	// Craft DCM
	Matrix R;
	Matrix Rt;
	
	// Sensor vectors (body frame)
	Matrix gyro_vector;
	Matrix mag_vector;
	Matrix solar_vector;
	
	// Attitude estimation vectors
	Matrix gyro_bias;
	
	// Inertial vectors (ecliptic frame)
	Matrix sv_inertial; // Found using the Julian date
	Matrix mag_inertial; // From IGRF
	Matrix craft_inertial; // From SGP4 (normalized)
	Matrix craft_j2000;
	
	// Feedback control error vectors (body frame)
	Matrix z_err;
	Matrix n_err;
	Matrix err; // err = z_err + n_err
	
	// Sensor hardware
	MovingAvgFilter mag_filter;
	MovingAvgFilter sv_filter;
	uint32_t sv_raw[NUM_SOLAR_PANELS]; // For DMA to use
	
	// Solar vector status
	SV_Status sun_status;
	
	// Satellite dynamic system
	Matrix w_rw; // Reaction wheel speed vector (body frame)
	Matrix J_rw; // Reaction wheel inertia matrix
	Matrix J_rw_inv; // Inverse of J_rw
	Matrix A_rw; // Reaction wheel projection onto body axes
	Matrix J_body; // Craft inertia matrix
	Matrix J_body_inv; // Inverse of J_body
	
	// Actuator PWMs
	Matrix rw_PWM;
	Matrix tr_PWM;
	
	// UART handle
	UART_HandleTypeDef* huart;
}ACSType;


/* Initialization Functions --------------------------------------------------*/

/** 
 * @brief  Initializes all the fields in the given ACS struct
 * @param  acs: a pointer to an existing Attitude Control System object
 * @return None
*/
void initializeACS(ACSType* acs);

/* Serial Communication Functions ---------------------------------------------*/

/** 
 * @brief  Initializes serial communication from the ACS
 * @param  acs: a pointer to an existing Attitude Control System object
 * @param  huart: pointer to a UART handle defined in main.c
 * @return None
*/
void initializeACSSerial(ACSType* acs, UART_HandleTypeDef* huart);

/** 
 * @brief  Reads sensor data from serial and updates acs
 * @param  acs: a pointer to an existing Attitude Control System object
 * @return Updates mag_vector, gyro_vector, and solar_vector in acs
*/
void readSensorsFromSerial(ACSType* acs);

/** 
 * @brief  Sends actuator PWMs from acs over serial
 * @param  acs: a pointer to an existing Attitude Control System object
 * @return None
*/
void sendActuatorsToSerial(ACSType* acs);

#endif
