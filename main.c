#include"FAT.h"

int main(void)
{
    uint32_t positionFileInFolder;
    uint8_t count = 0;
    uint8_t flag = 1;
    readBootRegion();
    readRootRegion();
    showFileInFolder();

    while(flag == 1)
    {
        printf("\nChoose the position of file you want to open:\n");
        scanf("%d", &positionFileInFolder);
        openFolder(positionFileInFolder);
        if(positionFileInFolder == count)
        {
            flag = 0;
        }
        else
        {
            system("cls");
            count = showFileInFolder();
            printf("\n\nPress any key to continue");
            getch();
        }
    }
    return 0;
}
