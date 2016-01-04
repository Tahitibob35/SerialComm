# SerialComm

# About

This arduino library simplify serial communications between arduinos, or between an arduino baord and a python script (https://github.com/Tahitibob35/pySerialComm).
- Attach actions to function
- Simply send and receive strings or integer
- Manage acknowledgments

# Examples

## 1 

Arduino receiver code
```c
SerialComm s(Serial);

void remoteAnalogRead( void ) {
    int value;
    s.getData("i", &value);
}

void setup() {
  s.attach(2, actionB);
}
```

Python sender script
```python
resp = ard.sendmessage(2, (i,), ack=False)
```

