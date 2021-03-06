/**************************************************************************
    Souliss - Lights
    
    It handle the four relays either via IN1 to IN4 inputs or using the
    Android interface. Connecting the relays to lights or similar electrical 
    appliance, you can get remote control of them.
    
    The Gateway node is connected via Ethernet to the Android (or any other)
    interface and via RS485 to all the peer nodes. 
    Use a twisted pair wire to connect the RS485 port of the Gateway to the
    nearest peer and from that peer to the next up to the end of the chain.
    
    Connection scheme:
    
    HOME-ROUTER                     GATEWAY     
    ETHERNET        (connect)       ETHERNET    

    GATEWAY                         PEER1
    RS-485 A        (connect)       RS-485 A
    RS-485 B        (connect)       RS-485 B
    RS-485 GND      (optional)      RS-485 GND
    
    PEER1                           PEER2
    RS-485 A        (connect)       RS-485 A
    RS-485 B        (connect)       RS-485 B
    RS-485 GND      (optional)      RS-485 GND  
    
    PEER2                           PEER3
    RS-485 A        (connect)       RS-485 A
    RS-485 B        (connect)       RS-485 B
    RS-485 GND      (optional)      RS-485 GND  
    
    Applicable for:
        - Light
        - Other ON/OFF electrical appliance
    
***************************************************************************/

#include "bconf/DINo_v2_EthernetBridge_RS485.h"                 // Define the board type
#include "conf/Gateway.h"                                       // The main node is the Gateway, we have just one node

// Include framework code and libraries
#include <SPI.h>

/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"

// Define the network configuration
uint8_t ip_address[4]  = {192, 168, 1, 77};
uint8_t subnet_mask[4] = {255, 255, 255, 0};
uint8_t ip_gateway[4]  = {192, 168, 1, 1};
#define Gateway_address         77
#define myvNet_address          ip_address[3]       // The last byte of the IP address (77) is also the vNet address
#define Gateway_RS485_address   0xCE01
#define Peer1_address           0xCE11
#define Peer2_address           0xCE12
#define Peer3_address           0xCE13
#define myvNet_subnet           0xFF00
#define myvNet_supern           Gateway_RS485_address

#define LIGHT1                  0           // This is the memory slot used for the execution of the logic
#define LIGHT2                  1           // This is the memory slot used for the execution of the logic
#define LIGHT3                  2           // This is the memory slot used for the execution of the logic
#define LIGHT4                  3           // This is the memory slot used for the execution of the logic
#define LIGHT_NO                4           
#define ONTIME                  15          // Light is on for ONTIME number of cycles


void setup()
{   
    InitDINo();

    // Setup the network configuration
    //
    Souliss_SetIPAddress(ip_address, subnet_mask, ip_gateway);
    SetAsGateway((U16)ip_address[3]);                                       // Last byte of the IP address is the vNet address

    // Define the address for the RS485 interface
    Souliss_SetAddress(Gateway_RS485_address, myvNet_subnet, 0x0000);         

    // This node as gateway will get data from the Peer
    SetAsPeerNode(Peer1_address, 1);    
    
    // Set the inputs
    SetInput1();
    SetInput2();
    SetInput3();
    SetInput4();
    
    // Set the outputs
    SetRelay1();
    SetRelay2();
    SetRelay3();
    SetRelay4();
    
    // Define Simple Light logics for the relays
    Set_SimpleLight(LIGHT1);
    Set_SimpleLight(LIGHT2);
    Set_SimpleLight(LIGHT3);
    Set_SimpleLight(LIGHT4);    
    
} 

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_50ms() {   // We process the logic and relevant input and output every 50 milliseconds
        
            DigIn(IN1, Souliss_T1n_ToggleCmd, LIGHT1);          // Read inputs from IN1
            DigIn(IN2, Souliss_T1n_ToggleCmd, LIGHT2);          // Read inputs from IN2
            DigIn(IN3, Souliss_T1n_ToggleCmd, LIGHT3);          // Read inputs from IN3
            DigIn(IN4, Souliss_T1n_ToggleCmd, LIGHT4);          // Read inputs from IN4
        
            Logic_SimpleLight(LIGHT1);                          // Execute the logic for Relay 1
            Logic_SimpleLight(LIGHT2);                          // Execute the logic for Relay 2
            Logic_SimpleLight(LIGHT3);                          // Execute the logic for Relay 3
            Logic_SimpleLight(LIGHT4);                          // Execute the logic for Relay 4
            
            DigOut(RELAY1, Souliss_T1n_Coil, LIGHT1);           // Drive the Relay 1
            DigOut(RELAY2, Souliss_T1n_Coil, LIGHT2);           // Drive the Relay 2
            DigOut(RELAY3, Souliss_T1n_Coil, LIGHT3);           // Drive the Relay 3
            DigOut(RELAY4, Souliss_T1n_Coil, LIGHT4);           // Drive the Relay 4
        } 
        
        // Here we process all communication with other nodes
        FAST_GatewayComms();    
        
    }
    
    EXECUTESLOW() { 
        UPDATESLOW();

        SLOW_10s() {  // Process the timer every 10 seconds  
            Timer_SimpleLight(LIGHT1);
            Timer_SimpleLight(LIGHT2);  
            Timer_SimpleLight(LIGHT3);
            Timer_SimpleLight(LIGHT4);              
        }     
    }
} 