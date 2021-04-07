#include <stdio.h>
#include <string.h>
#include <chrono>

#include <sl/Camera.hpp>
#include <sl_iot/IoTCloud.hpp>
#include <csignal>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace sl;
using namespace sl_iot;
using json = sl_iot::json;


// Parameters, defined as global variables
bool draw_bboxes = true;
bool recordVideoEvent  = true;
int nbFramesNoDetBtw2Events = 30; //number of frame
bool recordTelemetry  = true;
float telemetryFreq  = 10.f; //in seconds


cv::Mat slMat2cvMat(Mat& input) {
    // Mapping between MAT_TYPE and CV_TYPE
    int cv_type = -1;
    switch (input.getDataType()) {
    case MAT_TYPE::F32_C1: cv_type = CV_32FC1; break;
    case MAT_TYPE::F32_C2: cv_type = CV_32FC2; break;
    case MAT_TYPE::F32_C3: cv_type = CV_32FC3; break;
    case MAT_TYPE::F32_C4: cv_type = CV_32FC4; break;
    case MAT_TYPE::U8_C1: cv_type = CV_8UC1; break;
    case MAT_TYPE::U8_C2: cv_type = CV_8UC2; break;
    case MAT_TYPE::U8_C3: cv_type = CV_8UC3; break;
    case MAT_TYPE::U8_C4: cv_type = CV_8UC4; break;
    default: break;
    }

    return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(MEM::CPU));
}



// Parameteres callbacks
void onDisplayParametersUpdate(FunctionEvent &event) {
    event.status = 0;
    draw_bboxes = IoTCloud::getParameter<bool>("draw_bboxes", PARAMETER_TYPE::APPLICATION, draw_bboxes);
    IoTCloud::log("New parameter : draw_bboxes modified",LOG_LEVEL::INFO);

}

void onVideoEventUpdate(FunctionEvent &event) {
    event.status = 0;
    recordVideoEvent = IoTCloud::getParameter<bool>("recordVideoEvent", PARAMETER_TYPE::APPLICATION, recordVideoEvent);
    nbFramesNoDetBtw2Events = IoTCloud::getParameter<int>("nbFramesNoDetBtw2Events", PARAMETER_TYPE::APPLICATION, nbFramesNoDetBtw2Events);
    IoTCloud::log("New parameters : recordVideoEvent or nbFramesNoDetBtw2Events modified",LOG_LEVEL::INFO);

}

void onTelemetryUpdate(FunctionEvent &event) {
    event.status = 0;
    recordTelemetry = IoTCloud::getParameter<bool>("recordTelemetry", PARAMETER_TYPE::APPLICATION, recordTelemetry);
    telemetryFreq = IoTCloud::getParameter<float>("telemetryFreq", PARAMETER_TYPE::APPLICATION, telemetryFreq);
    IoTCloud::log("New parameters : recordTelemetry or telemetryFreq modified",LOG_LEVEL::INFO);
}



