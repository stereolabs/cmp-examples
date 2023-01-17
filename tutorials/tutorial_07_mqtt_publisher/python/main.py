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

import pyzed.sl_iot as sliot
import time


def main():
    # Initialize the communication to ZED Hub, without a zed camera.
    status_iot = sliot.HubClient.connect("pub_app")
    if status_iot != sliot.STATUS_CODE.SUCCESS:
        print("Initialization error ", status_iot)
        exit(1)

    # The name of the topic we will publish our message in
    topic_name = "/my_custom_data"

    # Main loop
    while True:
        my_message_js = {}
        my_message_js["message"] = "Hello World"
        my_message_js["my_custom data"] = 54
        my_message_js["timestamp"] = int(time.time())

        sliot.HubClient.publish_on_topic(topic_name, my_message_js)
        sliot.HubClient.send_log("Message published", sliot.LOG_LEVEL.INFO)
        time.sleep(10)

    # Close the communication with ZED Hub properly.
    status_iot = sliot.HubClient.disconnect()
    if status_iot != sliot.STATUS_CODE.SUCCESS:
        print("Terminating error ", status_iot)
        exit(1)
    
    return


if __name__ == "__main__":
    main()
 
