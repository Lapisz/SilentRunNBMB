#pragma once
#include <iostream>
#include <fstream>
#include <regex>
#include "QMLUtils.h"

#include "yaml-cpp/yaml.h"

using namespace std;

#define QML_CONFIG_VERSION 1 //int
#define QML_AFK_GAME_LIMIT 20 //int

struct CurrentConfig {

};

class QMLConfig {
private:
    bool runOnce;
    char executable[64];
    char parameters[1024];

    bool afkEnabled;
    uint32_t afkTimeout;

    bool gameModeEnabled;
    uint32_t gameModeTimeout;
    char gameProcesses[QML_AFK_GAME_LIMIT][64];

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

        for (int i = 0; i < QML_AFK_GAME_LIMIT; i++) {
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
            << "    ### Format: \"integer.integer.integer.integer\" where integer is not negative and less than 256\n"
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

    void set_data(CurrentConfig qmlct) {

    }

    CurrentConfig get_data() {
        CurrentConfig test; //temp
        return test;
    }

    bool config_routine(bool testMode) {
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
            YAML::Node config;
            try {
                config = YAML::LoadFile("config.yml");
            }
            catch (YAML::Exception &error) {
                cout << "Critical error parsing configuration file: " << error.msg << " (line " << error.mark.line << ", column " << error.mark.column << ")" << endl;
                cout << "Please fix syntax or run resetConfig" << endl;
                return false;
            }
            
            if (!config["configversion"]) { 
                cout << "Config missing version field, please run resetConfig" << endl;
                return false;
            }
            else if (config["configversion"].as<int>() != 1) {
                cout << "Config version mismatch, please run resetConfig" << endl;
                return false;
            }
            else {
                bool needsRepair = false;
                try {
                    //execution_settings
                    if (config["execution_settings"]) {
                        runOnce = config["execution_settings"]["run_once"].as<bool>();
                        strncpy_s(executable, const_cast<char*>(config["execution_settings"]["executable"].as<string>().c_str()), sizeof(executable));
                        strncpy_s(parameters, const_cast<char*>(config["execution_settings"]["parameters"].as<string>().c_str()), sizeof(parameters));
                    }
                    else {
                        cout << "execution_settings collection missing from config, will assume default values" << endl;
                    }
                    
                    //afk_settings
                    if (config["afk_settings"]) {
                        //general_afk
                        if (config["afk_settings"]["general_afk"]) {
                            afkEnabled = config["afk_settings"]["general_afk"]["enabled"].as<bool>();
                            afkTimeout = config["afk_settings"]["general_afk"]["timeout"].as<uint32_t>();
                        } else {
                            cout << "general_afk collection missing from config, will assume default values" << endl;
                        }

                        //game_mode
                        if (config["afk_settings"]["game_mode"]) {
                            gameModeEnabled = config["afk_settings"]["game_mode"]["enabled"].as<bool>();
                            gameModeTimeout = config["afk_settings"]["game_mode"]["timeout"].as<uint32_t>();

                            //game_list
                            if(config["afk_settings"]["game_mode"]["game_list"]){
                                for (size_t index = 0; (index < config["afk_settings"]["game_mode"]["game_list"].size()) && (index < QML_AFK_GAME_LIMIT); index++) {
                                    strncpy_s(gameProcesses[index], 
                                        const_cast<char*>(config["afk_settings"]["game_mode"]["game_list"][index].as<string>().c_str()), 
                                        sizeof(gameProcesses[index]));
                                    if ((index == QML_AFK_GAME_LIMIT - 1) && (gameModeEnabled || testMode)) {
                                        cout << "AFK game limit reached (" << QML_AFK_GAME_LIMIT << " games), last game added: \"" << gameProcesses[index] << "\"" << endl;
                                    }
                                }
                            }
                            else if (gameModeEnabled || testMode) {
                                needsRepair = true;
                                cout << "AFK game list missing from config, will use default games and write them to file" << endl;
                            }

                        } else {
                            cout << "game_mode collection missing from config, will assume default values" << endl;
                        }
                    } else {
                        cout << "afk_settings collection missing from config, will assume default values" << endl;
                    }

                    //monitor_settings
                    if (config["monitor_settings"]) {
                        monitorEnabled = config["monitor_settings"]["enabled"].as<bool>();
                        strncpy_s(monitorType, const_cast<char*>(config["monitor_settings"]["type"].as<string>().c_str()), sizeof(monitorType));

                        char ipRaw[16];
                        strncpy_s(ipRaw, const_cast<char*>(config["monitor_settings"]["server_ip"].as<string>().c_str()), sizeof(ipRaw));
                        int index = 0;
                        char processing[4][4];
                        char* remaining;
                        strncpy_s(processing[index], strtok_s(ipRaw, ".", &remaining), sizeof(processing[index]));
                        while ((strcmp(remaining, "\0") != 0) && index < 3) {
                            index++;
                            strncpy_s(processing[index], strtok_s(NULL, ".", &remaining), sizeof(processing[index]));
                        }
                        if (index < 3) {
                            if (monitorEnabled || testMode) {
                                cout << "Formatting error for remote server IP! Disabling remote statistics and resetting IP field to default" << endl;
                                monitorEnabled = false;
                                needsRepair = true;
                            }
                        }
                        else {
                            for (int i = 0; i < 4; i++) {
                                ipAddress[i] = strtol(processing[i], NULL, 10);

                                if (ipAddress[i] > 255) {
                                    ipAddress[i] = 255;
                                    if (monitorEnabled || testMode) {
                                        cout << "IP Address section #" << i << " is too high, will set it to 255." << endl;
                                        needsRepair = true;
                                    }
                                }
                                else if (ipAddress[i] < 0) {
                                    ipAddress[i] = 0;
                                    if (monitorEnabled || testMode) {
                                        cout << "IP Address section #" << i << " cannot be negative, will set it to 0." << endl;
                                        needsRepair = true;
                                    }
                                }
                            }
                        }

                        port = config["monitor_settings"]["port"].as<int>();
                        remoteOperations = config["monitor_settings"]["enable_remote_operations"].as<bool>();
                    } else {
                        cout << "monitor_settings collection missing from config, will assume default values" << endl;
                    }

                }
                catch (YAML::Exception &error) {
                    cout << "Something went wrong while parsing the configuration file (" << error.msg << ")," << endl;
                    cout << "will reset to default value for the field at line " << error.mark.line << ", column " << error.mark.column << endl;
                    needsRepair = true;
                }

                if (needsRepair) {
                    write_config();
                }

            }

            return true;
        }
    }

    
};

