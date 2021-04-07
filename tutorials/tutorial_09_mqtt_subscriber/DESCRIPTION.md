# Tutorial 9 - MQTT Subscriber

> **NOTE**: This tutorial should be runned with the tutorial 8 : **MQTT Subscriber**
> **NOTE**: The source code of this application and a code explaination are available [here](https://github.com/stereolabs/cmp-examples/tree/main/tutorials)

This tutorial shows you how to communicate between apps on the local network. It subscribes to a MQTT topics and send a log to notify that the messages have been received.

## What you should see after deployment

The app subscribe to the MQTT topic where the tutorial 8 (MQTT publisher) publishes: `/v1/local_network/my_custom_data`. A log is published each time a message is received. 

![](./images/logs.png " ")


## Deployment

### Requirements
You will deploy this tutorial on one of the devices installed on your CMP workspace. The CMP supports Jetson Nano, TX2 and Xavier or any computer. If you are using a Jetson, make sure it has been flashed. If you haven't done it already, [flash your Jetson](https://docs.nvidia.com/sdk-manager/install-with-sdkm-jetson/index.html).

To be able to run this tutorial:

- [Sign In the CMP and created a workspace](https://www.stereolabs.com/docs/cloud/overview/get-started/).
- [Add and Setup a device](https://www.stereolabs.com/docs/cloud/overview/get-started/#add-a-camera).
- A ZED must be plugged to this device.
- **Enable recordings** and **disable privacy mode** in the Settings panel of your device

### How to deploy your application
You just need to [deploy your app](https://www.stereolabs.com/docs/cloud/applications/sample/#deploy) using the CMP interface:

- Select the devices on which you want to deploy the app 
- Click on the **Deploy** button
