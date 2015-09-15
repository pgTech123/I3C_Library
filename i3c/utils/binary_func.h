//Author:           Pascal Gendron
//Creation date:    September 5th 2015
//Description:      Functions on bits

#ifndef BINARY_FUNC_H
#define BINARY_FUNC_H

#ifdef __cplusplus
extern "C"
{
#endif

//bool is_Base2(unsigned char v);
bool is_Base2(int v);

//int numberHighBits(unsigned char v);
int numberHighBits(int v);

int firstHighBit(int v);

#ifdef __cplusplus
}
#endif

#endif // BINARY_FUNC_H
