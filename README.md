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
SerialComm s( Serial );

void remoteAnalogWrite( void ) {
    int pin = 0;
    int value = 0;
    s.getData( "ii" , &pin , &value );
    analogWrite( pin , value );
}

void setup( ) {
    ...
    s.attach( 2 , remoteAnalogWrite );
    ....
}

void loop() {
    s.check_reception();
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
SerialComm s ( Serial );

void remoteAnalogRead( void ) {
    int pin;
    s.getData( "i" , &pin );
    int value = analogRead( pin );
    s.sendAck( "i" , value );
}

void setup( ) {
  s.attach( 2 , remoteAnalogRead );
}

void loop() {
    s.check_reception();
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
SerialComm s ( Serial );
char myname[] = "Arduino receiver";

void sendMyName( void ) {
    s.sendAck( "s" , myname );
}

void setup( ) {
    ...
    s.attach( 2, sendMyName );
    ...
}

void loop() {
    s.check_reception();
}

```

### Arduino sender code

```c
SerialComm s ( Serial );

void loop( ) {
    ...
    s.sendMessage( 2 , true ) ) {
    char a_string[20] = "";
    if ( s.sendMessage( 2 , true ) ) {
        s.getData( "s" , &a_string , sizeof( a_string ) );
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
```

## Remote digitalRead

```c
int  rDigitalRead( uint8_t pin );
```

## Remote analogRead

```c
int  rAnalogRead( uint8_t pin );
```

## Remote analogWrite

```c
void rAnalogWrite( uint8_t pin , int value);
```

## Remote digitalWrite

```c
void rDigitalWrite( uint8_t pin , int value);
```

## Get the complete state of a digital pin

```c
void rdigitalPinState( int pin , int * rw , int * pwm_cap , int * pwm_enabled , int * value);
```

rw : Writable or not
pwm_cap : PWM capable pin
pwm_enabled : PWM enabled or not
value : Current value (LOW, HIG, 0-255)

## Attach an action id to a callback function

```c
bool attach(int command, void (*ptrfonction)(void));
```

## Sending a message without values

```c
s.sendMessage( 2 , false );
```

The second argument indicates if an ack is awaited.
The method returns true if an ck has been received.

## Sending a message with an integer

```c
s.sendMessage( 2 , false, "i", 5 );
```

"i" means for an integer.

## Sending a message with two integer

```c
s.sendMessage( 2 , false, "ii", 5, 2000 );
```

## Sending a message with a string

```c
s.sendMessage( 2 , false, "s", a_string );
```
"s" means for an string.

## Sending a message with two strings

```c
s.sendMessage( 2 , false, "ss", a_stringA, a_stringB );
```

## Sending a message with an integer and a string

```c
s.sendMessage( 2 , false, "is", an_integer,  a_string );
```

## Sending a message with three integers and three string

```c
s.sendMessage( 2 , false, "iiisss", an_integerA, an_integerB, an_integerC, a_stringA, a_stringB, a_stringC );
```

## Check for incoming messages (in loop)

```c
if (mySerial.available()) {
    s.check_reception();
}
```

## Extract an integer from an incoming message

```c
s.getData( "i" , &an_integer );
```

This method can be used in the callback function of after sending a message if an ack has been specified.

## Extract two integers from an incoming message

```c
s.getData( "ii" , &an_integerA, &an_integerB );
```

## Extract a string from an incoming message

```c
s.getData( "s" , &a_string, sizeof(a_string) );
```

## Extract a string from an incoming message

```c
s.getData( "ss" , &a_stringA, sizeof(a_stringA) , &a_stringB, sizeof(a_stringB) );
```

## Extract a string and an integer from an incoming message

```c
s.getData( "si" , &a_string, sizeof(a_string), &an_integer );
```

## Extract a string, an integer, a string and an integer from an incoming message

```c
s.getData( "sisi" , &a_stringA, sizeof(a_stringA), &an_integerA , &a_stringB, sizeof(a_stringB), &an_integerB );
```

## Send an ack without data

```c
s.sendAck( );
```

This method is used in the callback function.

## Send an ack with an integer

```c
s.sendAck( "i" , 5 );
```

## Send an ack with a string

```c
s.sendAck( "s" , a_string );
```
## Send an ack with a string and an integer

```c
s.sendAck( "ss" , a_string , an_integer );
```


