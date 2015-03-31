/*
  Uses serialport
  https://github.com/voodootikigod/node-serialport
  npm install serialport
*/
var portName = '/dev/ttyACM0';
var serialport = require("./node_modules/serialport");
var SerialPort = serialport.SerialPort;
var sp = new SerialPort(portName, {
  baudrate: 9600,
  dataBits: 8,
  parity: 'none',
  stopBits: 1,
  parser: serialport.parsers.readline("\n")
}, false);

sp.on('data', function(data) {
  var text = data.toString();
  console.log(text);
  if (text.indexOf('{') == 0)
  {
    var  msg = JSON.parse(text);
    console.log('Humidity ' + msg.humidity);
  }
});
sp.open(function(err){
  if (err)
  {
    console.log('Error Opening Port ' + portName);
    sp.close();
  }
  else
  {
    console.log('Opened Port ' + portName);
  }
});
