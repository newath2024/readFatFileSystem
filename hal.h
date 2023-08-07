#ifndef _HAL_H
#define _HAL_H
#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>

#define SECTOR_SIZE                    (512U)

/*******************************************************************************
* Name: HAL_ReadSector
* Function: read sector position index
* Parameters:   @uint32_t index: position sector
                @uint8_t *buff: array contain this sector
* Return:   
                @uint32_t byteRead: byte read in this sector
******************************************************************************/
uint32_t HAL_ReadSector(uint32_t index, uint8_t *buff);

/*******************************************************************************
* Name: HAL_ReadMultiSector
* Function: read sector from index to num
* Parameters:   @uint32_t index: position sector
                @uint32_t num: number sector read
                @uint8_t *buff: array contain this sector
* Return:   
                @uint32_t byteRead: byte read in 'num' sector
******************************************************************************/
uint32_t HAL_ReadMultiSector(uint32_t index, uint32_t num, uint8_t *buff);

#endif /*_HAL_H*/
