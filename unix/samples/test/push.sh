#!/bin/sh

COUNTER=0
while [  $COUNTER -lt 10 ]; do
  curl -X POST \
    -H "X-Parse-Application-Id: YOUR_APP_IP" \
    -H "X-Parse-REST-API-Key: YOUR_REST_API_KEY" \
    -H "Content-Type: application/json" \
    -d "{
          \"where\": {
            \"deviceType\": \"embedded\"
          },
          \"data\": {\"id\": \"$1\"}
        }" \
    https://api.parse.com/1/push
  let COUNTER=COUNTER+1 
done
