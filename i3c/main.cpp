//Author:           Pascal Gendron
//Creation date:
//Description:      Tests the I3C Librairy

#include <time.h>
#include "tests/tests.h"
#include "utils/logs.h"

int main(int argc, char *argv[])
{
    time_t rawtime;
    struct tm * timeinfo;

    //START LOG
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    logs << "Program started : \t\t" << asctime(timeinfo) << endl;

    //Actual program
    Tests test;
    int appReturn;
    //TODO: Select tests via argv
/*
    appReturn = test.readWriteV1();
    if(appReturn){
        logs << "Test readWriteV1 failed with code: " << appReturn << endl;
        return appReturn;
    }
*/
    appReturn = test.readV1WriteV2();
    if(appReturn){
        logs << "Test readV1WriteV2 failed with code: " << appReturn << endl;
        return appReturn;
    }

    appReturn = test.readV2WriteV1();
    if(appReturn){
        logs << "Test readV2WriteV1 failed with code: " << appReturn << endl;
        return appReturn;
    }

    appReturn = test.renderingEngine(argc, argv);

    //END LOG
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    logs << "Program ended with code: " << appReturn << "\t " << asctime(timeinfo) << endl;

    return appReturn;

}
