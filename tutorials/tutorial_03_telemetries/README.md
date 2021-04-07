# Tutorial 3 - Telemetries

This tutorial shows you how to send telemetry to the cloud. This sample app opens a ZED and enable ZED tracking, meaning that you can access the camera position at each frame. Then the application gets the camera position and sends it to the cloud at each frame. Therefore the Telemetry panel will contain all the concecutive position of your camera.  


## Requirements
You will deploy this tutorial on one of the devices installed on your CMP workspace. The CMP supports Jetson Nano, TX2 and Xavier or any computer. If you are using a Jetson, make sure it has been flashed. If you haven't done it already, [flash your Jetson](https://docs.nvidia.com/sdk-manager/install-with-sdkm-jetson/index.html).

To be able to run this tutorial:

- [Sign In the CMP and created a workspace](https://www.stereolabs.com/docs/cloud/overview/get-started/).
- [Add and Setup a device](https://www.stereolabs.com/docs/cloud/overview/get-started/#add-a-camera).
- A ZED must be plugged to this device.
- **Enable recordings** and **disable privacy mode** in the Settings panel of your device
 

## Build and deploy this tutorial

### How to build your application
To build your app just run:

```
$ cd /PATH/TO/tutorial_01_basic_app
$ ./cmp_builder.sh
```

- The script will ask for the **device type** (jetson or classic x86 computer) on which you want to deploy this app. **Note** that it may be different than the computer on which you run `cmp_builder.sh`.
- The script will also ask for your **device cuda version**. If you do not know it you can find it in the **Info** section of your device in the CMP interface.
- Finally you will be asked the **IOT version** you want to use. It corresponds to the base docker imaged used to build your app docker image. You can chose the default one, or look for the [most recent version available on Dockerhub](https://hub.docker.com/r/stereolabs/iot/tags?page=1&ordering=last_updated).


### How to deploy your application
`cmp_builder.sh` packages your app by generating a app.zip file. 
Now you just need to [deploy your app](https://www.stereolabs.com/docs/cloud/applications/sample/#deploy) using the CMP interface:

- In your workspace, in the **Applications** section, click on **Create a new app** 
- Get the .zip an Drag’n’Drop in the dedicated area
- Select the devices on which you want to deploy  the app and press **Deploy** 


**Additional information about deployment and CMP apps :**

This README only focus on the source code explaination and the way to deploy the app without giving technical explaination about the app deployment. 
Please refer to the main README of this repository if you want more information about the CMP apps structure and technical precisions.  


## What you should see after deployment
This app have two direct consequences in the CMP interface:

- A live stream should be visible
- The published telemetry should be accessible

### Live video
Wait at least until your app is **running**. 
If you click in the **Devices** panel  on the device where the app is deployed, you should see the live video (with a delay of a few seconds).

![](./images/live_and_recordings.png " ")

###  Telemetry
If you click go in the **Telemetry** panel, you should see the telemetry of your camera position as follow:

![](./images/telemetry.png " ")


## Code Overview

This sample app opens a ZED and enable ZED tracking, meaning that you can access the camera position at each frame. Then the application gets the camera position and sends it to the cloud at each frame. Therefore the Telemetry panel will contain all the concecutive position of your camera.  

What exactly appends:

- Init IOT to enable communications with the cloud. (See tutorial_01_basic_app README for more information).

- Open the ZED with `p_zed->open(initParameters)`. (See tutorial_02_live_stream_and_recording  README for more information).

- Enable Positional Tracking with `p_zed->enablePositionalTracking(positional_tracking_param)`. (See [ZED SDK API documentation](https://www.stereolabs.com/docs/api/classsl_1_1Camera.html#a7989ae783fae435abfdf48feaf147f44) for more information).

- In a While loop, grab a new frame

```
// Main loop
    while (true) {
        // Grab a new frame from the ZED
        status_zed = p_zed->grab(runtime_parameters);
        if (status_zed != ERROR_CODE::SUCCESS) break;
```

- Every seconds, get the camera position (Translation and rotation)

```
    if (curr_timestamp.getMilliseconds() >= prev_timestamp.getMilliseconds() + 1000) {
        // Retrieve camera position
        p_zed->getPosition(cam_pose);
        sl::Translation translation = cam_pose.getTranslation();
        sl::float3 rotation_vector = cam_pose.getRotationVector();

```

- Store the camera position inside a json and call `IoTCloud::sendTelemetry` 
A label is specified as follow `sendTelemetry(std::string label, json& value)`. It allows to improve the log consultation in the CMP interface as it is possible to sort them by label.

```
    // Send Telemetry
    sl_iot::json position_telemetry;
    position_telemetry["tx"] = translation.x;
    position_telemetry["ty"] = translation.y;
    position_telemetry["tz"] = translation.z;
    position_telemetry["rx"] = rotation_vector.x;
    position_telemetry["ry"] = rotation_vector.y;
    position_telemetry["rz"] = rotation_vector.z;
    IoTCloud::sendTelemetry("camera_position", position_telemetry);
    prev_timestamp = curr_timestamp;
```

- Call IoTCloud::Refresh in order to send the current image to the cloud
(See tutorial_02_live_stream_and_recording  README for more information)

```  
    // Always refresh IoT at the end of the grab loop
    IoTCloud::refresh();
    sleep_ms(1);
```