/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include <main.h>

/* 'list_of_devices' is  of type 'CYBLE_GAPC_ADV_REPORT_T' (defined in 
 * BLE_StackGap.h) that is used to store the reports of the peripheral devices
* being scanned by this Central device*/
CYBLE_GAPC_ADV_REPORT_T 	list_of_devices;

/* 'connectPeriphDevice' is a variable of type 'CYBLE_GAP_BD_ADDR_T' (defined in 
* BLE_StackGap.h) and is used to store address of the connected device. */
CYBLE_GAP_BD_ADDR_T 		connectPeriphDevice;

/* 'connHandle' is a variable of type 'CYBLE_CONN_HANDLE_T' (defined in 
* BLE_StackGatt.h) and is used to store the connection handle parameters after
* connecting with the peripheral device. */
CYBLE_CONN_HANDLE_T			connHandle;

//This button toggle state. First time button is pressed, it will set to 1, next time
// button is pressed it will reset the value to 0. Based on this server led will be on
//on off
uint8 OnOff = 0;


/* 'deviceConnected' flag tells the status of connection with BLE peripheral Device */
uint8 deviceConnected = FALSE;


/* 'address_store' stores the addresses returned by Scanning results */
uint8 address_store[6];

/* 'peripheralAddress' stores the addresses of device presently connected to */
uint8 peripheralAddress[6];

/* 'peripheralFound' flag tells whether the peripheral device with the required Address
 * has been found during scanning or not. */
uint8 peripheralFound = FALSE;

/* 'addedDevices' tells the number of devices that has been added to the list till now. */
uint8 addedDevices = FALSE;

/* 'restartScanning' flag indicates to application whether starting scan API has to be
* called or not */
uint8 restartScanning = FALSE;

//This datastructure is used to recieve the data sent by server
union ServerBytes sensor_data;
//This datastructure is used to send the data to the server.
union ClientBytes send_data;


char buf[200]; //just to hold text values in for writing to UART

/*******************************************************************************
* Function Name: LoadPeripheralDeviceData
********************************************************************************
* Summary:
*        This function stores the address of the peripheral device to which this
* Central device wishes to connect. 
*
* Parameters:
*  void
*
* Return:
*  void
*

*******************************************************************************/
void LoadPeripheralDeviceData(void)
{
	/* This stores the 6-byte BD address of peripheral device to which we have to connect.
       This is Servers MAC address. To know the MAC Address. Program your server, 
       Open your CySmart App in your phone. Serach for devices. It should list "Server BLE"
       device. Below the name it should list the MAC address of that device.
	.*/
    
		peripheralAddress[5] = 0x00;
		peripheralAddress[4] = 0xA0;
		peripheralAddress[3] = 0x50;
		peripheralAddress[2] = 0x16;
		peripheralAddress[1] = 0x1E;
		peripheralAddress[0] = 0x1A;
             
}

/*******************************************************************************
* Function Name: HandleScanDevices
********************************************************************************
* Summary:
*        This function checks for new devices that have been scanned 
*           sets flag to allow connection with it.
*
* Parameters:
*  scanReport:		parameter of type CYBLE_GAPC_ADV_REPORT_T* returned by BLE
* 					event CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT.
*
* Return:
*  void
*
*******************************************************************************/
void HandleScanDevices(CYBLE_GAPC_ADV_REPORT_T* scanReport)
{
    //Compare scanned device address with the server address
	if(FALSE == memcmp(&peripheralAddress[0], scanReport->peerBdAddr, ADV_ADDR_LEN))
	{
		/* Save the connected device BD Address and Type*/
		connectPeriphDevice.bdAddr[0] = scanReport->peerBdAddr[0];
		connectPeriphDevice.bdAddr[1] = scanReport->peerBdAddr[1];
		connectPeriphDevice.bdAddr[2] = scanReport->peerBdAddr[2];
		connectPeriphDevice.bdAddr[3] = scanReport->peerBdAddr[3];
		connectPeriphDevice.bdAddr[4] = scanReport->peerBdAddr[4];
		connectPeriphDevice.bdAddr[5] = scanReport->peerBdAddr[5];
        
		
		connectPeriphDevice.type = scanReport->peerAddrType;
			
		/* Set the flag to notify application of a connected peripheral device */
		peripheralFound = TRUE;	
		
		/* Stop existing BLE Scan */
		CyBle_GapcStopScan();
	}
	
	
}

