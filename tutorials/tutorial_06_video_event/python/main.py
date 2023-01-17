########################################################################
#
# Copyright (c) 2023, STEREOLABS.
#
# All rights reserved.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
########################################################################

import pyzed.sl as sl
import pyzed.sl_iot as sliot


def main():
    # Initialize the communication to ZED Hub, with a zed camera.
    zed = sl.Camera()
    status_iot = sliot.HubClient.connect("event_app")

    if status_iot != sliot.STATUS_CODE.SUCCESS:
        print("Initialization error ", status_iot)
        exit(1)

    status_iot = sliot.HubClient.register_camera(zed)
    if status_iot != sliot.STATUS_CODE.SUCCESS:
        print("Camera registration error ", status_iot)
        exit(1)

    # Open the zed camera
    init_params = sl.InitParameters()
    init_params.camera_resolution = sl.RESOLUTION.HD2K
    init_params.depth_mode = sl.DEPTH_MODE.PERFORMANCE

    status_zed = zed.open(init_params)

    if status_zed != sl.ERROR_CODE.SUCCESS:
        sliot.HubClient.send_log(
            "Camera initialization error : " + str(status_zed), sliot.LOG_LEVEL.ERROR)
        exit(1)

    # Enable Position tracking (mandatory for object detection)
    track_params = sl.PositionalTrackingParameters()
    track_params.set_as_static = False
    status_zed = zed.enable_positional_tracking(track_params)
    if status_zed != sl.ERROR_CODE.SUCCESS:
        sliot.HubClient.send_log(
            "Positionnal tracking initialization error : " + str(status_zed), sliot.LOG_LEVEL.ERROR)
        exit(1)

    # Enable the Objects detection module
    object_detection_params = sl.ObjectDetectionParameters()
    object_detection_params.image_sync = True
    object_detection_params.enable_tracking = False
    object_detection_params.detection_model = sl.DETECTION_MODEL.MULTI_CLASS_BOX
    status_zed = zed.enable_object_detection(object_detection_params)
    if status_zed != sl.ERROR_CODE.SUCCESS:
        sliot.HubClient.send_log(
            "Object detection initialization error : " + str(status_zed), sliot.LOG_LEVEL.ERROR)
        exit(1)

    # Object Detection runtime parameters : detect person only
    # see  the ZED Doc for the other available classes https://www.stereolabs.com/docs/api/group__Object__group.html#ga13b0c230bc8fee5bbaaaa57a45fa1177 
    object_detection_runtime_params = sl.ObjectDetectionRuntimeParameters()
    object_detection_runtime_params.detection_confidence_threshold = 50
    object_detection_runtime_params.object_class_filter = []
    object_detection_runtime_params.object_class_filter.append(
        sl.OBJECT_CLASS.PERSON)

    counter_no_detection = 0
    objects = sl.Objects()
    event_reference = ""
    first_event_sent = False
    last_ts = sl.Timestamp()

    # Main loop
    while True:
        status_zed = zed.grab()
        if status_zed != sl.ERROR_CODE.SUCCESS:
            break

        zed.retrieve_objects(objects, object_detection_runtime_params)

        #----------     Define event   ----------
        # Let's define a video event as a video on which you can detect someone at least every 10 frames.
        # If nobody is detected for 10 frames, a new event is defined next time someone is detected.
        # Cf README.md to understand how to use the event_reference to define a new event.

        current_ts = objects.timestamp
        if len(objects.object_list) >= 1 and (current_ts.get_seconds() - last_ts.get_seconds() >= 2):
            new_event = True
            if counter_no_detection >= 10 or not first_event_sent:
                event_reference = "detected_person_" + \
                    str(current_ts.get_milliseconds())
            else:
                new_event = False

            event_params = sliot.EventParameters()
            event_params.timestamp = current_ts.get_milliseconds()
            event_params.reference = event_reference
            event_params.retention = 5
            event_label = "People detection"
            event_to_send = {}
            event_to_send["message"] = "Current event as reference : " + \
                str(event_reference)
            event_to_send["nb_detected_person"] = len(objects.object_list)

            if new_event or not first_event_sent:
                sliot.HubClient.start_video_event(
                    event_label, event_to_send)
                first_event_sent = True
                print("Event started")
            else:
                sliot.HubClient.update_video_event(
                    event_label, event_to_send)
                print("Event updated")


            counter_no_detection = 0
            last_ts = current_ts

        elif current_ts.get_seconds() - last_ts.get_seconds() >= 2:
            counter_no_detection = counter_no_detection + 1

        # In the end of a grab(), always call a update() on the cloud.
        sliot.HubClient.update()

    print("Grab error ", status_zed)
    # Close the camera
    if zed.is_opened():
        zed.close()

    # Close the communication with ZED Hub properly.
    status_iot = sliot.HubClient.disconnect()
    if status_iot != sliot.STATUS_CODE.SUCCESS:
        print("Terminating error ", status_iot)
        exit(1)

    return


if __name__ == "__main__":
    main()
