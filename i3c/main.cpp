//Author:           Pascal Gendron
//Creation date:
//Description:      Tests the I3C Librairy

// Select modules:

//#define     I3C_RENDER_MODULE_ENABLED     // required: OpenCL, OpenGL, Windows
#define     I3C_EDITING_MODULE_ENABLED

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

    int appReturn = Tests::runTests(I3C_TESTS_EDITING);

    //END LOG
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    logs << "Program ended with code: " << appReturn << "\t " << asctime(timeinfo) << endl;

    return appReturn;

}
