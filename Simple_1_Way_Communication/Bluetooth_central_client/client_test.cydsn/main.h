/******************************************************************************
* Project Name		: PSoC_4_BLE_Central_IAS
* File Name			: main.h
* Version 			: 1.0
* Device Used		: CY8C4247LQI-BL483
* Software Used		: PSoC Creator 3.1
* Compiler    		: ARM GCC 4.8.4, ARM RVDS Generic, ARM MDK Generic
* Related Hardware	: CY8CKIT-042-BLE Bluetooth Low Energy Pioneer Kit 
* Owner				: ROIT
*
********************************************************************************
* Copyright (2014-15), Cypress Semiconductor Corporation. All Rights Reserved.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress)
* and is protected by and subject to worldwide patent protection (United
* States and foreign), United States copyright laws and international treaty
* provisions. Cypress hereby grants to licensee a personal, non-exclusive,
* non-transferable license to copy, use, modify, create derivative works of,
* and compile the Cypress Source Code and derivative works for the sole
* purpose of creating custom software in support of licensee product to be
* used only in conjunction with a Cypress integrated circuit as specified in
* the applicable agreement. Any reproduction, modification, translation,
* compilation, or representation of this software except as specified above 
* is prohibited without the express written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH 
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the 
* materials described herein. Cypress does not assume any liability arising out 
* of the application or use of any product or circuit described herein. Cypress 
* does not authorize its products for use as critical components in life-support 
* systems where a malfunction or failure may reasonably be expected to result in 
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of 
* such use and in doing so indemnifies Cypress against all charges. 
*
* Use of this Software may be limited by and subject to the applicable Cypress
* software license agreement. 
*******************************************************************************/

/********************************************************************************
*	Contains all macros and function declaration used in the main.c file 
********************************************************************************/
#if !defined (MAIN_H)
#define MAIN_H
#include <project.h>
#include <string.h>

    
    
#include <stdlib.h>
#include <stdio.h>
//#include <mpu6050.h>
#include <math.h> 

/********************************** Macors ******************************************/
/* General Macros*/
#define TRUE							0x01
#define FALSE							0x00
/******************************************************************************************/
//This is my high low signal. Usually PSoC will have opposite signal configuration. That is
// high will be treated as low. So avoid that confusion inside the coding logic.
    
#define HIGH 0
#define LOW  1
    
/* BD Address Length*/
#define ADV_ADDR_LEN				0x06
	
/* Public Address indication used for Scanning */
#define PUBLIC_ADDR_TYPE			0x00
	
/* Random Address indication used for Scanning */
#define RANDOM_ADDR_TYPE			0x01

    
//********************************Type Defs & Unions****************************************
//This is my datastructures which will be used to store the data in the Bluetooth GATT DB
// Even in the client side also I will be using this datastructure to read the data.
//So in future, if you want to use different data to be transfered like float or more than
// 3 elements to be transferred, then first change it in topdesign. Then here your values
// will have different size or differnt type. Then corresponding bytes.
// Example 1 float = 4 uint8
union ServerBytes{               
  uint8 values[3];                      
  uint8 bytes[3]; 
};  
/********************************************************************************************/

/**************************Function Declarations*****************************/
void InitializeSystem(void);
void BluetoothEventHandler(uint32 event, void *eventparam);
void LoadPeripheralDeviceData(void);
void HandleScanDevices(CYBLE_GAPC_ADV_REPORT_T* scanReport);
/****************************************************************************/

#endif

/* [] END OF FILE */
