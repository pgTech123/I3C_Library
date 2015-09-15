#include "binary_func.h"

/*int numberHighBits(unsigned char v)
{
    int count = 0;
    unsigned char ucComparator = 0x01;

    for(int i = 0; i < sizeof(unsigned char)*8; i++)
    {
        if((v & ucComparator) != 0)
        {
            count ++;
        }
        ucComparator = ucComparator << 1;
    }

    return count;
}
*/
int numberHighBits(int v)
{
    int count = 0;
    int iComparator = 0x01;

    for(int i = 0; i < sizeof(int)*8; i++)
    {
        if((v & iComparator) != 0){
            count ++;
        }
        iComparator = iComparator << 1;
    }
    return count;
}

/*bool is_Base2(unsigned char v)
{
    if(numberHighBits(v) == 1)
    {
        return true;
    }
    return false;
}
*/
bool is_Base2(int v)
{
    if(numberHighBits(v) == 1){
        return true;
    }
    return false;
}

int firstHighBit(int v)
{
    int iComparator = 0x01;

    for(int i = 0; i < sizeof(int)*8; i++)
    {
        if((v & iComparator) != 0){
            return i;
        }
        iComparator = iComparator << 1;
    }

    return 0;
}
