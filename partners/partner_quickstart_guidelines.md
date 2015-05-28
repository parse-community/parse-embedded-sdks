# Parse Quickstart Guidelines for IoT Partners

This document is for partner platform owners that want to make the Parse Embedded SDK run on their board.

## Overview

The Quickstart (QS) is one of the most important documents at Parse. It is a canonical guide that gets the developer up and running with Parse on a particular platform.  You will be creating a quickstart guide for your Github repo fork's README.  This document assumes that you have already read [Partner Platform Instructions](/partners/partner_platform_instructions.md).

There are a few audiences that our QS serves:

1. **New Developers** - For people who just signed up with Parse, they will most likely head to the QS to get going with their favorite platform, and also to discover what is possible with Parse.
2. **Existing Developers** - The QS is also the fastest way to try out a new feature in Parse, or to simply start a new project. Even seasoned Parse developers will use this flow.

Principles

* **Completeness** - After the developer goes through every step, they should have a working app on the platform that saves an object, and that can receive a push notification.
* **Succinct** - It should not contain anything superfluous from the completeness goal. That's what the full Guide is for.
* **The High Five Moment.** - The developer should be forced to paste in some code, compile, and see something work (like saving an object). This gives the developer a “high five” moment.

## Prerequisites 

Before writing your Quickstart guide, you will need the following:

* A Parse SDK (Embedded C / Arduino) that works on your board/platform.
* A working project in your IDE that integrates the Parse SDK.

## Templates

Use the following templates as you create your QS. 

There are two templates: 

1. New Projects: For devs that want to download a blank project that already has Parse integrated. 
2. Existing Projects: For devs that have an existing project and want to integrate Parse into it.

### New Project Template

**1. Setup the Environment**

* Instructions on getting the development environment setup for your board/platform.
* If you already have a guide for this online, you can link to it. Ideally, this guide only has what is necessary to get the environment setup, and no more. Explain any caveats or exceptions.

**2. Project File with Parse Integration**

* Please provide a link for downloading a project file that has the Parse SDK integrated and ready for the developer to code against.

**3. Loading and Prepping the Project**

* Provide instructions on how to load the project. This may be as simple as “click on the file to open it in the IDE.”
* Provide any additional instructions to setup the project once opened. Examples:
    * Connecting the device to WiFi.
    * Setting build targets and options.
    * Jumper configuration.
* At the end of this, the developer should be able to successfully build the project and also flash it onto the device. But, nothing should really happen — this is just a blank project.

**4. Code for Saving a Parse Object**

* Provide the exact code needed to save a Parse Object using the Parse SDK for your platform.
* This forces the developer to copy and paste it into the location.
* They can now build and run the project and observe that an object is saved.
* Please direct the developer the Parse.com Data Browser, where they can see their saved object.

**5. Code for Receiving a Push Notification**

* Provide the exact code needed to receive a Parse Push Notification using the Parse SDK for your platform to make an LED turn on for a duration of time, and turn off.
* This forces the developer to copy and paste it into the location.
* They can now build and run the project and send a push notification to the device to see an LED blink.
* To test push receiving, please instruct the developer to send a push using this curl command:
```
curl -X POST \
  -H "X-Parse-Application-Id: YOUR_PARSE_APPLICATION_ID" \
  -H "X-Parse-REST-API-Key: YOUR_PARSE_REST_API_KEY" \
  -H "Content-Type: application/json" \
  -d '{
        "where": {
          "deviceType": "embedded"
        },
        "data": {
          "alert": "A test push from Parse!"
        }
      }' \
  https://api.parse.com/1/push
```

**6. Troubleshooting**

* Provide troubleshooting tips for common issues.

### Existing Project Template

**1. Download Parse SDK and Integrate**

* Provide a link for downloading the Parse SDK for your platform.  This can be your Github repo fork's zip file download.
* Give instructions on how to load and configure the Parse SDK into an existing project.
* Provide instructions on how to get connect the device to WiFi, if necessary.
* At the end of this, the developer should be able to successfully build their project and also flash it onto the device. But, nothing should really happen with Parse functionality.

**2. Code Saving a Parse Object**

* Provide the exact code needed to save a Parse Object using the Parse SDK for your platform.
* This forces the developer to copy and paste it into the location.
* They can now build and run the project and observe that an object is saved.
* Please direct the developer the Parse.com Data Browser, where they can see their saved object.

**3. Code for Receiving a Push Notification**

* Provide the exact code needed to receive a Parse Push Notification using the Parse SDK for your platform to make an LED turn on for a duration of time, and turn off.
* This forces the developer to copy and paste it into the location.
* They can now build and run the project and send a push notification to the device to see an LED blink.
* To test push receiving, please instruct the developer to send a push using a curl command (see above).

**4. Troubleshooting**

* Provide troubleshooting tips for common issues.

