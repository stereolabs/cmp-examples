# Tutorial 5 - Remote Function Calls
> **NOTE**: The source code of this application and a code explaination are available [here](https://github.com/stereolabs/cmp-examples/tree/main/tutorials)

This tutorial shows how to make an application containing a **remote function** that computes an addition called by a **remote function call**.
This tutorial does not require a ZED to be runned.
 
#### What is a remote function call ?
A **remote function call** is a call to a remote function **declared and register by a CMP app**. The app described in this tutorial declares and registers the `additionCallback` remote function. 
The call to this function can be done from **any computer** connected to the internet, by using a **REST request**. The way to perform this request is explained later in this tutorial.


## What you will obtain after deployment
The application in itself only define and register a **callback function** that can be called from anywhere if the app is running. Therefore there is not a lot to see in the CMP interface. Make simply sure that your application has a **building** and finaly a **running status**.  

### The registered function
The function registered by the application performs a basic **addition**.


### Call your remote function
Before calling your remote function, you have to get the necessary values to use the REST API.

- Your **API token** that you can register from the **API panel** in CMP. Get more informations in the [REST API documenation](https://www.stereolabs.com/docs/cloud/rest-api/)

- Your **region url** that you can get using the REST API in the ```/workspaces``` endpoint. Get more informations in the [REST API documenation](https://www.stereolabs.com/docs/cloud/rest-api/workspaces/)

Exemple:
```
$ curl -s -H "Content-Type:application/json" -H "Authorization:Bearer ${your_token}" -X GET https://cloud.stereolabs.com/api/v1/workspaces

```

- Your **workspace id** that you can get using the REST API in the ```/workspaces``` endpoint. Get more informations in the [REST API documenation](https://www.stereolabs.com/docs/cloud/rest-api/workspaces/)


- Your **device id** that you can get using the REST API in the ```/workspaces/$workspace_id/devices``` endpoint. Get more informations in the [REST API documentation](https://www.stereolabs.com/docs/cloud/rest-api/devices/)


To call your remote function, simply use the script ```remote_function_call.sh``` that contains a REST request using curl with the given arguments with **num1** and **num2** as the numbers you want to use in your addition :

```
./remote_function_call.sh $num1 $num2 $region_url $workspace_id $device_id $access_token
```

The REST request uses this format :\
It uses the endpoint ```${region_url}/workspaces/${workspace_id}/devices/${device_id}/functions/${function_name}``` where ```function_name``` is the name of the function you registered in your application using the ```IoTCloud::registerFunction```. In this tutorial the function registered is `tuto05_add` .\
The `parameters` key of the json contained in the request contains the parameters of the function. In our case `num1` and `num2`.

```
    curl -s -H "Content-Type: application/json" \
       -H "Authorization: Bearer ${access_token}"\
       -X POST ${region_url}/workspaces/${workspace_id}/devices/${device_id}/functions/tuto05_add \
       -d "{ \
       \"parameters\": { \
         \"num1\": ${num1}, \
	 \"num2\": ${num2} \
         }\
       }"
```

You can get more informations about remote function calls in the [documentation](https://www.stereolabs.com/docs/cloud/remote-functions/call/).

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
