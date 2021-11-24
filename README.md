# QMLaunch
Stands for QuietMiningLaunch, also known as SilentRunNBMB/SilentRunNBMinerBatchfile

## What is this?
QMLaunch is a program written in C++ with the Windows API to semi-automate cryptocurrency mining. I started writing this because me and my friends were tired of having to close the mining window during gaming and reopening it after. Sometimes one of us would forget to reopen the mining at all, losing hashrate and profitability for the group. Also I wanted to put the Windows API to more interesting uses. This is currently still heavily work-in-progress, as it barely has any significant features at the moment.

# Features and usage
## Command-line
```enableOnStartup``` - self-explanatory, the program will be added to be run on startup. Requires admin privileges on the local machine
```run``` - silently runs a file called "runnable.bat" that is presumed to already exist. It is also assumed that this batch file contains a script to start mining software.

With these features, it is currently possible to have the mining window automatically start in the background on computer bootup without any user involvement. 

# Todo
## Command-line
```disableOnStartup``` - removes autorunning the program on startup
```genConfig``` - generates a config.json for further configuration

## config.json
AFK timer settings/toggle - QMLaunch will autokill the mining when user activity (mouse, keyboard) is detected, and will restart after AFK for a certain time. Good for on-the-fly gaming and mining switching without having to do anything.

MMM server integration - IP and login to a remote mining monitor server 

## Other features not added yet
Retrieval of data from NBMiner/other API, to be connected with MMM server.

# How to build
1. Download the repository using your preferred method.
2. Open the project file with Visual Studio. I use 2019 Community Edition, but it should probably work fine in other versions. Note: You may need to make your own solution file in Visual Studio first, then import the project file.
3. Compile the project (<kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>B</kbd>)
4. Artifact will be in ```solution_directory/release_channel/architecture```. I believe default ```release_channel``` in Visual Studio is "Debug", and default ```architecture``` is "x86".

# License
GNU GPLv3.

# Disclaimer
Mining on other peoples computers without their knowledge and/or consent is most likely against the law in your area. I do not hold any responsibility for trouble that you get into by misusing this program.
