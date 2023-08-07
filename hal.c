#include"hal.h"

/*******************************************************************************
* Name: HAL_ReadSector
* Function: read sector position index
* Parameters:   @uint32_t index: position sector
                @uint8_t *buff: array contain this sector
* Return:   
                @uint32_t byteRead: byte read in this sector
******************************************************************************/
uint32_t HAL_ReadSector(uint32_t index, uint8_t *buff)
{
    uint64_t positionPointer;
    uint32_t byteRead;
    FILE *fp;
    fp = fopen("floppy.img","rb");
    if(fp == NULL)
    {
        printf("cannot open file");
    }
    else
    {
        positionPointer = index * SECTOR_SIZE;
        fseek(fp, positionPointer, SEEK_SET);
        byteRead = fread(buff, sizeof(uint8_t), SECTOR_SIZE, fp);
    }
    fclose(fp);
    return byteRead;
}

/*******************************************************************************
* Name: HAL_ReadMultiSector
* Function: read sector from index to num
* Parameters:   @uint32_t index: position sector
                @uint32_t num: number sector read
                @uint8_t *buff: array contain this sector
* Return:   
                @uint32_t byteRead: byte read in 'num' sector
******************************************************************************/
uint32_t HAL_ReadMultiSector(uint32_t index, uint32_t num, uint8_t *buff)
{
    uint64_t positionPointer;
    uint32_t byteRead;
    FILE *fp;
    fp = fopen("floppy.img", "rb");
    if(fp == NULL)
    {
        printf("cannot open file");
    }
    else
    {
        positionPointer = index * SECTOR_SIZE;
        fseek(fp, positionPointer, SEEK_SET);
        byteRead = fread(buff, sizeof(uint8_t), SECTOR_SIZE * num, fp);
    }
    fclose(fp);
    return byteRead;
}
