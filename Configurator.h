#pragma once
#include <iostream>
#include <fstream>
#include "QMLUtils.h"

using namespace std;

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

    int configVersion;
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

        configVersion = 1;
    }

    void config_routine() {
        //check if file exists
        ifstream ifile;
        ifile.open("config.yml");
        if (!ifile) {
            cout << "config.yml not present, it will be created" << endl;
            ifile.close();

        }
        else { ifile.close(); }

    }

    void write_config(bool) {

    }
};

