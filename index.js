var AWS = require('aws-sdk');
var nrf24=require("nrf24");
require('dotenv').config();

AWS.config.update({region: process.env.AWS_REGION});
var sqs = new AWS.SQS({apiVersion: '2012-11-05'});
var queueURL = process.env.AWS_QUEUE;

var queueParams = {
    MaxNumberOfMessages: 1,
    AttributeNames: [
       "SentTimestamp"
    ],
    MaxNumberOfMessages: 10,
    MessageAttributeNames: [
       "All"
    ],
    QueueUrl: queueURL,
    VisibilityTimeout: 20,
    WaitTimeSeconds: 20
};

var rf24= new nrf24.nRF24(parseInt(process.env.CE), parseInt(process.env.CS));
rf24.begin(true);
rf24.useWritePipe(process.env.PIPE,true);

function getMessage()
{
   sqs.receiveMessage(queueParams, function(err, data) {
        if (err) {
            console.log("Receive Error", err);
        } else if (data.Messages) {            
            let success = rf24.write(Buffer.from(data.Messages[0].Body));
            console.log("Sent " + ( success ? "OK" : "KO" ) + Buffer.from(data.Messages[0].Body));

            var deleteParams = {
            QueueUrl: queueURL,
            ReceiptHandle: data.Messages[0].ReceiptHandle
            };
            
            sqs.deleteMessage(deleteParams, function(err, data) {
                if (err) {
                    console.log("Delete Error", err);
                } else {
                    console.log("Message Deleted", data);
                }
            });
        }
        getMessage();
    });
}

getMessage();