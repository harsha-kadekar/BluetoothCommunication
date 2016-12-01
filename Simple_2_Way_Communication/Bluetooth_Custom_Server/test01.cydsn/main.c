/* ==================================================================================================
 *
 * Name: Server BLE
 * Description: This is a simple program to illiustrate the 2-way communication
 *              between the server and client in Bluetooth.
 *              Server reads the data from client as well as Client reads the data
 *              from the server. When you press the button in server, LED of client
                will be flashed, Similarly when you press the button in client side
                LED of server will be flashed. Server counts the number of button presses
                based on that corresponding LEDS will be on in client side. In client side
                it is just On and Off. Each time you press button in client, LED will On of OFF
                in server.
                0 -> Red low, Green low, Blue low
                1 -> Red low, Green low, Blue high
                2 -> Red low, Green high, Blue low
                3 -> Red low, Green high, Blue high
                4 -> Red high, Green low, Blue low
                5 -> Red high, Green low, Blue high
                6 -> Red high, Green high, Blue low
                7 -> Red high, Green high, Blue high
 *
 * =====================================================================================================
*/

#include<main.h>

/* 'connHandle' is a variable of type 'CYBLE_CONN_HANDLE_T' (defined in 
* BLE_StackGatt.h) and is used to store the connection handle parameters after
* connecting with the device. */
CYBLE_CONN_HANDLE_T			connHandle;

//This is my counter to count the number of times button is pressed in this device.
int nCounter = 0;
//This is the value which will be recieved from client end
uint8 onOff = 0;
//This is the datastructure used to recieved the data which was sent by client.
union ClientBytes recievedData;

//This flag will indicate whether client is connected or not.
int deviceConnected = FALSE;

/*
Name: updateGATTDB
Description: This function will update Server's GATTDB so that client can read the values
             Here I am converting my counter value to RGB value and storing as 3 byte array
             in server's GATT DB. Whenever client do a read on server, the values in GATTDB
             will be sent to client.
Parameters: counter - Number of times button has been pressed or value to be sent to client
Return: - 
*/
void updateGATTDB(int counter)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T		TransmitDataHandle; //This describes the data to be sent to client. 
       
    union ServerBytes ExampleData;      //This is the data to be stored in DB
    uint8 red= LOW;    
    uint8 green = LOW;
    uint8 blue = LOW;
    
    //This switch converts my counter to a 3 byte value for the RGB LED light
    switch(counter)
    {
        case 0:
                {
                    red = green = blue = LOW;
                    break;
                }
        case 1:
                {
                    red = green = LOW;
                    blue = HIGH;
                    break;
                }
        case 2:
                {
                    red = LOW;
                    green = HIGH;
                    blue = LOW;
                    break;
                }
        case 3:
                {
                    red = LOW;
                    green = HIGH;
                    blue = HIGH;
                    break;
                }
        case 4:
                {
                    red = HIGH;
                    green = LOW;
                    blue = LOW;
                    break;
                }
        case 5:
                {
                    red = HIGH;
                    green = LOW;
                    blue = HIGH;
                    break;
                }
        case 6:
                {
                    red = HIGH;
                    green = HIGH;
                    blue = LOW;
                    break;
                }
        case 7:
                {
                    red = HIGH;
                    green = HIGH;
                    blue = HIGH;
                    break;
                }
    }
    
    //Put that calculated value to the Server DB datastructure.
    ExampleData.values[0] = red;   
    ExampleData.values[1] = green;    
    ExampleData.values[2] = blue;

    //Before committing the value to GATTDB, process any previous
    //bluetooth signals.
    CyBle_ProcessEvents();
    
    TransmitDataHandle.attrHandle = 0x000Eu;                //CYBLE_CUSTOMSERVICE_TRANSMITDATA_CHAR_HANDLE in BLE_custom.h
    TransmitDataHandle.value.val = ExampleData.bytes;       //Actual values to be stored
	TransmitDataHandle.value.len = 3;                       //Number of bytes to be transferred or stored
    
    //This function call will store the value to the Server GATTDB. 
    // Any read requests from client after executing below function will 
    // reflect the new values
    CyBle_GattsWriteAttributeValue(&TransmitDataHandle, 
									FALSE, 
									&cyBle_connHandle, 
									CYBLE_GATT_DB_LOCALLY_INITIATED);
    
}

