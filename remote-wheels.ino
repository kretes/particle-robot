/* 
fork from https://www.hackster.io/bdub/spark-sumo-bot-1c36d1
    D0 - PWMB (speed right)
    A1 - BIN1 (direction bits)
    A2 - BIN2
    
    A3 - STBY (motor enable)
    
    A4 - AIN1 (direction bits)
    A5 - AIN2
    D1 - PMWA (speed left)
*/

SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);

#define STBY A3

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
Wheel right("right", A5, A4, D0);

int _duration = 0;

void setup() {
    
    pinMode(STBY, OUTPUT);
    digitalWrite(STBY, HIGH);
    
    left.Setup();
    right.Setup();
    
    Particle.function("cmd",cmdRobot);
    
    left.Forward(200);
    right.Forward(200);
    delay(100);
    left.Stop();
    right.Stop();
}

void loop() {
    
    if (_duration > 0) {
        Particle.publish("waiting for " + String(_duration));
        delay(_duration);
        left.Stop();
        right.Stop();
        Particle.publish("waited ");
        _duration = 0;
    }
}

int cmdRobot(String c) {
    // c format: x,yyy,zzz,
    // x = direction (f/b/l/r)
    // y = speed in 0-255
    // z = duration in milliseconds
    //
    // fblr, forward, backward, left turn, right turn
    
    Particle.publish("command received " + c);
    
    char command = c.charAt(0);
    String speedString = c.substring(2,c.indexOf(',',2));
    int speed = speedString.toInt();
    int duration = c.substring(2+speedString.length()+1).toInt();
    
    Particle.publish("parsed command as c = " + String(command) + " speed = " + String(speed) + " duration = " + String(duration));
    
    if (command == 'f') {
        left.Forward(speed);
        right.Forward(speed);
    }
    else if (command == 'b') {
        left.Backward(speed);
        right.Backward(speed);
    }
    else if (command == 'l') {
        left.Forward(speed);
        right.Backward(speed);
    }
    else if (command == 'r') {
        left.Backward(speed);
        right.Forward(speed);
    }
    
    _duration = duration;
    
    return 0;
}
