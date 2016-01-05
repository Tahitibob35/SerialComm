# SerialComm

# About

This arduino library simplifies serial communications between arduinos, or between an arduino board and a python script (https://github.com/Tahitibob35/pySerialComm).

# Features

- Attach actions to functions
- Simply send and receive strings or integers
- Manage acknowledgments

# Examples

## 1 - Python -> Arduino, without ack

Arduino receiver code
```c
SerialComm s(Serial);

void remoteAnalogWrite( void ) {
    int pin = 0;
    int value = 0;
    s.getData("ii", &pin, &value);
    analogWrite(pin, value);
}

void setup() {
    ...
    s.attach(2, remoteAnalogWrite);
    ....
}
```

Python sender script
```python
arduino = SerialComm('/dev/ttyUSB0', baudrate=115200)
pin = 9
value = 120
resp = arduino.sendmessage(2, (pin, value), ack=False)
```

## 2 - Python -> Arduino, with ack

### Arduino receiver code

```c
SerialComm s(Serial);

void remoteAnalogRead( void ) {
    int pin;
    s.getData("i", &pin);
    int value = analogRead(pin);
    s.sendAck(s.getId() , "i", value);
}

void setup() {
  s.attach(2, remoteAnalogRead);
}
```

### Python sender script

```python
arduino = SerialComm('/dev/ttyUSB0', baudrate=115200)
pin = 5
resp = arduino.sendmessage(2, (i,), ack=True)
values = arduino.parsedata("i", resp)
pin_value = values[0]
```

## 3 - Arduino -> Arduino, with ack

### Arduino receiver code

```c
SerialComm s(Serial);
char myname[] = "Arduino receiver";

void sendMyName( void ) {
    s.sendAck(s.getId() , "s", myname);
}

void setup() {
    ...
    s.attach(2, sendMyName);
    ...
}
```


### Arduino sender code

SerialComm s(Serial);

void loop() {
    ...
    s.sendMessage( 2 , true) ) {
    char a_string[20] = "";
    if ( s.sendMessage( 2 , true) ) {
        s.getData( "s" , &a_string, sizeof(a_string));
    }
    ...
}

```

# Samples

## Sample1

This sample communicates with the python script sample1.py (https://github.com/Tahitibob35/pySerialComm).

## SoftwareSerial1 and SoftwareSerial2

These samples communicate together using the SoftwareSerial library.

# Quick documentation

## Initialization

```c
SerialComm s(Serial);  // Use hardware serial port
void setup() {
    Serial.begin(9600);
}
```

## Attach an action id to a callback function

```c
bool attach(int command, void (*ptrfonction)(void));
```

## Sending a message without values

```c
s.sendMessage( 2 , true);
```

## Sending a message with an integer

```c
s.sendMessage( 2 , true, "i", 5);
```

## Sending a message with two integer

```c
s.sendMessage( 2 , true, "ii", 5, 2000);
```

## Sending a message with a string

```c
s.sendMessage( 2 , true, "s", &a_string, sizeof(a_string));
```

## Sending a message with two strings

```c
s.sendMessage( 2 , true, "ss", &a_stringA, sizeof(a_stringA), &a_stringB, sizeof(a_stringB));
```

## Sending a message with an integer and a string

```c
s.sendMessage( 2 , true, "is", an_integer, &a_stringB, sizeof(a_stringB));
```
