
var  config = require('./config');
var serialport = require("serialport");
var AMQPClient = require('amqp10');

var connected = false;

var uri = 'amqps://' + encodeURIComponent(config.sasName) + ':' + encodeURIComponent(config.sasKey) + '@' + config.serviceBusHost;
var sendAddr = config.eventHubName;

var client = new AMQPClient(AMQPClient.policies.EventHubPolicy); // Uses PolicyBase default policy
client.connect(uri, function(conn_err) {
  if (conn_err)
  {
    console.log('Error connecting to Event Hub (' + conn_err + ')');
  }
  else
  {
    console.log('Connected to Event Hub');
    connected = true;
  }
});

var sp = new SerialPort(portName, {
  baudrate: 9600,
  dataBits: 8,
  parity: 'none',
  stopBits: 1,
  parser: serialport.parsers.readline("\n")
}, false);

sp.on('data', function(data) {
  var text = data.toString();
  //console.log(text);
  if (text.indexOf('{') == 0)
  {
    try {
       var sensorData = JSON.parse(text);
       sensorData.sensorId = 'arduino01';
       sendData(sensorData);
    }
    catch (e) {
       console.log('Error Parsing Data');
    }
  }
});
sp.open(function(err){
  if (err)
  {
    console.log('Error Opening Port ' + config.portName);
    sp.close();
  }
  else
  {
    console.log('Opened Port ' + config.portName);
  }
});

function sendData(sensorData)
{
  if (connected)
  {
      var partitionKey = '00';
      var message = JSON.stringify(sensorData);
      client.send(message, sendAddr,
                { 'x-opt-partition-key' : partitionKey },
                   function(err) {
                     if (err)
                       console.log(err);
                     else
                       console.log('Message Enqueued:' + message);
                }
             );
  }
}
