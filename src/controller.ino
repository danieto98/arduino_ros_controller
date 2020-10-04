#include <ros.h>
#include <sensor_msgs/Joy.h>

// Set channel numbers for both joysticks
#define J1_SELECT_CHANNEL 2
#define J1_X_CHANNEL A0
#define J1_Y_CHANNEL A1

#define J2_SELECT_CHANNEL 3
#define J2_X_CHANNEL A2
#define J2_Y_CHANNEL A3

// Start up ROS and advertise publisher
ros::NodeHandle nh;
sensor_msgs::Joy joy_msg;
ros::Publisher joy_pub("joy", &joy_msg);

// Variables for joystick values
long int selects[2];
float axes[4];

// ISRs for button state changes on the joysticks
void j1_read(){
  selects[0] = digitalRead(J1_SELECT_CHANNEL);
}

void j2_read(){
  selects[1] = digitalRead(J2_SELECT_CHANNEL);
}

// Function to map a float from one value range to another
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup() {
  // Set input pins and interrupts on button state changes
  pinMode(J1_SELECT_CHANNEL, INPUT);
  pinMode(J1_X_CHANNEL, INPUT);
  pinMode(J1_Y_CHANNEL, INPUT);

  attachInterrupt(digitalPinToInterrupt(J1_SELECT_CHANNEL), j1_read, CHANGE);

  pinMode(J2_SELECT_CHANNEL, INPUT);
  pinMode(J2_X_CHANNEL, INPUT);
  pinMode(J2_Y_CHANNEL, INPUT);

  attachInterrupt(digitalPinToInterrupt(J2_SELECT_CHANNEL), j2_read, CHANGE);

  // Initialize ROS and advertise joy topic
  nh.initNode();
  nh.advertise(joy_pub);
}

void loop() {
  // Read analog values from both joysticks
  axes[0] = mapfloat((float) analogRead(J1_X_CHANNEL), 0, 1023, -1, 1);
  axes[1] = mapfloat((float) analogRead(J1_Y_CHANNEL), 0, 1023, -1, 1);

  axes[2] = mapfloat((float) analogRead(J2_X_CHANNEL), 0, 1023, -1, 1);
  axes[3] = mapfloat((float) analogRead(J2_Y_CHANNEL), 0, 1023, -1, 1);

  // Create ROS message content
  joy_msg.buttons_length = 2;
  joy_msg.axes_length = 4;
  joy_msg.buttons = selects;
  joy_msg.axes = axes;

  // Send message
  joy_pub.publish(&joy_msg);

  // Spin
  nh.spinOnce();
  delay(10);
}
