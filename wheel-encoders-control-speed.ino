
/* 
fork from https://www.hackster.io/bdub/spark-sumo-bot-1c36d1
*/

#define STBY A3

class IRSensor
{
    int pin;
    
    int values[4];
    int counter;
    
    public:
    IRSensor(int _pin)
    {
        pin = _pin;
        counter =0;
    }
    
    void read()
    {
        if ( counter == arraySize(values)) {
            counter = 0;        
        }
        values[counter++] = analogRead(pin);
    }
    
    void setup()
    {
        pinMode(pin, INPUT);
    }
    
    double avg()
    {
        double result = 0.0;
        for (int i=0;i<arraySize(values);++i) {
          result += values[i];  
        }
        return result/arraySize(values);
    }
    
};

class Wheel
{
	int dirPin1;
	int dirPin2;
	int powerPin;
	String name;
	
  public:
  Wheel(String _name, int _dirPin1, int _dirPin2, int _powerPin)
  {
      
    name = _name;
	dirPin1 = _dirPin1;
	dirPin2 = _dirPin2;
	powerPin = _powerPin;
  }
  
  void Setup()
  {
	pinMode(dirPin1, OUTPUT);  
	digitalWrite(dirPin1, LOW);
	
	pinMode(dirPin2, OUTPUT);     
	digitalWrite(dirPin2, LOW);
	
	pinMode(powerPin, OUTPUT);     
	analogWrite(powerPin, 0);
  }

  void Forward(int _speed)
  {
    digitalWrite(dirPin1, HIGH);
    digitalWrite(dirPin2, LOW);
    analogWrite(powerPin, _speed);
  }
  
  void Backward(int _speed)
  {
    digitalWrite(dirPin1, LOW);
    digitalWrite(dirPin2, HIGH);
    analogWrite(powerPin, _speed);
  }
  
  void Stop()
  {
    analogWrite(powerPin, 0);  
  }
};

class WheelEncoder
{
  int pin;
  int counter;
  int lastCounter;
  
  public:
  WheelEncoder(int _pin)
  {
	pin = _pin;
  }
  
  void count()
  {
      counter++;
      lastCounter++;
  }
 
  int getCount()
  {
      return counter;
  }
  
  int speed()
  {
      return lastCounter;
  }
  
  void tick()
  {
      lastCounter = 0;
  }
  
  void resetCount()
  {
      counter = 0;
  }
  
   void setup()
  {
      
      pinMode(pin, INPUT_PULLUP);
      attachInterrupt(pin, &WheelEncoder::count, this, CHANGE);
    //   Timer tickTimer(1000, [this]() -> void{tick(); });
    //   tickTimer.start();
    // don't work but why?
  }
  
};

Wheel left("left", A1, A2, D1);
Wheel right("right", D5, D4, D0);

WheelEncoder leftEncoder(D2);
WheelEncoder rightEncoder(D3);

void tickEncoders()
{
     leftEncoder.tick(); 
     rightEncoder.tick();
}

Timer tickTimer(1000, tickEncoders);
      

void publishEncoders()
{
    // Particle.publish("encoders10", String(leftEncoder.getCount()) + "," + String(rightEncoder.getCount()) + ";" + String(leftEncoder.speed()) + "," + String(rightEncoder.speed()));
    tickEncoders();
}

Timer publishEncodersTimer(1000,publishEncoders);

int _duration = 0;

IRSensor leftSensor(A5);
IRSensor middleSensor(A6);
IRSensor rightSensor(A7);

float leftSpeed;
float rightSpeed;

void publish()
{
    // Particle.publish("left-middle-right", String(leftSensor.avg()) + "," + String(middleSensor.avg()) + "," + String(rightSensor.avg()));
    Particle.publish("speeds - real vs commanded", String(leftEncoder.speed()) + "," + String(rightEncoder.speed()) + " ; " + String(leftSpeed) + "," + String(rightSpeed));
    
}

void readAll()
{
    leftSensor.read();
    middleSensor.read();
    rightSensor.read();
}

Timer publishTimer(1000, publish);
Timer readAllTimer(5, readAll);

void setup() {
    
    leftEncoder.setup();
    rightEncoder.setup();
    
    leftSensor.setup();
    middleSensor.setup();
    rightSensor.setup();
    
    // tickTimer.start();
    // readAllTimer.start();
    publishTimer.start();
    publishEncodersTimer.start();
    
    pinMode(STBY, OUTPUT);
    digitalWrite(STBY, HIGH);
    
    left.Setup();
    right.Setup();
    
    
    
}


void stopMotors()
{
    left.Stop();
    right.Stop();
}



void trySpeeds()
{
    float baseSpeed = 150;
    float leftRealSpeed = (float) leftEncoder.speed();
    float rightRealSpeed = (float) rightEncoder.speed();
    
    float diff = leftRealSpeed - rightRealSpeed;
    float fraction = abs(diff)/(leftSpeed + rightSpeed);
    float positiveFactor = 1.0f + fraction;
    float negativeFactor = 1.0f - fraction;
    if (diff >0) {
        leftSpeed = baseSpeed * negativeFactor;
        rightSpeed = baseSpeed * positiveFactor;
    } else if (diff < 0.0f) {
        rightSpeed = baseSpeed * negativeFactor;
        leftSpeed = baseSpeed * positiveFactor;
    } else {
        leftSpeed = baseSpeed;
        rightSpeed = baseSpeed;
    }
    
    delay(3);
}

void loop() 
{ 
    
    trySpeeds();
    
    left.Backward(leftSpeed);
    right.Backward(rightSpeed);
    
    int secondWheelSpeed = 30;
    int followingSpeed = 160;
    int threshold = 250;
    
    // if (rightSensor.avg() - leftSensor.avg() > threshold) {
    //     left.Forward(secondWheelSpeed);
    //     right.Forward(followingSpeed);
    // } else if (leftSensor.avg() - rightSensor.avg() > threshold) {
    //     right.Forward(secondWheelSpeed);
    //     left.Forward(followingSpeed);   
    // } else {
    //     left.Forward(followingSpeed);
    //     right.Forward(followingSpeed);
    // }
    // delay(3);
    
}
