#define trigPinIn D2
#define echoPinIn D3
#define trigPinOut D6
#define echoPinOut D7

float lastInDistance = 0, lastOutDistance = 0, inDistance = 0, outDistance = 0;

float measureDistance(byte trigPin, byte echoPin);

void setup()
{
  Serial.begin(1000000);
  pinMode (trigPinIn, OUTPUT);
  pinMode (echoPinIn, INPUT);
  pinMode (trigPinOut, OUTPUT);
  pinMode (echoPinOut, INPUT);

  inDistance = measureDistance(trigPinIn, echoPinIn);
  outDistance = measureDistance(trigPinOut, echoPinOut);
  if(lastInDistance == 0 || lastOutDistance == 0) //Prevent wrong trigger after boot
  {
    lastInDistance = inDistance;
    lastOutDistance = outDistance;
  }
}

void loop()
{
  inDistance = measureDistance(trigPinIn, echoPinIn);
  outDistance = measureDistance(trigPinOut, echoPinOut);
}

float measureDistance(byte trigPin, byte echoPin)
{
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return (pulseIn(echoPin, HIGH) * 0.034 / 2);
}
