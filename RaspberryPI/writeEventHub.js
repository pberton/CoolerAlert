/*
  Uses amqp10
  https://github.com/noodlefrenzy/node-amqp10
  npm install amqp10
*/
var AMQPClient = require('amqp10');

var sasName = 'sendGateway';
var sasKey = 'q4jErQcumSBJbgEx29YdE5cdZVul+LRqSXvxKzXnAx8=';
var serviceBusHost = 'weatherdata.servicebus.windows.net/weatherData';
var eventHubName = 'weathersensordata';
var connected = false;

var uri = 'amqps://' + encodeURIComponent(sasName) + ':' + encodeURIComponent(sasKey) + '@' + serviceBusHost;
var sendAddr = eventHubName;

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


    var value = '{ "DataString": "From Node", "DataValue": "value" }';
    sendValue(value);
  }
});


function sendValue(value)
{
    if (connected)
        client.send(value, sendAddr,
                  { 'x-opt-partition-key' : 'pk1' },
                  function(err) {
                      if (err)
                        console.log(err);
                      else
                        console.log('Message Enqueued');
                  }
               );
}
