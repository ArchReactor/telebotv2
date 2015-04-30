# Telebotv2

WebRTC based telepresense bot heavily based on https://bitbucket.org/webrtc/codelab.  can run "off-line", no internet needed.

Dependancies:
 - nodejs modules: socket.io, node-static, and serialport.  
 - arduino libraries: Arduino's SPI and Servo,  RobotCore (included), and IRremote (https://github.com/shirriff/Arduino-IRremote)
 - - IRremote is optional, you'll have to comment out the code though.  not having an IR revceiver connected will not affect operation
 
Hadrware:
 - power wheelchair base
 - any arduino
 - motor controller (RobotCore is configurable for servo based or H-bridge)
 - a PC that can run nodejs and Chrome or Firefox
 - optional: IR remote

Installation:  internet is required durring installation, but not to run the code
 - download repo
 - copy RobotCore to your arduino libraries folder
 - install arduino SPI, Servo, and IRremote libraries as per thier instructions
 - copy telebot folder to your skechhbook
 - edit telebot scketch to reflect your pin choices for the motor controller.  if using h-bridge see RobotCore's examples and edit as needed.
 - upload sketch
 - cd to nodejs/server folder
 - install node modules (npm install socket.io node-static serialport)
 - if you do not have an offical Arduino you'll need to edit server.js to set the USB detection to match what you have
 - run server with command: 
```sh
$ node server.js
```
 - load web page at your IP and port 2013, such as http://localhost:2013
 - connect from a 2nd device for remote driving
 
NOTES:
 - for testing purposes, the server code will happily run without an arduino, and you'll see the "move" commands in the console.  it will not auto-detect if you plug an arduino in later.
