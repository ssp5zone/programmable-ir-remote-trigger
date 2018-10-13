# Microchip's PIC based Programmable IR Remote Reader/Trigger

A device that remembers the last IR signal that was taught to it and triggers some action when the same signal is received again.

To teach it, 
1. Start the device.
2. Point any IR remote (TV/AC/Toy etc.) to the reader.
3. Press a button till read-completed LED glows.

To use it,
1. Put your code below while burning to device,
```C
void doStuff() {
	// do whatever you want here
}
```
2. Press the same button on the remote again.
