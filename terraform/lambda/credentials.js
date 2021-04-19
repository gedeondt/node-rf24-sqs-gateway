'use strict'
const AWS = require('aws-sdk');

exports.handler = function (event, context, callback) {
  var response = {
    statusCode: 200,
    headers: {
      'Content-Type': 'text/html; charset=utf-8',
    },
    body: '<h2>Hello world...Welcome to AWS...</h2>',
  }

  var roleToAssume = {RoleArn: process.env.ROLE_ARN,
                      RoleSessionName: 'session1',
                      DurationSeconds: 900,};
  var roleCreds;
  
  // Create the STS service object    
  var sts = new AWS.STS({apiVersion: '2011-06-15'});
  
  //Assume Role
  sts.assumeRole(roleToAssume, function(err, data) {
      console.log(err, data);
      callback(null, response);
  });
  
}