#pragma once
#include <iostream>
#include <fstream>
#include "QMLUtils.h"

#include "yaml-cpp/yaml.h"

using namespace std;

#define QML_CONFIG_VERSION 1 //int

class QMLConfig {
private:
    bool runOnce;
    char executable[64];
    char parameters[1024];

    bool afkEnabled;
    uint32_t afkTimeout;

    bool gameModeEnabled;
    uint32_t gameModeTimeout;
    char gameProcesses[20][64];

    bool monitorEnabled;
    char monitorType[16];
    int *ipAddress;
    int port;
    bool remoteOperations;

public:
    QMLConfig() {
        runOnce = false;
        strncpy_s(executable, "nbminer.exe", sizeof(executable));
        strncpy_s(parameters, "-a algo -o stratum+ssl://pool.net:port -u 0xaddress.worker -d 0 -i 100", sizeof(parameters));

        afkEnabled = true;
        afkTimeout = 60000;

        gameModeEnabled = true;
        gameModeTimeout = 1200000;
        strncpy_s(gameProcesses[0], "osu!.exe", sizeof(gameProcesses[0]));
        strncpy_s(gameProcesses[1], "VALORANT.exe", sizeof(gameProcesses[1]));
        strncpy_s(gameProcesses[2], "csgo.exe", sizeof(gameProcesses[2]));
        strncpy_s(gameProcesses[3], "javaw.exe", sizeof(gameProcesses[3]));

        monitorEnabled = false;
        strncpy_s(monitorType, "MMM", sizeof(monitorType));
        ipAddress = new int[4];
        ipAddress[0] = 1;
        ipAddress[1] = 2;
        ipAddress[2] = 3;
        ipAddress[3] = 4;
        port = 8080;
        remoteOperations = false;
    }

    void set_field(bool alsoUpdateConfig = false) {

    }

    void write_config() {
        ofstream file("config.yml", ios::trunc);

        if (!file.is_open()) {
            cout << "Failed to open config file" << endl;
        }

        file << "### General settings when executing \"run\" command\n"
            << "execution_settings:\n"
            << "    ### If true, \"run\" runs the program once.\n"
            << "    ### It will bypass any AFK or status reporting features\n"
            << "    ### regardless of their configuration settings.\n"
            << "    run_once: " << bool_to_charArr(runOnce) << "\n"
            << "    \n"
            << "    ### The file you want to run\n"
            << "    executable: \"" << executable << "\"\n"
            << "    \n"
            << "    ### Optional arguments for the file.\n"
            << "    parameters: \"" << parameters << "\"\n"
            << "\n"
            << "### Settings for AFK mining\n"
            << "afk_settings:\n"
            << "    ### General AFK timer until mining starts\n"
            << "    general_afk:\n"
            << "        enabled: " << bool_to_charArr(afkEnabled) << "\n"
            << "        \n"
            << "        ### In milliseconds\n"
            << "        timeout: " << afkTimeout << "\n"
            << "    \n"
            << "    ### AFK timer until mining starts when games are open.\n"
            << "    ### Overrides general AFK timer.\n"
            << "    ### general_afk must be enabled for this to work.\n"
            << "    game_mode:\n"
            << "        enabled: " << bool_to_charArr(gameModeEnabled) << "\n"
            << "        \n"
            << "        ### In milliseconds\n"
            << "        timeout: " << gameModeTimeout << "\n"
            << "        \n"
            << "        ### List of exe files that QMLaunch will detect running\n"
            << "        game_list:\n";

        for (int i = 0; i < (sizeof(gameProcesses) / sizeof(gameProcesses[0])); i++) {
            if (!(strcmp(gameProcesses[i], "") == 0)) {
                file << "        - \"" << gameProcesses[i] << "\"\n";
            }
        }

        file << "\n";
        file << "### Settings for remote status reporting\n"
            << "monitor_settings:\n"
            << "    enabled: " << bool_to_charArr(monitorEnabled) << "\n"
            << "    \n"
            << "    ### Remote server type. default: MMM\n"
            << "    type: \"" << monitorType << "\"\n"
            << "    \n"
            << "    ### IP of the remote server\n"
            << "    server_ip: \"" << ipAddress[0] << "." << ipAddress[1] << "." << ipAddress[2] << "." << ipAddress[3] << "\"\n"
            << "    \n"
            << "    ### Port of the remote server\n"
            << "    port: " << port << "\n"
            << "    \n"
            << "    ### Setting this to true allows the remote server to control the mining status\n"
            << "    enable_remote_operations: " << bool_to_charArr(remoteOperations) << "\n"
            << "\n"
            << "##### DO NOT TOUCH\n"
            << "configversion: " << QML_CONFIG_VERSION;
        
        file.close();
    }

    void config_routine() {
        //check if file exists
        ifstream ifile;
        ifile.open("config.yml");
        if (!ifile) {
            ifile.close();
            cout << "Config not present, it will be created" << endl;
            
            write_config();
        }
        else {
            ifile.close();
            //load config into memory
            bool needsRepair = false;
            YAML::Node config = YAML::LoadFile("config.yml");
            if (config["configversion"].as<int>() != 1) {
                cout << "Config version mismatch, the file will be reset" << endl;
                write_config();
            }
            else {
                //try catch
            }

            if (needsRepair) {

            }
        }
    }

    
};

