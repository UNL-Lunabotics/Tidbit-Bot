#include <Arduino.h>
#include <Bluepad32.h>
#include <Basicmicro.h>
#include <ESP32Servo.h>


/*---------------------------------------------------------------------
STEPPER DRIVER PINS
---------------------------------------------------------------------*/
constexpr int STEP_PIN = 5;             // Stepper pin
constexpr int DIR_PIN = 16;             // Direction of movement pin
constexpr int EN_PIN = 17;              // Enable pin (active low)

/*---------------------------------------------------------------------
ROBOCLAW MOTOR DRIVER PINS
---------------------------------------------------------------------*/
#define ROBOCLAW_RX 26                  // Roboclaw serial RX pin
#define ROBOCLAW_TX 27                  // Roboclaw serial TX pin

constexpr uint8_t ROBOCLAW_ADDRESS = 131;
constexpr uint32_t ROBOCLAW_BAUD = 115200;

Basicmicro roboclaw( &Serial2, 10000 );

/*---------------------------------------------------------------------
SERVO PINS
---------------------------------------------------------------------*/
constexpr int SERVO_PIN = 32;
Servo servo;

/*---------------------------------------------------------------------
BLUETOOTH CONTROLLER
---------------------------------------------------------------------*/
ControllerPtr myController = nullptr;


/**********************************************************************
 * FUNCTION: on_connected_controller( ControllerPtr ctl ) 
**********************************************************************/
void on_connected_controller( ControllerPtr ctl )
{
    if ( myController == nullptr ) myController = ctl;
}

/**********************************************************************
 * FUNCTION: on_disconnected_controller( ControllerPtr ctl ) 
**********************************************************************/
void on_disconnected_controller( ControllerPtr ctl ) 
{
    if ( myController == ctl ) myController = nullptr;
}


/**********************************************************************
 * SETUP FUNCTION
**********************************************************************/
void setup()
{
    /*-----------------------------------------------------------------
    Start serial
    -----------------------------------------------------------------*/
    Serial.begin(115200);
    
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
    setMotors(0);

    /*-----------------------------------------------------------------
    Keep stepper driver disabled on robot initialization 
    -----------------------------------------------------------------*/
    digitalWrite( STEP_PIN, LOW );
    digitalWrite( DIR_PIN, LOW );
  
    /*-----------------------------------------------------------------
    Setup servo 
    -----------------------------------------------------------------*/
    servo.setPeriodHertz(50);
    servo.attach(SERVO_PIN);
    servo.write(90);

    /*-----------------------------------------------------------------
    Initialize Bluepad32 and register connection lifecycle callbacks 
    -----------------------------------------------------------------*/
    BP32.setup( &on_connected_controller, &on_disconnected_controller );
}


/**********************************************************************
 * FUNCTION: step_claw()
**********************************************************************/
void step_claw( bool direction )
{
    digitalWrite( DIR_PIN, direction ? HIGH : LOW );
    delayMicroseconds(10);
    
    for (int i = 0; i < 5; i++) {
        digitalWrite( STEP_PIN, HIGH );
        delayMicroseconds( 1000 );
        digitalWrite( STEP_PIN, LOW );
        delayMicroseconds( 1000 );
    }
}


/**********************************************************************
 * FUNCTION: loop()
**********************************************************************/
void loop()
{
    bool dataUpdated = BP32.update();

    if ( myController && dataUpdated && myController->isConnected() && myController->hasData() )
    {
        
    }
}