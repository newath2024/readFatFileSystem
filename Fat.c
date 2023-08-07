#include"Fat.h"
/*******************************************************************************
* Variables
******************************************************************************/
boot bootSector;
rootEntry *head = NULL;
uint8_t *buff;

/*******************************************************************************
* Prototypes
******************************************************************************/

void readBootRegion(void);
rootEntry *createNewNodeAsRootEntry(uint8_t *buff);
void readRootRegion(void);
uint8_t showFileInFolder(void);
void openFolder(uint32_t positionFileInFolder);
void resetLinkedList(void);

/*******************************************************************************
* Code
******************************************************************************/

/*******************************************************************************
* Name: sizeFileInFat
* Function: get value little endian of 4 bytes
* Parameters:
            @uint8_t lastByte 
            @uint8_t thirdByte
            @uint8_t secondByte
            @uint8_t firstByte       
* Return:   
            @result: value little endian of 4 bytes 
******************************************************************************/
static uint32_t sizeFileInFat(uint8_t lastByte, uint8_t thirdByte, uint8_t secondByte, uint8_t firstByte)
{
    unsigned int result = (lastByte << 24) | (thirdByte << 16) | (secondByte << 8) | firstByte;
    return result;
}

/*******************************************************************************
* Name: littleEndian
* Function: get value little endian of 2 bytes
* Parameters:
            @uint8_t lowByte 
            @uint8_t highByte       
* Return:   
            @result: value little endian of 2 bytes 
******************************************************************************/
static uint32_t littleEndian(uint8_t lowByte, uint8_t highByte)
{
    return (lowByte << 8) | highByte;   
}

/*******************************************************************************
* Name: readBootRegion
* Function: read information from boot region
* Parameters:
* Return:   
            void
******************************************************************************/
void readBootRegion(void)
{
    uint8_t i;
    buff = (uint8_t *)malloc(SECTOR_SIZE*sizeof(uint8_t));
    if(HAL_ReadSector(0,buff) != SECTOR_SIZE)
    {
        printf("Read boot region error!");
    }
    else
    {
        for(i = 0;i < SIZE_OF_OEM;i++)
        {
            bootSector.OEMName[i] = buff[i + SIZE_OF_JMP];
        }
        bootSector.bytePerSec = littleEndian(buff[12],buff[11]);
        bootSector.secPerClus = buff[13];
        bootSector.numReseredSec = littleEndian(buff[15],buff[14]);
        bootSector.numFatTable = buff[16];
        bootSector.numRootDirectoryEntries = littleEndian(buff[18],buff[17]);
        bootSector.numSecInFile = littleEndian(buff[20],buff[19]);
        bootSector.numSecPerFAT = littleEndian(buff[23],buff[22]);
    }
    free(buff);
}

/*******************************************************************************
* Name: positionByteContainCluster
* Function: get position byte contain Cluster
* Parameters: @uint32_t cluster
* Return:   
              @position:
******************************************************************************/
uint32_t positionByteContainCluster(uint32_t cluster)
{
    uint32_t position;
    if (cluster <= 1) 
    {
        position = 1;
    } 
    else 
    {
        position = 3 * (cluster / 2) + 1;
    }
    return position;
}

/*******************************************************************************
* Name: getFirstFatEntryValue
* Function: get value of first fat entry
* Parameters: @uint32_t secondByte
              @uint32_t firstByte
* Return:   
              @valueFatEntry: value of first fat entry
******************************************************************************/
static uint16_t getFirstFatEntryValue(uint8_t firstByte, uint8_t secondByte) {
    uint16_t valueFatEntry = 0;
    valueFatEntry |= ((secondByte & MASK_FIRST_FATENTRY) << SHIFT_FIRST_FATENTRY);  
    valueFatEntry |= firstByte;                  
    return valueFatEntry;
}

/*******************************************************************************
* Name: getSecondFatEntryValue
* Function: get value of second fat entry
* Parameters: @uint32_t secondByte
              @uint32_t thirdByte
* Return:   value of second fat entry
******************************************************************************/
static uint16_t getSecondFatEntryValue(uint8_t secondByte, uint8_t thirdByte)
{
    return (((uint16_t)thirdByte & MASK_SECOND_FATENTRY) << SHIFT_SECOND_FATENTRY) | (secondByte >> SHIFT_SECOND_FATENTRY);
}