int main(int argc, char **argv) {
    // Create camera object
    std::shared_ptr<sl::Camera> p_zed;
    p_zed.reset(new sl::Camera());


    //Init sl_iot
    const char * application_token = ::getenv("SL_APPLICATION_TOKEN");
    STATUS_CODE status_iot = IoTCloud::init(application_token, p_zed);
    if (status_iot != STATUS_CODE::SUCCESS) {
        std::cout << "IoTCloud " << status_iot << std::endl;
        exit(EXIT_FAILURE);
    }
    
    //Open the ZED camera
    sl::InitParameters initParameters;
    initParameters.camera_resolution = RESOLUTION::HD2K;
    initParameters.depth_mode = DEPTH_MODE::PERFORMANCE;

    sl::ERROR_CODE status_zed = p_zed->open(initParameters);
    if (status_zed != ERROR_CODE::SUCCESS) {
        IoTCloud::log("Camera initialization error : " + std::string(toString(status_zed)), LOG_LEVEL::ERROR);
        exit(EXIT_FAILURE);
    }



    // Enable Position tracking (mandatory for object detection)
    sl::PositionalTrackingParameters trck_params;
    trck_params.set_as_static = false;
    std::cout<<"[Device CORE app] Enable Positional Tracking "<<std::endl;
    auto zed_error =  p_zed->enablePositionalTracking(trck_params);
    if (zed_error != ERROR_CODE::SUCCESS) {
        std::cout << sl::toVerbose(zed_error) << "\nExit program." << std::endl;
        p_zed->close();
        return 1;
    }

    // Enable the Objects detection module
    std::cout<<"[Device CORE app] Enable Object Detection Module"<<std::endl;
    sl::ObjectDetectionParameters obj_det_params;
    obj_det_params.image_sync = true;
    obj_det_params.enable_tracking = true;
    obj_det_params.detection_model =  sl::DETECTION_MODEL::MULTI_CLASS_BOX;

    zed_error = p_zed->enableObjectDetection(obj_det_params);

    if (zed_error != ERROR_CODE::SUCCESS) {
        std::cout << sl::toVerbose(zed_error) << "\nExit program." << std::endl;
        p_zed->close();
        return 1;
    }
    // Object Detection runtime parameters : detect person only
    // see  the ZED Doc for the other available classes https://www.stereolabs.com/docs/api/group__Object__group.html#ga13b0c230bc8fee5bbaaaa57a45fa1177 
    ObjectDetectionRuntimeParameters objectTracker_parameters_rt;
    objectTracker_parameters_rt.detection_confidence_threshold = 50;
    objectTracker_parameters_rt.object_class_filter.clear();
    objectTracker_parameters_rt.object_class_filter.push_back(sl::OBJECT_CLASS::PERSON);


    // Runtime parameters
    sl::RuntimeParameters rt_param;
    rt_param.measure3D_reference_frame = sl::REFERENCE_FRAME::CAMERA;

    /*********    App parameters      *************/

    CallbackParameters callback_display_param;
    callback_display_param.setParameterCallback("onDisplayParametersUpdate", "draw_bboxes", CALLBACK_TYPE::ON_PARAMETER_UPDATE, PARAMETER_TYPE::APPLICATION);
    IoTCloud::registerFunction(onDisplayParametersUpdate, callback_display_param);

    CallbackParameters callback_event_param;
    callback_event_param.setParameterCallback("onVideoEventUpdate", "recordVideoEvent|nbFramesNoDetBtw2Events", CALLBACK_TYPE::ON_PARAMETER_UPDATE, PARAMETER_TYPE::APPLICATION);
    IoTCloud::registerFunction(onVideoEventUpdate, callback_event_param);


    CallbackParameters callback_telemetry_param;
    callback_telemetry_param.setParameterCallback("onTelemetryUpdate", "recordTelemetry|telemetryFreq", CALLBACK_TYPE::ON_PARAMETER_UPDATE, PARAMETER_TYPE::APPLICATION);
    IoTCloud::registerFunction(onTelemetryUpdate, callback_telemetry_param);

    // get values defined by the CMP interface. 
    // Last argument is default value in case of failure
    draw_bboxes = IoTCloud::getParameter<bool>("draw_bboxes", PARAMETER_TYPE::APPLICATION, draw_bboxes);
    recordVideoEvent = IoTCloud::getParameter<bool>("recordVideoEvent", PARAMETER_TYPE::APPLICATION, recordVideoEvent);
    nbFramesNoDetBtw2Events = IoTCloud::getParameter<int>("nbFramesNoDetBtw2Events", PARAMETER_TYPE::APPLICATION, nbFramesNoDetBtw2Events);
    recordTelemetry = IoTCloud::getParameter<bool>("recordTelemetry", PARAMETER_TYPE::APPLICATION, recordTelemetry);
    telemetryFreq = IoTCloud::getParameter<float>("telemetryFreq", PARAMETER_TYPE::APPLICATION, telemetryFreq);

    /****************************/

    // Main loop
    int counter_no_detection = 0;
    sl::Objects objects;
    std::string event_reference = "first_event";
    sl::Timestamp prev_timestamp = p_zed->getTimestamp(TIME_REFERENCE::CURRENT);

    // Images
    sl::Mat imgLeftCustom(1280,720, sl::MAT_TYPE::U8_C4);
    cv::Mat leftImageCpuCV;
    leftImageCpuCV = slMat2cvMat(imgLeftCustom);

    sl::Resolution image_raw_res = p_zed->getCameraInformation().camera_resolution;

    while (true) {
        // Grab a new frame from the ZED
        status_zed = p_zed->grab(rt_param);

        if (status_zed != ERROR_CODE::SUCCESS) break;
        
        p_zed->retrieveObjects(objects, objectTracker_parameters_rt);

        /*******     Define event   *********/
        /*
        Let's define a video event as a video on wich you can detect someone at least every 10 frames.
        If nobody is detected for 10 frames, a new event is defined next time someone is detected.
        Cf README.md to understand how to use the event_reference to define a new event.
        */
       
        sl::Timestamp current_ts = objects.timestamp;

        int counter_reliable_objects = 0;
        for (int i = 0; i < objects.object_list.size(); i++){
            if (objects.object_list[i].tracking_state == sl::OBJECT_TRACKING_STATE::OK){
                counter_reliable_objects ++;
            }
        }


        if (recordVideoEvent && counter_reliable_objects >= 1){
            if (counter_no_detection >= nbFramesNoDetBtw2Events){
                event_reference = "detected_person_" + std::to_string(current_ts.getMilliseconds()); 
                IoTCloud::log("New Video Event defined",LOG_LEVEL::INFO);
            }
            else{
                // Do nothing, keep previous event reference --> The current frame will be defined as being part of the previous video event  
            }
            
            EventParameters event_params;
            event_params.timestamp = current_ts.getMilliseconds();
            event_params.reference = event_reference;    
            std::string event_label = "People Detection"; // or label of your choice
            json event2send; // Use to store all the data associated to the video event. 
            event2send["message"] = "Current event as reference " + event_reference;
            event2send["nb_detected_personn"] = objects.object_list.size();

            IoTCloud::addVideoEvent(event_label, event2send, event_params);

            counter_no_detection = 0; //reset counter as someone as been detected
        }
        else {
            counter_no_detection ++;
        }
        /*******************************/


        /*******     Custom stream : Draw bboxes on custom stream   *********/
        if(draw_bboxes){
            p_zed->retrieveImage(imgLeftCustom, sl::VIEW::LEFT, sl::MEM::CPU, imgLeftCustom.getResolution());


            float ratio_x = (float)leftImageCpuCV.cols/(float)image_raw_res.width;
            float ratio_y = (float)leftImageCpuCV.rows/(float)image_raw_res.height;


            for (int i= 0;i< objects.object_list.size();i++) {
                if (objects.object_list[i].tracking_state == sl::OBJECT_TRACKING_STATE::OK) {
                    sl::uint2 tl = objects.object_list[i].bounding_box_2d[0];
                    sl::uint2 br = objects.object_list[i].bounding_box_2d[2];
                    cv::Rect ROI = cv::Rect(cv::Point2i(tl.x*ratio_x, tl.y*ratio_y), cv::Point2i(br.x*ratio_x, br.y*ratio_y));
                    cv::Scalar color = cv::Scalar(50, 200, 50, 255);
                    cv::rectangle(leftImageCpuCV, ROI, color,2);
                }
            }

            IoTCloud::setCustomVideoMat(imgLeftCustom);
        }
        /*******************************/

        /*******     Define and send Telemetry   *********/
        // In this exemple we send every second the number of people detected and there mean distance to the camera

        sl::Timestamp curr_timestamp = p_zed->getTimestamp(sl::TIME_REFERENCE::IMAGE);
        if (recordTelemetry && (uint64) (curr_timestamp.getMilliseconds() >= (uint64) (prev_timestamp.getMilliseconds() + (uint64)telemetryFreq * 1000ULL))) {
            float mean_distance = 0;
            // compute objects ( = people)  mean distance from camera. This value will be sent as telemetry  
            for (int i= 0;i< objects.object_list.size();i++) {
                mean_distance += (objects.object_list[i].position).norm();
            }

            if (objects.object_list.size() > 0 ){
                mean_distance /= (float) objects.object_list.size();
            }

            // Send Telemetry
            sl_iot::json position_telemetry;
            position_telemetry["number_of_detection"] = objects.object_list.size();
            position_telemetry["mean_distance_from_cam"] = mean_distance;
            IoTCloud::sendTelemetry("object_detection", position_telemetry);
            prev_timestamp = curr_timestamp;
        }

        /*******************************/

        // Always refresh IoT at the end of the grab loop
        IoTCloud::refresh();
    }

    // Handling camera error
    if(status_zed != ERROR_CODE::SUCCESS){
        IoTCloud::log("Grab failed, restarting camera. "+std::string(toString(status_zed)), LOG_LEVEL::ERROR);
        p_zed->close();
        sl::ERROR_CODE e = sl::Camera::reboot(p_zed->getCameraInformation().serial_number);
    }
    //Close the camera
    else if(p_zed->isOpened())
        p_zed->close();
    
    return 0;
}
