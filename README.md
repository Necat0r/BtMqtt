# BtMqtt
Bluetooth&lt;->Mqtt bridge to track phone presence status

## What it does
It will scan all paired BT devices, publishing either 'true' or 'false' to the **btmqtt/presence** topic.

This is triggered from two sources
* Every 30 minutes it will trigger a scan of its own just to sync stuff up.
* It's also subscribed to **btmqtt/scan**, where a message will trigger an intensive scan for 1 minute, scanning every 5 seconds for devices. This is useful when a motion sensor triggers to help reduce latency in presence detection.

## Building it
Depends on mosquitto to build.
Download & install from http://mosquitto.org/download/
The solution relies upon the MOSQUITTO_DIR environment variable to find the mosquitto libs & headers


## Running
Mosquitto depends on both OpenSSL & Pthreads.
**NOTE: Pick up an OpenSSL 1.0.x release** (1.1.x doesn't seem to include libeay32.dll & ssleay32.dll).

From the mosquitto _readme-windows.txt_ file
```
Dependencies - win32
--------------------

* OpenSSL
    Link: http://slproweb.com/products/Win32OpenSSL.html
    Install "Win32 OpenSSL <version>"
    Required DLLs: libeay32.dll ssleay32.dll
* pthreads
    Link: ftp://sourceware.org/pub/pthreads-win32
    Install "pthreads-w32-<version>-release.zip
    Required DLLs: pthreadVC2.dll

Please ensure that the required DLLs are on the system path, or are in the same directory as
the mosquitto executable.
```
