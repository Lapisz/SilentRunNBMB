# QMLaunch
Stands for Quiet Mining Launcher, but the project may also be known as SilentRunNBMB


## What is this?
QMLaunch is a program written in C++ with the Windows API and yaml-cpp to semi-automate cryptocurrency mining. I started writing this because me and my friends were tired of having to close the mining window during gaming and reopening it after. Sometimes one of us would forget to reopen the mining at all, losing hashrate and profitability for the group. Also I wanted to put the Windows API to more interesting uses. This is currently still heavily work-in-progress, there are more features that I want to add in the future.


# Disclaimer
Mining on other peoples computers without their knowledge and/or consent is most likely against the law in your area. I do not hold any responsibility for trouble that you get into by misusing this program.


# Features and usage
## Command-line
```disableOnStartup``` - removes itself from automatically running on logon. May ask for system admin rights

```enableOnStartup``` - adds itself to automatically run on logon. May ask for system admin rights

```resetConfig``` - clears all values in configuration and resets them to default

```run``` - runs an executable specified in ```config.yml``` with the parameters also specified there. Will do configuration check similar to testConfig

```stop``` - kills background QMLaunch process (if running) and the executable specified in ```config.yml```

```testConfig``` - will test ```config.yml``` for formatting or data type errors. Might automatically correct or reset them to default


<h2>Configuration</h2>

Configuration is done in the file ```config.yml```

Default configuration and values:

```yml
### General settings when executing "run" command
execution_settings:
    ### The file you want to run
    executable: "nbminer.exe"
    
    ### Optional arguments for the file.
    parameters: "-a algo -o stratum+ssl://pool.net:port -u 0xaddress.worker -d 0 -i 100"

### Settings for AFK mining
afk_settings:
    ### General AFK timer until mining starts
    general_afk:
        enabled: true
        
        ### In milliseconds
        timeout: 60000
    
    ### AFK timer until mining starts when games are open.
    ### Overrides general AFK timer.
    ### general_afk must be enabled for this to work.
    game_mode:
        enabled: true
        
        ### In milliseconds
        timeout: 1200000
        
        ### List of exe files that QMLaunch will detect running
        game_list:
        - "osu!.exe"
        - "VALORANT.exe"
        - "csgo.exe"
        - "javaw.exe"

### Settings for remote status reporting
monitor_settings:
    enabled: false
    
    ### Remote server type. default: MMM
    type: "MMM"
    
    ### IP of the remote server
    ### Format: "integer.integer.integer.integer" where integer is not negative and less than 256
    server_ip: "1.2.3.4"
    
    ### Port of the remote server
    port: 8080
    
    ### Setting this to true allows the remote server to control the mining status
    enable_remote_operations: false

##### DO NOT TOUCH
configversion: 1
``` 


# Todo
## Command-line
```config``` - opens a GUI for easier configuration (may require a lot of backend reorganization for this, we will see)


## config.json
MMM server integration - IP and login to a remote mining monitor server 

Disable-able tray icon


## Other features not added yet
Retrieval of data from NBMiner/other API, to be connected with MMM server.


# How to build
1. Download this repository using your preferred method.
2. Obtain [yaml-cpp source code from here](https://github.com/jbeder/yaml-cpp/)
3. Compile yaml-cpp by running cmake in its source folder, opening the generated master solution in Visual Studio, then building it with ```Release x64```. You should get a file called ```yaml-cpp.lib```. There are more details on how to run cmake online and in its command-line help screen.
4. Open QMLaunch's solution file with Visual Studio.
5. Go to ```Project Properties -> C++ -> General -> Additional Include Directories```, and set it to the include folder inside of yaml-cpp's source folder.
7. In ```Project Properties -> Linker -> Input```, set ```Additional Dependencies``` to your ```yaml-cpp.lib```
8. Compile QMLaunch (<kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>B</kbd>)
9. Artifact location should be specified in Visual Studio's build log.


# License
GNU GPLv3.


