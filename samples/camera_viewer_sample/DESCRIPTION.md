# Object Detection

> **NOTE**: The source code of this application and a code explaination are available [here](https://github.com/stereolabs/cmp-examples/tree/main/tutorials)

This sample is a basic exemple that shows you how to display your ZED live view in the CMP Video Panel. It also allows you to continuously record the video. This sample use and explain the following features of the CMP:
- **Logs** that informs you about the app's status
- **Live Stream** that shows you the live video from your zed in the **Video** Panel
- **Recordings** that are listed hour by hour in the **Video** Panel



## What you should see after deployment
Make sure that the recordings are enable and that the privacy mode is disabled (Settings panel of your device, in the CMP interface).
thanks to this app you will have access to:
- a **Live Stream** that shows you the live video
- **Recordings** listed in the **Video** panel by our
- **Logs** that informs you about the app's status


### Live video
In the **Settings** panel of your device, make sure that the **Privacy mode** is disabled, otherwisethe video won't be visible.
Wait at least until your app is **running**. 

![](./images/running.png " ")

If you click in the **Devices** panel  on the device where the app is deployed, you should see the live video (with a delay of a few seconds).

![](./images/live_view.png " ")



### Recordings

In the **Settings** panel of your device, make sure that the **Enable Recording** parameter is set to True, otherwise the video won't be recorded. Keep **Recording Mode** on **Continuous**. It means that everything will be recorded. The only limit is your device Hard Drive storage. When there is no space left on it, the older recordings are **definitly erased**. (see tutorial_07_video_event to understand the **On Event** recording mode).

It is the only thing to do to start recording. The recordings are listed by hour and day in the **Video** panel of your device. 


###  App logs
Wait until your app is **running**.
If you click  on the device where the app is deployed and go in the **Logs** section, you should see the logs associated to this application. If everything went well, you should see this: 

![](./images/logs.png " ")



### Terminal logs
If you click on the app status, you will have access to the app **logs in a terminal**, both for the building and the running stage.

![](./images/terminal.png " ")




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

