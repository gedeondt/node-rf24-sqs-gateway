'use strict'
const AWS = require('aws-sdk');

exports.handler = function (event, context, callback) {
  
  const savedPasswords = JSON.parse(process.env.PASSWORDS);
  const requestPassword = JSON.parse(event.body);

  if(savedPasswords[requestPassword['user']] &&
     savedPasswords[requestPassword['user']] == requestPassword.password)
  {
      var roleToAssume = {RoleArn: process.env.ROLE_ARN,
                      RoleSessionName: 'session1',
                      DurationSeconds: 900,};
      var roleCreds;
      
      // Create the STS service object    
      var sts = new AWS.STS({apiVersion: '2011-06-15'});
      
      //Assume Role
      sts.assumeRole(roleToAssume, function(err, data) {
          console.log(err, data);
            callback(null, {
              statusCode: 200,
              headers: {
                'Content-Type': 'application/json; charset=utf-8',
              },
              body: JSON.stringify(data.Credentials),
            });
      });
  }
  else
  {
    callback(null, { statusCode: 401, body: JSON.stringify(savedPasswords)+JSON.stringify(requestPassword)});
  }
}