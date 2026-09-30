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
constexpr int SERVO_PIN = 18;
Servo servo;

/*---------------------------------------------------------------------
BLUETOOTH CONTROLLER
---------------------------------------------------------------------*/
ControllerPtr myController = nullptr;
bool prevY = false;
bool isStepperEnabled = true;


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

void set_motors(int16_t left_duty, int16_t right_duty) {
    roboclaw.DutyM1(ROBOCLAW_ADDRESS, left_duty);
    roboclaw.DutyM2(ROBOCLAW_ADDRESS, right_duty);
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
    set_motors( 0, 0 );

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
    // delayMicroseconds(10);
    
    for (int i = 0; i < 50; i++) {
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
    BP32.update();

    if ( myController && myController->isConnected() && myController->hasData() )
    {
        /*-------------------------------------------------------------
        Drivetrain Control (Left Joystick Y, Right Joystick X)
        -------------------------------------------------------------*/
        int throttle = (myController->axisY() * -1) * 64; 
        int steering = (myController->axisRX() * -1) * 64;       

        int left_speed = constrain(throttle + steering, -32767, 32767);
        int right_speed = constrain(throttle - steering, -32767, 32767);

        set_motors( left_speed, right_speed ); 

        /*-------------------------------------------------------------
        Claw Pitch Control (D-Pad Up and Down)
        -------------------------------------------------------------*/
        if (myController->dpad() & DPAD_DOWN) {
            // Enable stepper motor before moving
            digitalWrite(EN_PIN, LOW);
            isStepperEnabled = true;
            step_claw(true); 
        } else if (myController->dpad() & DPAD_UP) {
            // Enable stepper motor before moving
            digitalWrite(EN_PIN, LOW); 
            isStepperEnabled = true;
            step_claw(false); 
        }

        /*-------------------------------------------------------------
        Manual Stepper Relax Toggle (Y Button)
        -------------------------------------------------------------*/
        bool currentY = myController->y();
        if (currentY && !prevY) {
            isStepperEnabled = !isStepperEnabled;
            digitalWrite(EN_PIN, isStepperEnabled ? LOW : HIGH); 
        }
        // save state for next iteration
        prevY = currentY; 

        /*-------------------------------------------------------------
        Claw Grip Control (Face Buttons)
        -------------------------------------------------------------*/
        if (myController->a()) {
            servo.write(0);
        } else if (myController->b()) {
            servo.write(90);
        }
    }
}