/*
Name: BluetoothEventHandler
Description: This function will handle all the incoming or outgoing BLUETOOH signals/events
Parameters: event - Type of signal like bluetooth started, bluetooth connected to another device,
                disconnected from client, read request, write request, etc.
        eventParams - information related to bluetooth event. Like if client wants to write
                       values, then event will write event and eventParams will have data to be written
return: -

*/
void BluetoothEventHandler(uint32 event, void * eventParam)
{
    //This is the structure which will be passed from the client side
    //when we request for data from client.
    CYBLE_GATTC_READ_RSP_PARAM_T *readParams;
	
	switch(event)
    {
		case CYBLE_EVT_STACK_ON:
			/* BLE stack is on. Start BLE advertisement */
			CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
		break;
		
		case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
			/* This event is generated whenever Advertisement starts or stops.
			* The exact state of advertisement is obtained by CyBle_State() */
			if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED)
			{
                Pin_Status_Write(LOW);
				CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
			}
           if(CyBle_GetState() == CYBLE_STATE_CONNECTED)
            {
                deviceConnected = TRUE;
            }
		break;
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            {
                deviceConnected = TRUE;
                break;
            }
			
		case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
			/* This event is generated at GAP disconnection. 
			* Restart advertisement */
            deviceConnected = FALSE;
            nCounter = 0;
            updateGATTDB(nCounter);
			CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
			break;

			
	    case CYBLE_EVT_GATT_DISCONNECT_IND:
			/* This event is generated at GATT disconnection */
			deviceConnected = FALSE;
			nCounter = 0;
            updateGATTDB(nCounter);
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
			break;
        
        case CYBLE_EVT_GATT_CONNECT_IND:

			/* When the peripheral device is connected, store the connection handle.*/
            connHandle = *(CYBLE_CONN_HANDLE_T *)eventParam;
            deviceConnected = TRUE;
            break;
        
        case CYBLE_EVT_GATTC_READ_RSP:
			/* This event is generated when the client sends a response to a read request*/
			readParams = (CYBLE_GATTC_READ_RSP_PARAM_T *) eventParam;
            
            onOff = readParams->value.val[0];
            
            
            recievedData.values[0] = readParams->value.val[0];
            
            if(onOff == HIGH)
                Pin_Red_Write(HIGH);
            else
                Pin_Red_Write(LOW);
                
            break;
			
        default:

       	break;
    }   	
}


int main()
{
    /* Start the components */
	InitializeSystem();
    
   
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {   
        //Process the BLUETOOTH events like incoming read request or notifications
        CyBle_ProcessEvents();
        
        //if button is pressed
        if(Pin_button_Read() == HIGH)
        {
            nCounter += 1;
            //Reset the counter once count reaches 8
            if(nCounter == 8)
                nCounter = 0;
            
            //Update the new value of count to Server DB so that client can get new values
            updateGATTDB(nCounter);
            
            
        }
        
        //If client is connected then only request for data from client
        if(deviceConnected == TRUE)
        {
            //This is just to indicate the client is connected.
            Pin_Status_Write(HIGH);
            //Here I am trying to read the Client Service - CYBLE_ONOFFSERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE
            CyBle_GattcReadCharacteristicValue(connHandle, 0x000Cu);   //*****Characteristic Handle From Clients BLE_Custom.H*******
            
            
        }
        else
        {
            Pin_Status_Write(LOW);
        }

         CyDelay(250);
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
	/* Enable Global Interrupt Mask */
	CyGlobalIntEnable;		

    //This will start the bluetooth and BluetoothEventHandler function will be used
    //to handle the bluetooth events.
	CyBle_Start(BluetoothEventHandler);
    
    nCounter = 0;
    //Set the initial values in the Server GATT DB
    updateGATTDB(nCounter);
    
    //Indicates that no client is connected.
    Pin_Status_Write(LOW);
    
    
    
    
}



/* [] END OF FILE */
