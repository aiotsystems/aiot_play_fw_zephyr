# AIOT Zephyr Application which uses Memfault for Performance Monitoring



## Project information

This project..



## Setup nRF Connect SDK (NCS) and nrfjprog

### NCS
The nRF Connect SDK (NCS) can be downloaded at:
https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/installation/assistant.html

The NCS is available as VS Code extension after installation.
Thus, the version of the NCS, toolchains and Zephyr can be updated inside VS code.

![image](./docs/1-1.jpg)

### nrfjprog
The nRF Command Line Tools can be downloaded at:
https://www.nordicsemi.com/Products/Development-tools/nrf-command-line-tools/download

If using the build, flash and debug functions of the NCS, check if the nrfjprog command line tool is installed by typing the command in your terminal:

```
nrfjprog -v
```

**Note**: In this tutorial we used:
- NCS version v.2.3.0 and Toolchain version v.2.3.0
- Command Line Tool versions:
```
nrfjprog -v
>> nrfjprog version: 10.21.0 external
>> JLinkARM.dll version: 7.80
```



## Clone Repository and Import into NCS

Clone Repository:
```
git@github.com:aiotsystems/aiot_play_fw_zephyr.git
```

Import in NCS:

![image](./docs/2-1.jpg)

Create Build Configuration:

![image](./docs/2-2.jpg)

FW is built in:

![image](./docs/2-3.jpg)


## Connect and Program AIOT

### Setup
HW picture


![image](./docs/3-1.jpg)

The should appear in

![image](./docs/3-2.jpg)

### Program

Check if UART Pins correct and if overlay file is used

![image](./docs/3-3.jpg)


Paste Memfault Cloud project key:

![image](./docs/3-4.jpg)

### Re-Build and Flash

![image](./docs/3-5.jpg)



## AIOT Network Joining Process

Plug-in the Manager into your computer and connect to serial API (thrid COM-Port in Windows):

![image](./docs/4-1.jpg)

Watch Join Process

![image](./docs/4-2.jpg)




## Receiving Heartbeats and Push Chunks to Memfault Cloud

### Setup
TODO


## Observe incoming Memfault Chunks at Memfault Cloud

### Setup
TODO
