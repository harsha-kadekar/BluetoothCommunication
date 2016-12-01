/* ======================================================================================
 * Name: Server BLE
 * Description: This is a simple program to show the working of 1-way Bluetooth 
 *              communication. Here server has one custom service. Each time
 *              you press the button(SW2) it will count. This is like a 3 bit counter
 *              for each count a different combination of leds will be flashed in the
 *              client side. 
                0 -> Red low, Green low, Blue low
                1 -> Red low, Green low, Blue high
                2 -> Red low, Green high, Blue low
                3 -> Red low, Green high, Blue high
                4 -> Red high, Green low, Blue low
                5 -> Red high, Green low, Blue high
                6 -> Red high, Green high, Blue low
                7 -> Red high, Green high, Blue high
 *
 * ======================================================================================
*/

#include<main.h>

//This is my counter. It will keep count of the number of times button has been pressed
int nCounter = 0;

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
    
    union ServerBytes ExampleData;              //This is the data to be stored in DB

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
				CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
			}
            
            if(CyBle_GetState() == CYBLE_STATE_CONNECTED)
            {
                //An indication that devices are connected via bluetooth
                Pin_Red_Write(HIGH);
            }
            
		break;
			
		case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
			/* This event is generated at GAP disconnection. 
			* Restart advertisement */
			CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            Pin_Red_Write(LOW);
			break;

			
	    case CYBLE_EVT_GATT_DISCONNECT_IND:
			/* This event is generated at GATT disconnection */
			nCounter = 0; //After disconnection resetting my server state.
            updateGATTDB(nCounter);
            Pin_Red_Write(LOW);
            //After reset, start advertisement - server available for connection with client
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            
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
        
        //If button is pressed
        if(Pin_button_Read() == HIGH)
        {
            nCounter += 1;
            //Reset the counter once count reaches 8
            if(nCounter == 8)
                nCounter = 0;
            
            //Update the new value of count to Server DB so that client can get new values
            updateGATTDB(nCounter);
            
            
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

    //Start your bluetooth. GeneralEventHandler is the function which will your 
    //bluetooth signals/events.
	CyBle_Start(BluetoothEventHandler);
    
    nCounter = 0;
    //Setting the initial values of the Server GATT DB
    updateGATTDB(nCounter);
 
}



/* [] END OF FILE */
