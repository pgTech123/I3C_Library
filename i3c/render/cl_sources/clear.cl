//Author:           Pascal Gendron
//Creation date:    October 1st 2015
//Description:


__kernel void clearMemoryBit(__global int *childId_memStatusBit)
{
    //FIXME: Verify the need to be global...
    atomic_and(childId_memStatusBit+get_global_id(0), 0x3FFFFFFF);  //Reset status bits
}
