'use strict';

const Protocol = require('azure-iot-device-mqtt').Mqtt;
const Client = require('azure-iot-device').ModuleClient;
const Message = require('azure-iot-device').Message;
const express = require('express');

let iotEdgeHubClient;

function startIotEdgeHubClient() {
  Client.fromEnvironment(Protocol, function (err, client) {
    if (err) {
      throw err;
    } else {
      client.on('error', function (err) {
        throw err;
      });
  
      console.log(`Connecting to IoTHub.`);
      client.open(function (err) {
        if (err) {
          throw err;
        } else {
          console.log('IoT Hub module client initialized');
          iotEdgeHubClient = client;
          client.on('inputMessage', onInputMessage);
          client.onMethod('ChangeLed', onDirectMethodChangeLed);

          console.log(`Configuring module twin handlers.`);
          client.getTwin((err, twin) => {
            if (err) {
              console.error(JSON.stringify(err));
            } else {
              twin.on('properties.desired', updateModuleConfiguration);
              console.log(`Module twin handlers configured.`);
            }
          });   

          updateModuleConfiguration();
          startExpressServer();
        }
      });
    }
  });  
}

function updateModuleConfiguration(delta) {
  console.log(`Module twin modified properties: ${JSON.stringify(delta)}.`);
}

function onInputMessage(inputName, msg) {
  console.log(`Received message ${JSON.stringify(msg)}.`);
  /* this points to iotEdgeHubClient */
  this.complete(msg, function(err, data) {});
}

function onDirectMethodChangeLed(request, response) {
}

function pipeMessage(client, inputName, messagePayload) {
  if (inputName === 'express') {
    messagePayload.timestamp = new Date().getTime();
    const message = new Message(JSON.stringify(messagePayload));
    message.properties.add('MessageType', 'face-match');
    client.sendOutputEvent('output1', message, function(err, data) {
      if (err) throw err;
      console.log(`Message sent (${JSON.stringify(data).substring(0,80)}...).`);
    });
  }
}

function startExpressServer() {
  const app = express()

  app.use(express.json())
  app.use(express.urlencoded({ extended: true }))
  
  app.post('/', async (req, res) => {
    if (iotEdgeHubClient) pipeMessage(iotEdgeHubClient, 'express', req.body);
    return res.status(200).send('Message accepted.');
  });
  
  app.listen(3000, () => console.log('Listening on port 3000!'));  
}

startIotEdgeHubClient();