/*******************************************************************************
* Function Name: BluetoothEventHandler
********************************************************************************
* Summary:
*        Call back event function to handle various events from BLE stack
*
* Parameters:
*  event:		event returned
*  eventParam:	link to value of the event parameter returned
*
* Return:
*  void
*
*******************************************************************************/
void BluetoothEventHandler(uint32 event, void *eventparam)
{
	/* 'apiResult' is a variable of type 'CYBLE_API_RESULT_T' (defined in 
	* BLE_StackTypes.h) and is used to store the return value from BLE APIs. */
	CYBLE_API_RESULT_T 			apiResult;
	
	/* 'scan_report' is a variable of type 'CYBLE_GAPC_ADV_REPORT_T' (defined in 
	* BLE_StackGap.h) and is used to store report retuned from Scan results. */
	CYBLE_GAPC_ADV_REPORT_T		scan_report;
    
    //This is the structure sent by server for any read requests initiated by client
    CYBLE_GATTC_READ_RSP_PARAM_T *readParams;
	

    
    uint8 red = 0;
    uint8 green = 0;
    uint8 blue = 0;
	
	switch(event)
	{
		case CYBLE_EVT_STACK_ON:
			
			/* Set start scanning flag to allow calling the API in main loop */
			restartScanning = TRUE;
		break;
			
		case CYBLE_EVT_GAPC_SCAN_START_STOP:
			/* Add relevant code here pertaining to Starting/Stopping of Scan*/
			if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED)
			{
				
				if(!peripheralFound)
				{
					
					restartScanning = TRUE;
				}
			}
			else
			{
				
			}
		break;
			
		case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
			/* This event is generated whenever there is a device found*/
			if(CYBLE_STATE_CONNECTED != CyBle_GetState())	
			{
				/* If we are not connected to any peripheral device, then save the new device  
					information so to add it to our list */
				scan_report = *(CYBLE_GAPC_ADV_REPORT_T*) eventparam;
				
				/* Add the new device to existing list if not done yet, and compare the address with our
					required address.*/
				HandleScanDevices(&scan_report);
			}
			
		break;
			
 		case CYBLE_EVT_GATT_CONNECT_IND:

			/* When the peripheral device is connected, store the connection handle.*/
            connHandle = *(CYBLE_CONN_HANDLE_T *)eventparam;
            break;
			
		case CYBLE_EVT_GATT_DISCONNECT_IND:
			/* When the peripheral device is disconnected, reset variables*/
			
			break;
			
		case CYBLE_EVT_GAP_DEVICE_CONNECTED:
			
			/* The Device is connected now. Start Attributes discovery process.*/
			apiResult = CyBle_GattcStartDiscovery(connHandle);
			
            
			if(apiResult != CYBLE_ERROR_OK)
			{

			}
			
			
        break;
			
		case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
			/* Reset all saved peripheral Addresses */
				list_of_devices.peerBdAddr[0] = FALSE;
				list_of_devices.peerBdAddr[1] = FALSE;
				list_of_devices.peerBdAddr[2] = FALSE;
				list_of_devices.peerBdAddr[3] = FALSE;
				list_of_devices.peerBdAddr[4] = FALSE;
				list_of_devices.peerBdAddr[5] = FALSE;
			
			/* Reset application Flags on BLE Disconnect */
			addedDevices = FALSE;
			peripheralFound = FALSE;
			deviceConnected = FALSE;
			
			/* Set the flag for rescanning after wakeup */
			restartScanning = TRUE;
                
            OnOff = 0;
			
			if(apiResult != CYBLE_ERROR_OK)
			{
			}
			break;
			
	
		case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
			/* This event is generated whenever the discovery procedure is complete*/
			
			/*Set the Device connected flag*/
			deviceConnected = TRUE;
			
		break;
            
        case CYBLE_EVT_GATTC_READ_RSP:
			/* This event is generated when the server sends a response to a read request*/
			readParams = (CYBLE_GATTC_READ_RSP_PARAM_T *) eventparam;
            
            red = readParams->value.val[0];
            green = readParams->value.val[1];
            blue = readParams->value.val[2];
            
            sensor_data.values[0] = readParams->value.val[0];
            sensor_data.values[1] = readParams->value.val[1];
            sensor_data.values[2] = readParams->value.val[2];
            
            if(red == HIGH)
                Pin_Red_Write(HIGH);
            else
                Pin_Red_Write(LOW);
                
            if(green == HIGH)
                Pin_Green_Write(HIGH);
            else
                Pin_Green_Write(LOW);
                
            if(blue == HIGH)
                Pin_Blue_Write(HIGH);
            else
                Pin_Blue_Write(LOW);    
            
            
            
            
		break;
			
	}
}

