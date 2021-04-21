'use strict'
const AWS = require('aws-sdk');

exports.handler = function (event, context, callback) {
  
  let savedPasswords;
  try {
    savedPasswords = JSON.parse(process.env.PASSWORDS);
  } catch (e) {
    callback(null, { statusCode: 503, body: "No passwords configured" });
    return;
  }
  
  let requestObject, user, password;
  try {
    requestObject = JSON.parse(event.body);
    user = requestObject['user'];
    password = requestObject['password'];
  } catch (e) {
    callback(null, { statusCode: 400, body: "Check json:"+e });
    return;
  }

  if(user && savedPasswords[user] == password)
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
    callback(null, { statusCode: 401, body: "Bad credentials"});
  }
}