# CMP Apps tutorials

This repository contains samples that show and explain the main CMP features. Each sample is writen in C++ and can be deployed as a CMP app. All of them are associated to a Readme that explains how to deploy the app and the main step of the source code. 

## What are the CMP features available and explained in these tutorials:

- **Live stream** : How to display  your camera live video in the CMP interface.
- **Telemetry** : How to upload and store any kind of data in order to analyse and display it later.
- **Application parameters**: How to define parameters to your application, settable in the CMP interface.
- **Remote functions**: How to define and call a remote function.
- **Custum stream** : How to replace your live camera stream by images of your choice.
- **Video event** : How to define Video Events, reccorded ans accessible through the interface.
- **MQTT publisher and receiver** : How to use MQTT to communicate between apps on the local network.


## Requirements
You will deploy these tutorials on one of the devices installed on your CMP workspace. The CMP supports Jetson Nano, TX2 and Xavier or any computer. If you are using a Jetson, make sure it has been flashed. If you haven't done it already, [flash your Jetson](https://docs.nvidia.com/sdk-manager/install-with-sdkm-jetson/index.html).

To be able to run this tutorial:

- [Sign In the CMP and created a workspace](https://www.stereolabs.com/docs/cloud/overview/get-started/).
- [Add and Setup a device](https://www.stereolabs.com/docs/cloud/overview/get-started/#add-a-camera).
- As most of the tutorials require a ZED camera, a ZED should be plugged to this device.

## What is a CMP app ?
A CMP app is an application deployed in a Docker container on one of the devices that you installed on your CMP workspace. Note that it can be coded in the langage of your choice and is not limited to a camera usage. It can be everything: a web server, a sensor reporter, an alert notifier, a node-red application ... However the CMP interface is optimized for the camera managment, and is especialy adapted to the ZED 3D cameras. Therefore most of the available tutorials requires a ZED to be runned.

### General structure
A CMP app is runned in a Docker container. The [CMP documentation](https://www.stereolabs.com/docs/cloud/applications/) explains in detail how an app is structured and deployed. 
To summury to deploy an app you need to upload a .zip file that contains at least:
- docker-compose.yml describing how the application should be runned
- a runtime dockerfile listing the steps and commands to compile and run the application.
- an app.json file that descibes your application, specifies its name and release name and defines the parameters (see tutorial_04_application_parameters for more information about it)
- the source code or the executable 

#### Devlopping an app in C++
You are adviced to developp your app in C++ insofar as a C++ API named sl_IOT is available in this langage and make it easier to use the CMP features.
The provided tutorials can be a good starting point for your own apps. Here is how they should be used:

```
.
├── app
│   └── Dockerfile.runtime
├── app.json
├── cmp_builder.sh
├── docker-compose.yml
├── README.md
└── sources
    ├── CMakeLists.txt
    ├── Dockerfile.build
    └── src
        └── main.cpp 
```

**Build stage**: 
The source code needs to be compiled before deploying the app. The code is compiled inside a dedicated docker image. To run the associated container you just need to run 
```
$ ./cmp_builder.sh
```
 The shell script will ask on which kind of device you will deploy your app and will set the `Dockerfile.build` parameteres in concequence. The `Dockerfile.build` descibes the build stage. It generates binaries, stored in the `./app` folder. Then an `app.zip` file is generated (still by the cmp_builder script). This `app.zip` file is your packaged app and is ready to be deployed. It contains : 
- docker-compose.yml
- app.json 
- Dockerfile.runtime
- The binaries generated during the **build stage**
- Optionaly an icon.png image

**Deployment stage**:  
Now you just need to deploy your app using the CMP interface:
- In your workspace, in the **Applications** section, click on **Create a new app** 
- Get the .zip an Drag’n’Drop in the dedicated area
- Select the devices on which you want to deploy  the app and press **Deploy** 

**Deployment stage in dev mode**:
There is an other way to deploy your application without using the CMP interface. Lets supposed you built an app/tutorial on a computer A that has been [setup as a CMP device](https://www.stereolabs.com/docs/cloud/overview/get-started/#add-a-camera) on the CMP interface, you can deploy the app on this computer by running on this computer:
```
$ sudo cmpcli run
```
Now you can also build an app/tutorial on a computer A, and deploy it on a device B.
There are only three requirements:
- Computer A and device B must be on the same local network
- Device B must be [setup as a CMP device](https://www.stereolabs.com/docs/cloud/overview/get-started/#add-a-camera)
- Computer A must have **cmpcli** installed. You can find it in the [CMP tutorials repository]()

 Run : 
```
$ sudo cmpcli run --host device_name@device_IP
```


#### Devlopping an app in Python
You are not advice to devlopp in Python as many CMp feature are not available (live view, recordings, Video Events...) However some samples are available to show you how to use the telemetry, logs and app parameters in Python.


## Docker complement
Here are some explaination about the docker related files.  

> **Note** : You need a bit of experience with docker. (As long as you know and understand the basics of building and running a container you’ll be more than fine)

### docker-compose.yml
The **docker-compose.yml** file is the first file that is read when app app is deployed. It many indicates the path to the runtime Dockerfile that indicates how the app must be runned. It also configure the Docker environment.
```
version: '2.3'
services:
  tuto01_service:
    build :
        context : ./app
        dockerfile : Dockerfile
        args :
            BASE_IMAGE: stereolabs/iot:0.22.0-runtime-jetson-jp4.4
    runtime: nvidia
    privileged: true
    network_mode: "host"
    environment:
        - NVIDIA_DRIVER_CAPABILITIES=all
```

In this example the container of the application will run the **tuto01_service** service. This service will use the Dockerfile named **Dockerfile** in the `./app` folder. This Dockerfile will be read with the parameter **BASE_IMAGE** set to `stereolabs/iot:0.22.0-runtime-jetson-jp4.4` by default.


### Runtime Dockerfile

The runtime Dockerfile is the file that described what your **Docker image** or **app** must do when deployed. Basicly it indicates which script or executable must be runned. In the exemple bellow the binary `app_executable` is runned.

```
ARG BASE_IMAGE
FROM ${BASE_IMAGE}

#Install build dependencies
RUN apt-get update -y && \
DEBIAN_FRONTEND=noninteractive apt-get install -y tzdata libpng-dev && \
     rm -rf /var/lib/apt/lists/* &&  apt-get autoremove &&  apt-get clean

#Copy your binary
COPY app_executable /
CMD ["/app_executable"]

```

### Build Dockerfile

The build Dockerfile is **only used for the c++ applications** and is **not part of your app** meaning that it is never package in the .zip file. It descibes the image used to compile your c++ code. This image is similar to the runtime image (the image used to run your app) but may contain tools required to build the source code and that your runtime image does not need. 

The build Dockerfile is used by the `cmp_builder.sh` script and generate one ore several **binaries**. These binaries are then used by the runtime Image described bu the runtime dockerfile.



## Next steps
These 7 samples/tutorials can be used as starting point to devlop your own app. Some other tutorials are available to help you to identify where and how modifing these app in order to adapte them to your needs.

### How to upload a new release?
Lets supposed you modified one of the tutorials and want to deploy this new version.

- Modify the `app.json` file by giving an other release name. 
```json
      ...
    "release":{
        "name": "1.0.0",  #modify 1.0.0 by a name of your choice. You are advice to keep the x.x.x shape
      ... 
```
You just have to follow the same build and deployment stages than those described in each tutorial.




### How to transform my not-CMP application in a CMP app?
(coming soon)


### How to use an other base Docker image than the IOT image for my application ? 
(coming soon)

