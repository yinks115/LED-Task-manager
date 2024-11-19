#include <millisDelay.h>//library with millisDelay objects

#define numLEDs 13 //number of LEDs that we want to blink (programmer can adjust this)
#define def 1000 //default delay time

//pin number for LEDs
#define LED1 8
#define LED2 2
#define LED3 3
#define LED4 4
#define LED5 5
#define LED6 6
//can add more (up to 13). I only plan on using 6

bool ledSelection = true;
bool intervalSelection = false;
int ledNum = 0; //use to store which led user selects.
int intervalInput = def; //used to store interval that the user enters


struct struct_led {
  bool isOn = false;
  uint8_t pinNumber;
  unsigned long delayInterval = 0;
  //millisDelay object which will be used to control and check the condition of led timer
  millisDelay delay; 
};

//container to hold our LEDs
struct struct_led ledContainer[numLEDs];
//intializing our task queue
void (*taskQueue[8])(); //array of function pointers. only have 7 tasks to do (blink leds and user input)
//last index of taskQueue will be null

//******function prototypes*****
void blinkLED(int ledNumber);
int getLED();
int getInterval();
void initiate(int ledIndex, int pinNum);//used to initialize led values

//task
void taskBlink1();
void taskBlink2();
void taskBlink3();
void taskBlink4();
void taskBlink5();
void taskBlink6();
void taskQuery();
//*****************************

void setup() {
  Serial.begin(9600);

  //initializes our 6 LEDs
  initiate(0, LED1);
  initiate(1, LED2);
  initiate(2, LED3);
  initiate(3, LED4);
  initiate(4, LED5);
  initiate(5, LED6);

  //assigning function to the taskqueue
  taskQueue[0] = taskQuery;
  taskQueue[1] = taskBlink1;
  taskQueue[2] = taskBlink2;
  taskQueue[3] = taskBlink3;
  taskQueue[4] = taskBlink4;
  taskQueue[5] = taskBlink5;
  taskQueue[6] = taskBlink6;
  taskQueue[7] = NULL;

  //program starts
  Serial.println("Which LED do you want to select (1, 2, 3, 4, 5, or 6): ");

}

void loop() {

  int index = 0;
  while(taskQueue[index] != NULL){
    (*taskQueue[index++])(); //note that we're using the postfix version of '++'
  }

}

void taskQuery(){//task wrapper for user input

  if (ledSelection){
    ledNum = getLED();

    if (ledNum != -1){//means user made a selection (keep in mind user input could be invalid. blink functions don't care!)

      //next we need to ask user for interval time so we set the following flag to true
      ledSelection = false;
      intervalSelection = true;
      Serial.println("Enter blinking interval in milliseconds: ");

    }

  } else if(intervalSelection){
    intervalInput = getInterval();

    if (intervalInput != -1){//only proceeds to next state if user has entered a delay time

      //we've gotten our delay time and can succesfully change the blink interval of the led denoted by the variable led num
      //so we restart the whole process by setting the ledSelection flag to true and the interval selection flag to false 
      //so that the user can select another LED
      intervalSelection = false;
      ledSelection = true;

      Serial.println("Which LED do you want to select (1, 2, 3, 4, 5, or 6): ");

    }
  }

}

void taskBlink1() {blinkLED(1);} //led at index 0
void taskBlink2() {blinkLED(2);} //led at index 1
void taskBlink3() {blinkLED(3);} //led at index 2
void taskBlink4() {blinkLED(4);} //led at index 3
void taskBlink5() {blinkLED(5);} //led at index 4
void taskBlink6() {blinkLED(6);} //led at index 5

void blinkLED(int ledNumber){

  //***************IMPORTANT!*******************
  //it should be noted that the index of the leds in ledContainer is 1 less the pin value
  //for example led 6 refers to the led at index 5 of the ledContainer
  //if this is not the case then expect unexpected behavior

  //ledNumber is name of Parameter
  //ledNum is global variable that stores user's LED selection
  int index = ledNumber - 1;
  int actualSelection = ledNum - 1; //same as the reason mentioned above

  //checks if timer has ended
  if (ledContainer[index].delay.justFinished()){

    ledContainer[index].delay.repeat(); //resets the timer

    //toggles the led
    ledContainer[index].isOn = !ledContainer[index].isOn;
    if (ledContainer[index].isOn == true){
      digitalWrite(ledContainer[index].pinNumber, HIGH);
    } else {
      digitalWrite(ledContainer[index].pinNumber, LOW);
    }

  }

  //checks if user selected this led
  if (index == actualSelection){ //same as if ledNumber == ledNum
    //checks if interval time is valid
    if (intervalInput >= 0){

      //sets new blink interval timer
      ledContainer[index].delay.finish();//forces timer to end
      ledContainer[index].delayInterval = intervalInput; //the delayInterval isn't actually necessary since the millisDelay object does all the heavy work
      ledContainer[index].delay.start(intervalInput); //sets a new timer

      //makes intervalInput an invalid value
      intervalInput = -1;
    }
  }


}

int getLED(){
  if (Serial.available() != 0){//means user entered a value onto the serial monitor
    return Serial.parseInt();
  } else {
    return -1;
  }
}

int getInterval(){
  if (Serial.available() != 0){//means user has entered a value onto the serial monitor
    return Serial.parseInt();
  } else {
    return -1;
  }
}

void initiate(int ledIndex, int pinNum){//only meant to be called in setup function

  /*initializes the led object in the ledContainer at the given index to the given 
  pin number and starts the blink timer. LED timer is set to the deault interval value
  and LED starts off in an off state.
  */
  
  ledContainer[ledIndex].delayInterval = def;
  ledContainer[ledIndex].isOn = false;
  ledContainer[ledIndex].pinNumber = pinNum;

  pinMode(ledContainer[ledIndex].pinNumber, OUTPUT);
  digitalWrite(ledContainer[ledIndex].pinNumber, LOW);

  ledContainer[ledIndex].delay.start(def);
}

