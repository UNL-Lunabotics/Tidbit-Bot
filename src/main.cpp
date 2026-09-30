#include <Arduino.h>
#include <Bluepad32.h>
#include <Basicmicro.h>
#include <ESP32Servo.h>


/*---------------------------------------------------------------------
STEPPER DRIVER PINS
---------------------------------------------------------------------*/
constexpr int STEP_PIN = 25;           // Stepper pin
constexpr int DIR_PIN = 26;            // Direction of movement pin
constexpr int EN_PIN = 27;             // Enable pin (active low)

/*---------------------------------------------------------------------
ROBOCLAW MOTOR DRIVER PINS
---------------------------------------------------------------------*/
#define ROBOCLAW_RX 16
#define ROBOCLAW_TX 17

constexpr uint8_t ROBOCLAW_ADDRESS = 131;
constexpr uint32_t ROBOCLAW_BAUD = 115200;

Basicmicro roboclaw( &Serial2, 10000 );

/*---------------------------------------------------------------------
SERVO PINS
---------------------------------------------------------------------*/
constexpr int SERVO_PIN = 18;
Servo servo;

/*---------------------------------------------------------------------
BLUETOOTH CONTROLLER
---------------------------------------------------------------------*/
ControllerPtr myController = nullptr;


/*---------------------------------------------------------------------
BLUETOOTH CONTROLLER FUNCTIONS
---------------------------------------------------------------------*/
void onConnectedController( ControllerPtr ctl )
{
    if ( myController == nullptr ) myController = ctl;
}

void onDisconnectedController( ControllerPtr ctl ) 
{
    if ( myController == ctl ) myController = nullptr;
}


/*---------------------------------------------------------------------
SETUP FUNCTION
---------------------------------------------------------------------*/
void setup()
{
    /*-----------------------------------------------------------------
    Set up pin modes
    -----------------------------------------------------------------*/
    pinMode( STEP_PIN, OUTPUT );
    pinMode( DIR_PIN, OUTPUT );
    pinMode( EN_PIN, OUTPUT );

    /*-----------------------------------------------------------------
    Start Roboclaw driver for drivetrain motors
    -----------------------------------------------------------------*/
    Serial2.begin( ROBOCLAW_BAUD, SERIAL_8N1, ROBOCLAW_RX, ROBOCLAW_TX );
    roboclaw.begin( ROBOCLAW_BAUD );

    /*-----------------------------------------------------------------
    Keep stepper driver disabled on robot initialization 
    -----------------------------------------------------------------*/
    digitalWrite( STEP_PIN, LOW );
    digitalWrite( DIR_PIN, LOW );

    /*-----------------------------------------------------------------
    Initialize Bluepad32 and register connection lifecycle callbacks 
    -----------------------------------------------------------------*/
    BP32.setup( &onConnectedController, &onDisconnectedController );
}


/*---------------------------------------------------------------------
LOOP FUNCTION
---------------------------------------------------------------------*/
void loop()
{
    bool dataUpdated = BP32.update();

    if ( dataUpdated )
    {

    }
}