/*******************************************************************************
* Name: addressNextCluster
* Function: get value which is address of this cluster
* Parameters: @int32_t cluster
* Return:   
              @uint16_t getAddress: address in this cluster
******************************************************************************/
static uint64_t addressNextCluster(uint32_t cluster)
{
    uint64_t byteContainAPartOfCluster = positionByteContainCluster(cluster);
    uint16_t getAddress;
    buff = (uint8_t *)malloc(SECTOR_SIZE * sizeof(uint8_t) * bootSector.numSecPerFAT);
    if(HAL_ReadMultiSector(1, bootSector.numSecPerFAT, buff) != SECTOR_SIZE * bootSector.numSecPerFAT)
    {
        printf("Read fat table error!");
    }
    else
    {
        if(cluster % 2 == 0)
        {
            getAddress = getFirstFatEntryValue(buff[byteContainAPartOfCluster - 1], buff[byteContainAPartOfCluster]);
        }
        else
        {
            getAddress = getSecondFatEntryValue(buff[byteContainAPartOfCluster], buff[byteContainAPartOfCluster + 1]);
        }
    }
    return getAddress;
}

/*******************************************************************************
* Name: typeOfFile
* Function: get type of file 
* Parameters: @uint8_t *buff
* Return:   
              @uint8_t typeFile: type of file
******************************************************************************/
static uint8_t typeOfFile(uint8_t *buff){
    uint8_t typeFile;
    if((buff[0] == 0x54 && buff[1] == 0x58 && buff[2] == 0x54) || (buff[0] == 0x44 && buff[1] == 0x4F && buff[2] == 0x43)){
        typeFile = DOC_OR_TXT;
    }
    else{
        typeFile = PNG_OR_PDF;
    }
    return typeFile;
}

/*******************************************************************************
* Name: createNewNodeAsRootEntry
* Function: creat new node in linked list as root directory entry 
* Parameters: @uint8_t *buff
* Return:   
              @rootEntry *newNode: this is the node is created
******************************************************************************/
rootEntry *createNewNodeAsRootEntry(uint8_t *buff)
{
    rootEntry *newNode = (rootEntry *)malloc(sizeof(rootEntry));
    uint8_t i;    
    if(newNode == NULL)
    {
        printf("\n\n The disk is empty!");
    }    
    else
    {
        for(i = 0; i < 8; i ++)
        {
            newNode->fileName[i] = buff[i];
        }
        for(i = 0; i < 3; i ++)
        {
            newNode->fileType[i] = buff[i+8]; 
        }
        newNode->fileAttributes = buff[11];
        newNode->userAttributes = buff[12];
        newNode->timeModified = littleEndian(buff[23], buff[22]);
        newNode->dateModified = littleEndian(buff[25], buff[24]);
        newNode->startCluster = littleEndian(buff[27], buff[26]);
        newNode->sizeFile = sizeFileInFat(buff[31], buff[30], buff[29], buff[28]);
        newNode->checkSum = 0;
        for(i = 0;i < SIZE_ROOT_ENTRY;i++)
        {
            newNode->checkSum = newNode->checkSum + buff[i];
        }
        newNode->next = NULL;
    }
    return newNode;
}

/*******************************************************************************
* Name: addRootEntryInList
* Function: add node in linked list
* Parameters: @rootEntry *newNode: node just created
* Return:   
******************************************************************************/
rootEntry addRootEntryInList(rootEntry *newNode)
{
    newNode->next = head;
    head = newNode;
}

/*******************************************************************************
* Name: readFileInFolder
* Function: read information of file in folder
* Parameters: @uint32_t index
              @uint8_t num
* Return:   
              @void
******************************************************************************/
void readFileInFolder(uint32_t index, uint8_t num)
{
    uint8_t *current;
    uint32_t count = 0;
    rootEntry *newNode;
    buff = (uint8_t*)malloc(num * SECTOR_SIZE * sizeof(uint8_t));
    HAL_ReadMultiSector(index, num, buff);
    for(count = 0; count < num * SECTOR_SIZE; count += SIZE_ROOT_ENTRY)
    {
        current = buff + count; 
        newNode = createNewNodeAsRootEntry(current);
        if(newNode->checkSum)
        {
            if(newNode->fileAttributes == IS_THIS_FODER || newNode->fileAttributes == IS_THIS_FILE)
            {
                addRootEntryInList(newNode);
            }       
            else
            {
                free(newNode);
            }
        }
    }
    free(buff);
}

/*******************************************************************************
* Name: readRootRegion
* Function: read information of root region
* Parameters: 
* Return:   
              @void
******************************************************************************/
void readRootRegion(void)
{
    uint8_t sectorStartRootRegion = bootSector.numReseredSec + (bootSector.numSecPerFAT * bootSector.numFatTable);
    uint16_t sizeRootRegion = bootSector.numRootDirectoryEntries * SIZE_ROOT_ENTRY / SECTOR_SIZE;
    readBootRegion();
    readFileInFolder(sectorStartRootRegion, sizeRootRegion);
}

