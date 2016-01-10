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
    //TODO: Select tests via argv
    int appReturn = test.renderingEngine(argc, argv);

    //END LOG
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    logs << "Program ended with code: " << appReturn << "\t " << asctime(timeinfo) << endl;

    return appReturn;

}
