# Tutorial 2 - Live Stream and Continuous Recording

> **NOTE**: The source code of this application and a code explaination are available [here](https://github.com/stereolabs/cmp-examples/tree/main/tutorials)

This tutorial shows how to deploy an application that starts a ZED camera and and sends the live stream to the CMP interface. You will also be able to store the video on your device.  The recorded video will be available on the CMP interface and downloadable. 


## What you will obtain after deployment
This app have two direct consequences in the CMP interface:

- A live stream should be visible
- The available recording should be listed

### Live video
In the **Settings** panel of your device, make sure that the **Privacy mode** is disabled, otherwisethe video won't be visible.
Wait at least until your app is **running**. 

![](./images/tuto_2_running.png " ")

If you click in the **Devices** panel  on the device where the app is deployed, you should see the live video (with a delay of a few seconds).

![](./images/live_and_recordings.png " ")



### Recordings

In the **Settings** panel of your device, make sure that the **Enable Recording** parameter is set to True, otherwise the video won't be recorded. Keep **Recording Mode** on **Continuous**. It means that everything will be recorded. The only limit is your device Hard Drive storage. When there is no space left on it, the older recordings are **definitly erased**. (see tutorial_07_video_event to understand the **On Event** recording mode).

It is the only thing to do to start recording. The recordings are listed by hour and day in the **Video** panel of your device. 

![](./images/recordings.png " ")


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