/*******************************************************************************
* Name: resetLinkedList
* Function: clear linked list
* Parameters: 
* Return:   
              @void
******************************************************************************/
void resetLinkedList(void)
{
    rootEntry *p;
    p = head;
    while(p != NULL)
    {
        p = p->next;
        free(head);
        head = p;
    }
}

/*******************************************************************************
* Name: openFolder
* Function: open file/folder in folder
* Parameters: @uint32_t positionFileInFolder: position of file in folder
* Return:   
              @void
******************************************************************************/
void openFolder(uint32_t positionFileInFolder)
{
    uint32_t startSectorOfDataRegion = 1 + bootSector.numSecPerFAT * bootSector.numFatTable \
                                        + bootSector.numRootDirectoryEntries * SIZE_ROOT_ENTRY / SECTOR_SIZE - FIRST_CLUSTER;
    rootEntry *current;
    current = head;
    uint32_t count = 1;
    uint32_t i;
    uint8_t typeFile;
    uint64_t currentCluster;
    while(current != NULL)
    {
        if(count == positionFileInFolder)
        {
            if(current->fileAttributes == IS_THIS_FODER && current->startCluster != 0)
            {
                resetLinkedList();
                readFileInFolder(current->startCluster + startSectorOfDataRegion, 1);
            }            
            else if (current->fileAttributes == IS_THIS_FODER && current->startCluster == 0)
            {
                resetLinkedList();
                readRootRegion();
            }            
            else if (current->fileAttributes == IS_THIS_FILE)
            {
                system("cls");
                typeFile = typeOfFile(current->fileType);
                currentCluster = current->startCluster;
                do
                {
                    buff = (uint8_t*)malloc(SECTOR_SIZE * sizeof(uint8_t));
                    HAL_ReadSector(currentCluster + startSectorOfDataRegion, buff);
                    if(typeFile == DOC_OR_TXT)
                    {
                        printf(" \n\n%s", buff);
                    }
                    else if(typeFile == PNG_OR_PDF)
                    {
                        for(i = 0;i < SECTOR_SIZE;i++)
                        {
                            printf("%0.2x ",buff[i]);
                        }
                    }           
                    currentCluster = addressNextCluster(currentCluster);
                    free(buff); 
                } 
                while (currentCluster != END_OF_FILE);
                printf(" \n\nPress any key to continue");
                getch();
            }            
        }   
        count++;
        current = current->next;
    }
}

/*******************************************************************************
* Name: showFileInFolder
* Function: show information file/folder in folder
* Parameters: 
* Return:   @void
******************************************************************************/
uint8_t showFileInFolder(void)
{
    uint8_t i;
    rootEntry *current;
    uint32_t count = 1;
    uint16_t day;
    uint16_t month;
    uint16_t year;
    uint16_t sec;
    uint16_t min;
    uint16_t hour;
    current = head;
    printf("\nNumOrder   Name           Type       Date modified         Time modified         Size\n");
    while(current != NULL)
    {
        printf("\n%d.         ", count);
        /*Print name of file in folder*/
        for(i = 0;i < 8;i++){
            printf("%c", current->fileName[i]);
        }
        printf("       ");
        /*Print type of file*/
        for(i = 0;i < 3;i++)
        {
            printf("%c", current->fileType[i]);
        }
        /*Print date modified*/
        day = ((current->dateModified) >> SHIFT_DAY) & MASK_DAY;
        month = ((current->dateModified) >> SHIFT_MONTH) & MASK_MONTH ;
        year  = (((current->dateModified) >> SHIFT_YEAR) & MASK_YEAR) + FIRST_YEAR;
        printf("        %0.2d/%0.2d/%0.4d         ", day, month, year);
        /*Print time modified*/
        sec = ((current->timeModified) >> SHIFT_SECOND) & MASK_SECOND;
        min = ((current->timeModified) >> SHIFT_MINUTE) & MASK_MINUTE;
        hour = ((current->timeModified) >> SHIFT_HOUR) & MASK_HOUR; 
        printf("    %0.2d:%0.2d:%0.2d           ", hour, min, sec);
        /*Print size of file*/
        printf("  %0.2d Byte         ", current->sizeFile);
        count++;
        current = current->next;
    }
    printf("\n%d.       Exit program!\n", count);
    return count;
}
