#Parse Embedded C SDKs

[Parse Embedded C SDKs](https://www.parse.com/products/iot) provide support for Arduino Yún, Unix, and RTOS C platforms.

These SDKs let you use [Parse](https://www.parse.com/) for building Internet of Things (IoT) applications with connected devices.  Parse is a developer-friendly cloud platform that lets you get an IoT project running in minutes.   With Parse, you can:

* Build [cross-platform integrations](https://www.parse.com/products) between your connected device and mobile/web/desktop apps.

* Allow your users to personalize and monitor their connected devices.  
    * Parse is the easiest way to [build user login](https://www.parse.com/docs/ios_guide#ui-login/iOS) on mobile apps.  Parse also has [user session APIs](https://www.parse.com/docs/ios_guide#sessions/iOS) in all mobile SDKs, which let you provision restricted sessions from the phone after the user logs into your app.  You can then transfer this restricted session token to your device so that your device can access user-specific data.
    * With Parse APIs, you can build a device manager screen in your mobile app what shows the user's provisioned connected devices (sample app below).  At any time, the user can revoke a device from accessing his or her data on Parse.

* Send [push notifications](https://www.parse.com/products/push) to your connected devices.

* Securely access your app's data from connected devices. 
    * All communication between your connected device's Embedded SDK and the Parse Cloud, including push notifications, is protected by SSL encryption.
    * You can protect user data with [Access Control Lists (ACLs)](https://www.parse.com/docs/data#security-objects) so it can only be accessed with that user's session token.

* Perform complex application logic in [Parse Cloud Code](https://www.parse.com/docs/cloud_code_guide), so you can minimize the memory footprint of your app on your connected device.

* Send [analytics events](https://www.parse.com/products/analytics) from your connected devices, and see realtime graphs in your Parse web dashboard.

* Intuitively visualize your cloud data with the [Data Browser](http://blog.parse.com/2012/12/20/the-new-data-browser-2/) on the Parse website.

##Getting Started

###Arduino Yún

Please follow our [Parse Arduino Quickstart](https://www.parse.com/apps/quickstart#embedded/arduinoyun).  We highly recommend using Arduino Software (IDE).  See the [yun directory](/yun) for more details.

###Raspberry Pi / Ubuntu / Debian / other Unix-based systems

Please follow our [Parse Raspberry Pi Quickstart](https://www.parse.com/apps/quickstart#embedded/raspberrypi). See the [include](/include), [unix](/unix), and [debian](/debian) directories for more details.

###TI CC3200 / OTHER Real time operatings systems

Please follow our [Parse CC3200 Quickstart](https://www.parse.com/apps/quickstart#embedded/ticc3200). See the [cc3200 directory](/cc3200) for more details.

##Documentation

Please see the Parse website for detailed developer guides:

* [Arduino Guide](https://www.parse.com/docs/arduino_guide)

* [Embedded C Guide](https://www.parse.com/docs/embedded_c_guide) (Raspberry Pi / Unix / TI CC 3200)

##Sample App

We prepared a [sample app](https://github.com/ParsePlatform/Anydevice) that demonstrates how to provision connected devices using a companion phone app such that connected devices can securely access user-specific data on the Parse Cloud. This sample app also demonstrates how to send push notifications between the phone app and connected devices.

##Contributing

See the CONTRIBUTING file for how to help out.

##License

Copyright (c) 2015, Parse, LLC. All rights reserved.

You are hereby granted a non-exclusive, worldwide, royalty-free license to use, copy, modify, and distribute this software in source code or binary form for use in connection with the web services and APIs provided by Parse.

As with any software that integrates with the Parse platform, your use of this software is subject to the Parse Terms of Service [https://www.parse.com/about/terms]. This copyright notice shall be included in all copies or substantial portions of the software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

