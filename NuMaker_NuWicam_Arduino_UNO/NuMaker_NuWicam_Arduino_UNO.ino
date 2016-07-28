/**
 *  Nuvoton NuMaker NuWicam Arduino sample code.
 *
 *  The purpose of this example is to link the Arduino digital and analog
 *  pins to an external device.
 */
 
#include <Wire.h>
#include <ModbusRtu.h>

// Sharing buffer index
enum {
  eData_MBInCounter,
  eData_MBOutCounter,
  eData_MBError,  
  eData_DI,
  eData_DO,
  eData_RGB,
  eData_MBResistorVar,
  eData_TemperatureSensor,
  eData_Cnt
} E_DATA_TYPE;

#define DEF_MB_SLAVE_ID   1
#define DEF_MB_BAUDRATE   115200

Modbus slave(DEF_MB_SLAVE_ID, 0, 0); // this is slave ID and UART
int8_t state = 0;
unsigned long tempus;

// data array for modbus network sharing
uint16_t au16data[eData_Cnt];

/**
 * pin maping:
 * 2, 3, 4, 5, 6, 7  - digital output
 * 8, 9, 10, 11 - digital input
 * pin 13 is reserved to show a successful query
 */
 
// define i/o
void io_setup() {
  int i;
  for(i=2;i<8;i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW );
  }
    
  for(i=8;i<12;i++)
    pinMode(i, INPUT);

  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH ); // this is for the UNO led pin
}

/**
 *  Link between the Arduino pins and the Modbus array
 */
void io_poll() {
  uint16_t tmp = ~au16data[eData_DO];
  
  // get digital inputs -> au16data[eData_DI]
  bitWrite( au16data[eData_DI], 0, digitalRead( 8 ));
  bitWrite( au16data[eData_DI], 1, digitalRead( 9 ));
  bitWrite( au16data[eData_DI], 2, digitalRead( 10 ));
  bitWrite( au16data[eData_DI], 3, digitalRead( 11 ));

  // set digital outputs -> au16data[eData_DO]
  digitalWrite( 2,  bitRead( tmp, 0 ) );
  digitalWrite( 3,  bitRead( tmp, 1 ) );
  digitalWrite( 4,  bitRead( tmp, 2 ) );
  digitalWrite( 5,  bitRead( tmp, 3 ) );
  digitalWrite( 6, bitRead( tmp, 4 ) );
  digitalWrite( 7, bitRead( tmp, 5 ) );

  // read analog inputs
  au16data[eData_TemperatureSensor] = analogRead( 0 ) / 9.71 ; //Tempeture.
  
  // diagnose communication
  au16data[eData_MBInCounter] = slave.getInCnt();
  au16data[eData_MBOutCounter] = slave.getOutCnt();
  au16data[eData_MBError] = slave.getErrCnt(); 
}

/**
 *  Setup procedure
 */
void setup() {
  analogReference(INTERNAL);
  io_setup(); // I/O settings

  // start communication
  slave.begin( 115200 );
  tempus = millis() + 100;
  digitalWrite(13, HIGH );
}


/**
 *  Loop procedure
 */
void loop() {
  // poll messages
  // blink led pin on each valid message
  state = slave.poll( au16data, eData_Cnt );

  if (state > 4) {
    tempus = millis() + 50;
    digitalWrite(13, HIGH);
  }
  if (millis() > tempus) digitalWrite(13, LOW );

  // link the Arduino pins to the Modbus array
  io_poll();
} 

