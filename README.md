# SerialComm

# About

This arduino library simplify serial communications between arduinos, or between an arduino baord and a python script (https://github.com/Tahitibob35/pySerialComm).
- Attach actions to function
- Simply send and receive strings or integer
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
  s.attach(2, remoteAnalogWrite);
}
```

Python sender script
```python
pin = 9
value = 120
resp = ard.sendmessage(2, (pin,value), ack=False)
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
pin = 5
resp = ard.sendmessage(2, (i,), ack=False)
values = ard.parsedata("s", resp)
pin_value = values[0]
```


