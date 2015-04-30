var static = require('node-static');
var http = require('http');
var file = new(static.Server)();
var app = http.createServer(function (req, res) {
  file.serve(req, res);
}).listen(2013);


// var express = require('express');
// var app = express();
// console.log(express.static(__dirname + '/js'));
// app.use(express.static(__dirname + '/js'));
// app.all('*', function(req, res){
// 	res.sendfile("index.html");
// });

// app.listen(9000);

var io = require('socket.io').listen(app);

var serialPort = require("serialport");
var comPort = "";
var botSer;
serialPort.list(function (err, ports) {
  ports.forEach(function(port) {
    console.log(port.comName);
    console.log(port.pnpId);
    console.log(port.manufacturer);
	if (port.manufacturer.indexOf("Arduino") > -1) {
		comPort = port.comName;
		console.log("COM Port is: " + comPort);
	}
  });
});

io.sockets.on('connection', function (socket){
	
	if(!botSer && comPort != "") {
		botSer = new serialPort.SerialPort(comPort, {
		  baudrate: 115200,
		  parser: serialPort.parsers.readline("\n")
		});

		botSer.on("data", function (data) {
			console.log("Bot says: "+data);
			if (socket) {
				var d = data.split(":");
				if(d[0] == 'W') {
					if(d[1] == 'D') {
						m = { cmd: 'dir', data: d[2] }; 
						socket.emit('robot', m);
						socket.broadcast.emit('robot', m);
					}
					if(d[1] == 'S') {
						m = { cmd: 'speed', data: d[2] }; 
						socket.emit('robot', m);
						socket.broadcast.emit('robot', m);
					}
				}
			}
		});
	}

	function log(){
		var array = [">>> "];
	  for (var i = 0; i < arguments.length; i++) {
	  	array.push(arguments[i]);
	  }
	    socket.emit('log', array);
	}

	socket.on('message', function (message) {
		log('Got message: ', message);
		socket.broadcast.emit('message', message); // should be room only
	});
	
	socket.on('robot', function (data) {
		console.log(data);
		socket.broadcast.emit('robot', data);
		socket.emit('robot', data);
		if(comPort == "") {
			console.log("Bot not found");
		} else {
			if (data['cmd'] == "dir") {
				botSer.write("M" + data['data']);
			}
			if (data['cmd'] == "speed") {
				botSer.write("S" + data['data']);
			}
		}
	});

	socket.on('create or join', function (room) {
		var numClients = io.sockets.clients(room).length;

		log('Room ' + room + ' has ' + numClients + ' client(s)');
		log('Request to create or join room', room);

		if (numClients == 0){
			socket.join(room);
			socket.emit('created', room);
		} else if (numClients == 1) {
			io.sockets.in(room).emit('join', room);
			socket.join(room);
			socket.emit('joined', room);
		} else { // max two clients
			socket.emit('full', room);
		}
		socket.emit('emit(): client ' + socket.id + ' joined room ' + room);
		socket.broadcast.emit('broadcast(): client ' + socket.id + ' joined room ' + room);

	});

});

