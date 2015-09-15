//Author:           Pascal Gendron
//Creation date:
//Description:      Tests the I3C Librairy

#include "tests/tests.h"

int main(int argc, char *argv[])
{
    Tests test;

    //TODO: Select tests via argv
    int appReturn = test.renderingEngine(argc, argv);
    return appReturn;

}
