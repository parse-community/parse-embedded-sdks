#!/bin/sh

COUNTER=0
while [  $COUNTER -lt 10 ]; do
  curl -X POST \
    -H "X-Parse-Application-Id: OVwxCTeK2dukeQ2UiO6N7D0LtqvxO5db7fKittB6" \
    -H "X-Parse-REST-API-Key: S1xbOgTGkTdV8WM2t5xpO5QMPHQcPs7mREUoaDcW" \
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
