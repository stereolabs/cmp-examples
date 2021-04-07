## App version : 
# Runtime image
# This image is for running the application
# we just copy the executable in this container


ARG BASE_IMAGE
FROM ${BASE_IMAGE}

#Install build dependencies
RUN apt-get update -y && \
DEBIAN_FRONTEND=noninteractive apt-get install -y tzdata libx11-dev  && \
apt-get install -y --no-install-recommends libopencv-dev && \
     rm -rf /var/lib/apt/lists/* && apt autoremove && apt clean

#Copy your binary
COPY app_executable /
# CMD ["valgrind", "/app_executable"]
CMD ["/app_executable"]
