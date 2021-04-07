# Tutorial 1 - Basic Application

This sample shows how to make a very simple application that **connects** itself to the cloud and **sends logs**. This tutorial does not require a ZED.


## Requirements
You will deploy this tutorial on one of the devices installed on **your CMP workspace**. The CMP supports Jetson Nano, TX2 and Xavier or any computer. If you are using a Jetson, make sure it has been flashed. If you haven't done it already, [flash your Jetson](https://docs.nvidia.com/sdk-manager/install-with-sdkm-jetson/index.html).

To be able to run this tutorial:

- [Sign In the CMP and created a workspace](https://www.stereolabs.com/docs/cloud/overview/get-started/).
- [Add and Setup a device](https://www.stereolabs.com/docs/cloud/overview/get-started/#add-a-camera).


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
`cmp_builder.sh` packages your app by generating an app.zip file. 
Now you just need to [deploy your app](https://www.stereolabs.com/docs/cloud/applications/sample/#deploy) using the CMP interface:

- In your workspace, in the **Applications** section, click on **Create a new app** 
- Get the .zip an **Drag’n’Drop** in the dedicated area
- Select the devices on which you want to deploy the app and press **Deploy** 

**Additional information about deployment and CMP apps :**

This README only focus on the source code explaination and the way to deploy the app without giving technical explaination about the app deployment. 
Please refer to the main README of this repository if you want more information about the CMP apps structure and technical precisions.  


## What you should see after deployment
This app is a minimalist app that only connect itself to the cloud and send logs (live stream is not provided by this app). It doesn't mean there is nothing to see:

###  App status
On the CMP interface you can consult the available applications list on your device. To do so, go in the Device panel. Click on the device where the app is deployed and go in the application section.
You should find a line corresponding to this tutorial, **"Tutorial 01 - Basic App"**.
The App status is displayed and indicates if your app is:

- Stopped
- Building
- Running
- Failed

Therefore you should see that the app is first in **building** state (for the first deployment it can last one minute or two) and finally **running**. 

![](./images/app_1_building.png " ")


### Terminal logs
If you click on the app status, you will have access to the app **logs in a terminal**, both for the building and the running stage.

![](./images/terminal_panel.png " ")


###  App logs
Wait until your app is **running**.
If you click  on the device where the app is deployed and go in the **Logs** section, you should see three logs associated to this tutorial:

- "Initialization succeeded"
- "Application connected"
- "Log 1 sent" and a new one every 15 seconds.

![](./images/logs_panel.png " ")



It corresponds to the `IoTCloud::log` functions of the source code : 

```c++    
    //Send a log
    IoTCloud::log("Initialization succeeded",LOG_LEVEL::INFO);

    //Is your application connected to the cloud
    if (IoTCloud::isInitialized()==STATUS_CODE::SUCCESS)
        IoTCloud::log("Application connected",LOG_LEVEL::INFO);
```


## The Source Code explained

To init your application, you have to get your ```SL_APPLICATION_TOKEN``` environment variable and use it in the ```IoTCloud::init``` function. `IoTCloud::init` starts communications between your app and the cloud. It must be called before using the IOTCloud API, so before sending logs (`IoTCloud::log`), telemetry(`IoTCloud::sendTelemetry`) custom stream (`IoTCloud::setCustomVideoMat`) and others ...
```c++
    const char * application_token = ::getenv("SL_APPLICATION_TOKEN");
    STATUS_CODE status_iot = IoTCloud::init(application_token);
```
You can set the log level limit to be displayed. Every log with LOG_LEVEL below the limit will not be print.  ``` setLogLevelThreshold(LOG_LEVEL local_terminal_log_level, LOG_LEVEL cloud_log_level)```
```c++
    IoTCloud::setLogLevelThreshold(LOG_LEVEL::INFO,LOG_LEVEL::INFO);
```

You can send a simple log to the cloud with ```IoTCloud::logInfo```
```c++
    IoTCloud::log("Application connected",LOG_LEVEL::INFO);
```

You can check if your application is connected to the cloud with ```IoTCloud::isInitialized```
```c++
    if (IoTCloud::isInitialized())
        IoTCloud::log("Application connected",LOG_LEVEL::INFO);
```

Not that there are 7 log Levels : 

- 0 - DEBUG : log level for debugging
- 1 - INFO : log level for an info
- 2 - STATUS : log level for a status
- 3 - WARNING : log level for a warning
- 4 - ERROR : log level for an error
- 5 - SUCCESS : log level for a success
- 6 - DISABLED : level where no log will be sent
