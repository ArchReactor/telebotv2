# Telebotv2

WebRTC based telepresense bot heavily based on https://bitbucket.org/webrtc/codelab.  can run "off-line", no internet needed.

Dependancies:
 - nodejs modules: socket.io, node-static, and serialport.  
 - arduino libraries: Arduino's SPI and Servo (included with Arduino),  RobotCore (included in telebotv2), and IRremote (https://github.com/shirriff/Arduino-IRremote)
 - - IRremote is optional, you'll have to comment out the code though.  not having an IR revceiver connected will not affect operation even with the code enabled
 
Hadrware:
 - power wheelchair base
 - any arduino
 - motor controller (RobotCore is configurable for servo based or H-bridge)
 - a PC that can run nodejs and Chrome or Firefox.
 - optional: IR remote

Installation:  internet is required during installation, but not to run the code
- install nodejs and npm (node package manager) using whatever method is recomended for your OS.
 - Debian and Ubuntu based distros:  the "node" binary is also used by another lesser used package, so you need to install the nodejs-legacy package so that modules that use the old command work.
   - sudo apt-get install nodejs-legacy npm
 - RaspberryPi/Rasbian, and possibly any Debian Wheezy:  the included Nodejs is too old.  Follow the instructions in the RaspberryPi section below
- download telebotv2 repo
 - option 1: click the Download Zip button on the github project page
 - option 2: use git to download it.  git clone https://github.com/ArchReactor/telebotv2
   - if you use option 2 you can use git to keep a version history of your local changes and also merge in updates later.  See git's documentation for more info.
- open a command prompt or terminal and switch to the server folder: cd telebotv2/nodejs/server (use \ instead of / on Windows)
- install node modules (npm install socket.io node-static serialport)
- Optional: setup the Arduino using the instructions below.  
- run server with command: 
```sh
$ node server.js
```
- load web page at your IP and port 2013, such as http://localhost:2013
- connect from a 2nd device

Ardunio setup: if you just want to try the video chat portion you may skip this
- make sure the server.js is not running or you will not able to program it
- copy RobotCore to your arduino libraries folder
- install arduino SPI, Servo, and IRremote libraries as per thier instructions
- copy telebot folder to your skechhbook
- edit telebot scketch to reflect your pin choices for the motor controller.  if using h-bridge see RobotCore's examples and edit as needed.
- upload sketch
- start server.js as above
- if you do not have an offical Arduino you may need to edit the serial open part of server.js to detect it.

Raspberry Pi Linux:

- Download and install node.js:

```bash
   wget http://nodejs.org/dist/v0.10.16/node-v0.10.16-linux-arm-pi.tar.gz
   tar xvfz node-v0.10.16-linux-arm-pi.tar.gz
   sudo mv node-v0.10.16-linux-arm-pi /opt/node/
```
- Once installed you may delete node-v0.10.16-linux-arm-pi.tar.gz to save space
- Set up your paths correctly (do this for pi and root and whatever other user you want to use Node.js):

```bash
   echo 'export PATH="$PATH:/opt/node/bin"' >> ~/.bashrc
   source ~/.bashrc
```
I have had some difficulty in repeating this on a clean install.  Copying folders from the sucessful one to the unsuccessful one does result it in it working, so something in npm may be breaking it and maybe be fixed later.

NOTES:
- for testing purposes, the server code will happily run without an arduino, and you'll see the "move" commands in the console.  it will not auto-detect if you plug an arduino in later.
- if the 2nd device reconnects it'll sometimes takes 2 tries to reconnect.  If the initial device disconnections, you must close both browser tabs and reconnect the "first" device first.  usually the "first" device is the robot's device.
- it is possible to run the node server on a 3rd device.  RaspberyPi has been tested with node 0.10.16.  newer versions may not work due to issues with the SerialPort module.
 
