Description
------

Control Velleman K8061 interface board with node.js.

Requirements
------
* node.js
* libusb
* [Velleman K8061 USB interface board](http://www.velleman.eu/products/view/?id=364910)

Install
------
```bash
npm install git://github.com/unkernet/node-k8061.git
```

Sample
------
```javascript
var k8061 = require('node-k8061');
var count = k8061.count;
console.log('Found ' + count + ' devices');

if (count > 0) {
  try {
    var board = k8061.k8061(); // Open the first available device
  } catch (e) {
    console.error(e.message);
    process.exit();
  }
  board.analogOut = 128;       // Set all analog channels to the middle value
  board.analogOut[7] = 255;    // Set last analog channels to the maximum value
  setInterval(function() {
    board.digitalOut = board.digitalIn;
  }, 100);                     // Bind digital output to the digital input
}
```

Common issues
------

API
------
* `count` Number of connected boards.
* `k8061(id, byAddress)` Open the device with specified id by connection order or by address jumpers.

### k8061 object:
* `close()` Close the device.
* `hasPower` 1 if the board is connected to an external power source.
* `digitalIn` Array of digital input pins, 0 or 1.
* `digitalOut` Array of digital output pins, 0 or 1. Can be set to an array or a single value.
* `analogIn` Array of digital input pins, 0 to 1023.
* `analogOut` Array of digital output pins, 0 to 255. Can be set to an array or a single value.
* `pwmOut` [PWM](http://en.wikipedia.org/wiki/Pulse-width_modulation) output value, 0 to 1023.
