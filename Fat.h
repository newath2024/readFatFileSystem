#ifndef _FAT_H
#define _FAT_H
#include "hal.h"

/*******************************************************************************
* Definitions
******************************************************************************/

#define SIZE_OF_JMP                    (3U)
#define SIZE_OF_OEM                    (8U)
#define SIZE_ROOT_ENTRY                (32U)
#define IS_THIS_FODER                  (0x10U)
#define IS_THIS_FILE                   (0x00U)
#define FIRST_CLUSTER                  (2U)

#define FIRST_YEAR                     (1980U)
#define SHIFT_DAY                      (0U)
#define MASK_DAY                       (0x1FU)
#define SHIFT_MONTH                    (5U)
#define MASK_MONTH                     (0x0FU)
#define SHIFT_YEAR                     (9U)
#define MASK_YEAR                      (0xFFFF)

#define SHIFT_HOUR                     (11U)
#define MASK_HOUR                      (0xFFFFU)
#define SHIFT_MINUTE                   (5U)
#define MASK_MINUTE                    (0x3FU)
#define SHIFT_SECOND                   (0U)
#define MASK_SECOND                    (0x1FU)

#define SHIFT_FIRST_FATENTRY           (8U)
#define MASK_FIRST_FATENTRY            (0x0FU)
#define SHIFT_SECOND_FATENTRY          (4U)
#define MASK_SECOND_FATENTRY           (0x00FFU)
#define END_OF_FILE                    (0xFFFU)

#define DOC_OR_TXT                     (1U)
#define PNG_OR_PDF                     (2U)

typedef struct{
    uint8_t JmpBoot[SIZE_OF_JMP];
    uint8_t OEMName[SIZE_OF_OEM];
    uint16_t bytePerSec;
    uint8_t secPerClus;
    uint16_t numReseredSec;
    uint8_t numFatTable;
    uint16_t numRootDirectoryEntries;
    uint16_t numSecInFile;
    uint8_t Media;
    uint16_t numSecPerFAT;
}boot;

typedef struct root
{
    uint8_t fileName[8];
    uint8_t fileType[3];
    uint8_t fileAttributes;
    uint8_t userAttributes;
    uint8_t reserved[9];
    uint16_t timeModified;
    uint16_t dateModified;
    uint16_t startCluster;
    uint32_t sizeFile;
    uint32_t checkSum;

    struct root *next;
}rootEntry;

/*******************************************************************************
* API
******************************************************************************/

/*******************************************************************************
* Name: readBootRegion
* Function: read information from boot region
* Parameters:
* Return:   void
******************************************************************************/
void readBootRegion(void);

/*******************************************************************************
* Name: createNewNodeAsRootEntry
* Function: creat new node in linked list as root directory entry 
* Parameters: @uint8_t *buff
* Return:   
              @rootEntry *newNode: this is the node is created
******************************************************************************/
rootEntry *createNewNodeAsRootEntry(uint8_t *buff);

/*******************************************************************************
* Name: readRootRegion
* Function: read information of root region
* Parameters: 
* Return:   
              @void
******************************************************************************/
void readRootRegion(void);

/*******************************************************************************
* Name: showFileInFolder
* Function: show information file/folder in folder
* Parameters: 
* Return:   @void
******************************************************************************/
uint8_t showFileInFolder(void);

/*******************************************************************************
* Name: openFolder
* Function: open file/folder in folder
* Parameters: @uint32_t positionFileInFolder: position of file in folder
* Return:   
              @void
******************************************************************************/
void openFolder(uint32_t positionFileInFolder);

/*******************************************************************************
* Name: resetLinkedList
* Function: clear linked list
* Parameters: 
* Return:   
              @void
******************************************************************************/
void resetLinkedList(void);
#endif/*_FAT_H*/

