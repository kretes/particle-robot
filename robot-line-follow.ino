/* 
fork from https://www.hackster.io/bdub/spark-sumo-bot-1c36d1
*/

#define STBY A3

class IRSensor
{
    int pin;
    
    int values[5];
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
    digitalWrite(dirPin1, LOW);
    digitalWrite(dirPin2, HIGH);
    analogWrite(powerPin, _speed);
  }
  
  void Backward(int _speed)
  {
    digitalWrite(dirPin1, HIGH);
    digitalWrite(dirPin2, LOW);
    analogWrite(powerPin, _speed);
  }
  
  void Stop()
  {
    analogWrite(powerPin, 0);  
  }
};

Wheel left("left", A1, A2, D1);
Wheel right("right", D5, D4, D0);

int _duration = 0;

IRSensor leftSensor(A5);
IRSensor middleSensor(A6);
IRSensor rightSensor(A7);

void publish()
{
    Particle.publish("left-middle-right", String(leftSensor.avg()) + "," + String(middleSensor.avg()) + "," + String(rightSensor.avg()));
}

void readAll()
{
    leftSensor.read();
    middleSensor.read();
    rightSensor.read();
}

Timer publishTimer(1000, publish);
Timer readAllTimer(20, readAll);

void setup() {
    
    leftSensor.setup();
    middleSensor.setup();
    rightSensor.setup();
    
    readAllTimer.start();
    publishTimer.start();
    
    pinMode(STBY, OUTPUT);
    digitalWrite(STBY, HIGH);
    
    left.Setup();
    right.Setup();
    
    left.Forward(200);
    right.Forward(200);
    delay(100);
    left.Stop();
    right.Stop();
}


void stopMotors()
{
    left.Stop();
    right.Stop();
}

void loop() 
{ 
    
    int followingSpeed = 140;
    int threshold = 300;
    
    if (rightSensor.avg() - leftSensor.avg() > threshold) {
        left.Stop();
        right.Backward(followingSpeed);
    } else if (leftSensor.avg() - rightSensor.avg() > threshold) {
        right.Stop();
        left.Backward(followingSpeed);   
    } else {
        left.Backward(followingSpeed);
        right.Backward(followingSpeed);
    }
    delay(5);
    
}
