# Tutorial 4 - Application Parameters

This tutorial shows how to use custom application parameters in your application. These parameters can be modified by everyone in the CMP interface.
This sample starts a ZED exactly as tutorial_02_live_stream_and_recording does, but it also define one parameter. The first one indicates whether the camera LED must be on or off. 


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
The app should be exactly the same than tutorial_02_live_stream_and_recording's app. It means that you should be able to see the live video and the recordings list in the CMP interface. In addition the two parameters should be visible in the interface: they are part of your application description in **the `Applications` panel of your device**.

![](./images/app_4_running_param.png " ")

### Modifing the app parameter
Wait until your app is **running**. 

In the **`Applications` panel of your device**, click on the figure that indicates the number of available parameters. A pop up window appears. You can modify the parameters value and update your changes. In our exemple you can turn the ZED LED on and off thanks to the parameters and verifie it work by checking that the LED does turn on and off.

![](./images/param_panel.png " ")

### Live video
Wait at least until your app is **running**. 
If you click in the **Video** panel  on the device where the app is deployed, you should see the live video (with a delay of a few seconds).

![](./images/live_and_recordings.png " ")



## Code overview

Here are the elements you have to add to your code when you want to define a new app parameter:

- Add the parameter to the app.json file. If not done, the parameter won't be available in the CMP interface

- Associate a callback function to the parameter. Thanks to that the callback is triggered when the parameter value is modified in the interface. It is a way to notifier a parameter's value modification. 

- Write the callback. On parameter's value modification you can do whatever you want

- Use the parameter to do something. In our case we modify the LED status using the SDK API.

Lets detail these steps:


### Add the parameter in the app.json

When you want to define a new parameter you need to "declare" it in the app.json file. To do that you just need to add two lines in release --> default_parameters --> requested :
- The first one associates the parameter id (a string of your choice) and its default value.
```
    "led_status": "false",
```
- The second one is optional but recommanded. It describes the parameter and allows to compexify the CMP interface.
```
                "$led_status": {"name":"LED Status", "order":1,
                    "type":"boolean",  "unit":"", "description": "Define whether your ZED LED is turned on or off" }
```


***********
// augmented Image 
***********

In the app.json file, we finally have :
```
    ...

    "default_parameters": {
        "requested":{
            "core": { "disable_app": false },
            "led_status": "false",
            "$led_status": {"name":"LED Status", "order":1, 
                "type":"boolean",  "unit":"", "description": "Define whether your ZED LED is turned on or off" }
        }
    }

    ...
```

### Associate the parameter to a callback

The parameter is defined in the app.json and associated to a default value.  When this value is modified in the interface you are so far not notified of this modification in your app (even if the new value does is available). To be notify on a parameter modification, you need to associate your parameter to a callback in your C++ code. 


In our case we want that the `led_status` parameter defined in the `app.json` file be associated to the `onLedStatusChange` callback function.
In two step we 

- associate the parameter to the callback with `setParameterCallback`:

```c++
    CallbackParameters callback_param_led;
    callback_param_led.setParameterCallback("onLedStatusChange", "led_status", CALLBACK_TYPE::ON_PARAMETER_UPDATE, PARAMETER_TYPE::APPLICATION);
    
```

- decare this association to the cloud : 

```c++
    IoTCloud::registerFunction(onLedStatusUpdate, callback_param_led);
```

So we finaly have: 
```c++
    //Set your parameter callback
    CallbackParameters callback_param_led;
    callback_param_led.setParameterCallback("onLedStatusChange", "led_status", CALLBACK_TYPE::ON_PARAMETER_UPDATE, PARAMETER_TYPE::APPLICATION);
    IoTCloud::registerFunction(onLedStatusUpdate, callback_param_led);
```

### Add a callback function

Your parameter has been associated to a callback but this callback needs to be writen.
A parameter callback must at least have the following structure:
```c++
void myCallbackName(FunctionEvent &event) {
    event.status = 0;

    //TODO

}
```
It can contains everything, but keep in mind that it is called each time the associated parameter is modified. You can of course get the new parameter value inside the callback by using the `IoTCloud::getParameter` function.

In our case we only store the fact that the parameter has been modify inside the `led_status_updated` global variable.

```c++
bool led_status_updated = false;

void onLedStatusUpdate(FunctionEvent &event) {
    event.status = 0;
    led_status_updated = true;
}
```

Not that the callback name `onLedStatusUpdate` must correspond to the first parameter of the `setParameterCallback` function.

```c++
    CallbackParameters callback_param_led;
    callback_param_led.setParameterCallback("onLedStatusChange", "led_status", CALLBACK_TYPE::ON_PARAMETER_UPDATE, PARAMETER_TYPE::APPLICATION);
    
```

### Use the up to date parameter value

Your parameter is now totally integrated in the application and can be used for any purpose. In our case we wait for a modification of its value to do something: When the parameter is modify in the user interface, the callback is triggered. The new value is therefore accessible by calling  `IoTCloud::getParameter`. The callback also set  `led_status_updated` to True.

In the main loop of the application:
- The current led status is got in order to us it as default value.
- The new parameter value is got thanks to `IoTCloud::getParameter`
Note that `led_status` is the parameter id defined in the **app.json** file. 
`curr_led_status` is used as default value in the case where `getParameter` fails.
```c++
    bool led_status = IoTCloud::getParameter<bool>("led_status", PARAMETER_TYPE::APPLICATION, curr_led_status);
    
```

- Then the LED status is physicaly modified by calling the SDK function  `setCameraSettings`.
- A log is set to the cloud to notify the parameter value that has been used with the function `IoTCloud::reportParameter`.

```c++
if (led_status_updated) {
    int curr_led_status = p_zed->getCameraSettings(sl::VIDEO_SETTINGS::LED_STATUS);
    bool led_status = IoTCloud::getParameter<bool>("led_status", PARAMETER_TYPE::APPLICATION, curr_led_status);
    p_zed->setCameraSettings(sl::VIDEO_SETTINGS::LED_STATUS, led_status);
    IoTCloud::reportParameter<bool>("led_status", PARAMETER_TYPE::APPLICATION, led_status);
    led_status_updated = false;
}
```


Finaly with these four steps we easily added a parameter to the initial application tutorial_02_live_stream_and_recording and we used its values.


## Next steps

Note that the parameters panel can be quite complex. Please have a look to the sample **Camera Viewer sample** and **Object Detection sample** to understand how it works. A detailed documentation is coming soon.

  
