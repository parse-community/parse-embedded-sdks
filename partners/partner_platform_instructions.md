# Partner Platform Instructions

This document is for partner platform owners that want to make the Parse Embedded SDK run on their board.  We are asking partners to do the following steps so that we can improve the developer experience and preserve the Git history for merging future updates.  The original repo is https://github.com/ParsePlatform/parse-embedded-sdks.

* Partners should fork the repo under your own organization on GitHub.
* Partners are responsible for managing your fork of the repo.
* In the README of your repo fork
    * Please include a **quickstart guide** telling the developer how to get started on your board (e.g. saving an object, receiving a push notification)
        * Please see [Partner Quickstart Guidlines](/partners/partner_quickstart_guidelines.md) for details.
    * Please include the **developer support contact** for your board/platform.
        * Partners are responsible for responding to developer support questions for your board/platform in a timely manner.
* Please try to reuse as much existing code as possible, and put your platform-specific code in its own directory.  This makes it easier for you to pull in future changes from the upstream Parse repo.
* Future updates
    * Parse may routinely update the https://github.com/ParsePlatform/parse-embedded-sdks repo with bug fixes and new features.
    * Partners are responsible for merging upstream changes into your own fork of the repo and testing that the final product actually works.
    * Partners can merge in upstream changes on your own schedule, but the partner's repo fork must have working code.
* Please link to your repo fork in your product marketing materials.

## Setting up your GitHub repo fork

1. Fork the Parse Embedded SDKs repo at https://github.com/ParsePlatform/parse-embedded-sdks
    1. Please fork this repo to YOUR_ORGANIZATION/parse-embedded-sdks
2. Add your platform-specific code
    1. RTOS-based platforms
        1. Add your platform-specific code and build script under the /YOUR_PLATFORM_NAME directory
            1. For example, see the existing cc3200 directory.
        2. Your code should reuse the shared code in the [/rtos](/rtos) directory.
    2. Unix-based platforms
        1. Add your code and build script under the /unix/YOUR_PLATFORM_NAME directory
            1. For example, see the existing [/unix/yun](/unix/yun) directory
        2. Your code should reuse the shared code in the [/unix/src](/unix/src) directory.
3. Testing the SDK on your platform
    1. The SDK has 2 main functionalities.  You should test that both works:
        1. Issuing REST API requests to Parse (saving objects, queries, Cloud Code calls, etc).  Requests should also work with a user session token (e.g. fetching objects with ACL for that user).  See the [Parse Embedded C Guide](https://www.parse.com/docs/embedded_c/guide) for more details.
        2. Registering and receiving push notifications from Parse.
    2. We've provided a set of basic tests under the [/unix/samples/test](/unix/samples/test) directory.  You should make sure they pass for your platform.