/*
Name: updateGATTDB
Description: This function will update Client's GATTDB so that server can read the values
             Here I am storing a 1 byte value as LED ON OFF status in client's GATT DB. 
             Whenever server do a read on client, the values in GATTDB will be sent to server.
Parameters: onOFF - value to be sent to client
Return: - 
*/
void updateGATTDB(uint8 onOff)
{
     CYBLE_GATT_HANDLE_VALUE_PAIR_T		OnOffDataHandle; //This describes the data to be sent to server. 
       
    union ClientBytes ExampleData; //This is the data to be stored in DB
    
    //Put that value to the Client DB datastructure.
    ExampleData.values[0] = onOff;   
    
    //Before committing the value to GATTDB, process any previous
    //bluetooth signals.
    CyBle_ProcessEvents();
            
    OnOffDataHandle.attrHandle = 0x000Cu; //CYBLE_ONOFFSERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE in BLE_custom.h
    OnOffDataHandle.value.val = ExampleData.bytes; //Actual values to be stored
	OnOffDataHandle.value.len = 1; //Number of bytes to be transferred or stored
    
    //This function call will store the value to the Client GATTDB. 
    // Any read requests from server after executing below function will 
    // reflect the new values
    CyBle_GattsWriteAttributeValue(&OnOffDataHandle, 
									FALSE, 
									&cyBle_connHandle, 
									CYBLE_GATT_DB_LOCALLY_INITIATED);
    
}


int main()
{
	InitializeSystem();
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /*Process Event callback to handle BLE events. The events generated and 
		* used for this application are inside the 'ApplicationEventHandler' routine */
		CyBle_ProcessEvents();
        
        if(peripheralFound)
        {
			/* If the desired peripheral is found, then connect to that peripheral */
			CyBle_GapcConnectDevice(&connectPeriphDevice);

			/* Call CyBle_ProcessEvents() once to process the Connect request sent above. */
			CyBle_ProcessEvents();
		
			/* Reset flag to prevent resending the Connect command */
			peripheralFound = FALSE;
		}
    
        /* If rescanning flag is TRUE, the restart the scan */
		if(restartScanning)
		{
			/* Reset flag to prevent calling multiple Start Scan commands */
			restartScanning = FALSE;
			
			/* Start Fast BLE Scanning. This API will only take effect after calling
			* CyBle_ProcessEvents()*/
			CyBle_GapcStartScan(CYBLE_SCANNING_FAST);
		}   
        
        if(deviceConnected)
        {
            //If server is connected than request the data from the server
            //Here we are reading 3 byte array for RGB from server
            // We are interested in CYBLE_CUSTOMSERVICE_TRANSMITDATA_CHAR_HANDLE
         CyBle_GattcReadCharacteristicValue(connHandle, 0x000Eu);   //*****Characteristic Handle From Server BLE CUSTOM.H*******
        }
        
        if(Pin_Button_Read() == HIGH)
        {
            if(OnOff == 0)
                OnOff = 1;
            else
                OnOff = 0;
            //Update the new value to Client DB so that Server can get new values
            updateGATTDB(OnOff);
        }
        

        //Just to show in UART, what values we recieved from Server
      sprintf(buf, "red = %d\ngreen = %d\nblue = %d\n\n", sensor_data.values[0], sensor_data.values[1], sensor_data.values[2]);
      SERIAL_UartPutString(buf);
    
    
    }
}

/*
Name: InitializeSystem
Description: This function will bring your system to ready state or initial state
             Here we switch on components like bluetooth or UART, switch of LEDs
             like that.
Parameters: - 
Return: - 
*/
void InitializeSystem(void)
{
	/* Enable global interrupts. */
	CyGlobalIntEnable; 
	
    //Start the UART
    SERIAL_Start();
    
	/* Start BLE component with appropriate Event handler function */
	CyBle_Start(BluetoothEventHandler);	
	
	/* Load the BD address of peripheral device to which we have to connect */
	LoadPeripheralDeviceData(); 
    
    sensor_data.values[0] = 0;
    sensor_data.values[1] = 0;
    sensor_data.values[2] = 0;
    
    OnOff = 0;
    
    
	
}

/* [] END OF FILE */
