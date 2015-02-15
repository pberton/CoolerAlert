/*
  Uses serialport
  https://github.com/voodootikigod/node-serialport
  npm install serialport
*/
var serialport = require("../serialport");
var SerialPort = serialport.SerialPort;
var sp = new SerialPort("COM3", {
  baudrate: 9600,
  parser: serialport.parsers.raw
}, true);
sp.on('open', function(){
  console.log('Serial Port Opened');
  serialport.on('data', function(data){
    console.log(data.toString());
  });
});
