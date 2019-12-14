
// I/O positions
#define switch_1 2
#define switch_2 3
#define switch_3 4
#define motor_pin_1 7
#define motor_pin_2 8
#define motor_speed_pin 6
#define switch_trigger 5
#define flashing_light 9

// default values
#define homing_speed 255
#define motor_speed_1 255
#define flashing_light_time 300000

// global variabels
int switch_status[5];
int switch_pin[5];
int motor_status; // 0 = off, 1 = clockwise, 2 = counterclockwise
bool trigger;
bool trigger_set = false;
bool homing;
bool flashing_light_status;
long trigger_counter;
int trigger_state_machine;

void setup() {
  Serial.begin(9600);
  determine_arrays();
  set_pinmode();

  while (!homing) {
    Serial.println("Homing");
    read_switch();
    homing_cycle();
  }
  
  Serial.println("Start program");
}

void loop() {
 
  read_switch();
  program_cycle();

}

void motor_set(bool DIRECTION, int SPEED) { // direction true = clockwise, false = counerclockwise
  
  if (DIRECTION) {
    digitalWrite(motor_pin_1, HIGH);
    digitalWrite(motor_pin_2, LOW);
    motor_status = 1;
  } else {
    {
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, HIGH);
      motor_status = 2;
    }
  }
  analogWrite(motor_speed_pin, SPEED);
}

void motor_stop() {
  digitalWrite(motor_pin_1, LOW);
  digitalWrite(motor_pin_1, LOW);
  analogWrite(motor_speed_pin, 0);
  motor_status = 0;
}

void determine_arrays() {
  switch_pin[1] = switch_1;
  switch_pin[2] = switch_2;
  switch_pin[3] = switch_3;
  homing = false;
  flashing_light_status = false;
  trigger_set = false;
  trigger_state_machine = 1;
}

void set_pinmode() {
  pinMode(switch_1, INPUT);
  pinMode(switch_2, INPUT);
  pinMode(switch_3, INPUT);
  pinMode(motor_pin_1, OUTPUT);
  pinMode(motor_pin_2, OUTPUT);
  pinMode(motor_speed_pin, OUTPUT);
  pinMode(switch_trigger, INPUT);
  pinMode(flashing_light, OUTPUT);
  digitalWrite(switch_trigger, HIGH);
  digitalWrite(switch_1, HIGH);
  digitalWrite(switch_2, HIGH);
  digitalWrite(switch_3, HIGH);
}

void read_switch() {
 
  for (int i = 0 ; i < 5; i++) {

    if (digitalRead(switch_pin[i]) == 1) {

    switch_status[i] = 0;
    } else {
      switch_status[i] = 1;
    }

  }
  if (digitalRead(switch_trigger) == 1) {
    trigger = false;
  } else {
    trigger_set = true;
   trigger = true;
  }

}

void set_flashing_light(bool STATUS) {
  if (STATUS == true) {
    digitalWrite(flashing_light, HIGH);
  } else {
    digitalWrite(flashing_light, LOW);
  }
}

void homing_cycle() {
  if (motor_status != 2) {
    motor_set(false, homing_speed);
  }
  if (switch_status[1] == 1) {
    motor_stop();
    homing = true;
  }
}

void program_cycle() {

  if (trigger_set) {

    switch (trigger_state_machine) {
      case 0:
      case 1:
        if (motor_status != 1) {

          motor_set(true, motor_speed_1);

        }
        if (switch_status[3] == 1) {
          motor_stop();
          set_flashing_light(true);
          trigger_state_machine = 2;
        }
        break;

      case 2:

        if (trigger_counter != flashing_light_time) {
          trigger_counter ++;
        } else {
          trigger_counter = 0;
          set_flashing_light(false);
          trigger_state_machine = 1;
          motor_set(false, motor_speed_1);
          trigger_set = false;
        }

        break;
    }
  } else {

    if (motor_status == 0) {
    
      motor_set(true, motor_speed_1);
    }

    if (switch_status[2] == 1) {
      
      motor_set(false, motor_speed_1);

    } else if (switch_status[1] == 1)
    {
      motor_set(true, motor_speed_1);

    }

  }

}
