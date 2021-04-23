'use strict'
const AWS = require('aws-sdk');

exports.handler = function (event, context, callback) {
  
  let savedPasswords;
  try {
    savedPasswords = JSON.parse(process.env.PASSWORDS);
  } catch (e) {
    callback(null, cors({ statusCode: 503, body: "No passwords configured" }));
    return;
  }
  
  let requestObject, user, password;
  try {
    requestObject = JSON.parse(event.body);
    user = requestObject['user'];
    password = requestObject['password'];
  } catch (e) {
    callback(null, cors({ statusCode: 400, body: "Check json:"+e }));
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
            callback(null, cors({
              statusCode: 200,
              headers: {
                'Content-Type': 'application/json; charset=utf-8',
              },
              body: JSON.stringify(data.Credentials),
            }));
      });
  }
  else
  {
    callback(null, cors({ statusCode: 401, body: "Bad credentials"}));
  }
}

function cors(response)
{
  return {...response,
  "headers": {
    "Access-Control-Allow-Methods":"DELETE,GET,HEAD,OPTIONS,PATCH,POST,PUT",
    "Access-Control-Allow-Origin":"*",
    "Access-Control-Allow-Headers ":"Content-Type,X-Amz-Date,Authorization,X-Api-Key,X-Amz-Security-Token"
  }}
}