
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

typedef struct {
   char* argv[80];
   int argc;
} UserArgs;

int my_open(UserArgs* userArgs, char* sector, char* names)
{
   int i;
   int x = 0;
   int result;
   int high_offset = 0;
   int low_offset = 0;

   unsigned int DIR_FstClusHI[2];
   unsigned int DIR_FstClusLO[2];

   int ClusNum[4];

   // Search through the directory names to make sure that the file exists 
   for(i = 0; i < 16; i++)
   {
      if(names[i] == userArgs->argv[1])
      {
	x = 1;
	result = i; 
      }
   }

   if(x == 0)
   {
	return -1;
   }else{
      // Get the high word and store in little endian order
      high_offset = (result * 32) + 20; 
      DIR_FstClusHI[1] = sector[high_offset + 1];
      DIR_FstClusHI[0] = sector[high_offset];

      // Get the low word and store in little endian order
      low_offset = (result * 32) + 26;
      DIR_FstClusLO[1] = sector[low_offset + 1];
      DIR_FstClusLO[0] = sector[low_offset];

      // Store the two values in the same array in the correct order
      ClusNum[0] = DIR_FstClusHI[0];
      ClusNum[1] = DIR_FstClusHI[1];
      ClusNum[2] = DIR_FstClusLO[0];
      ClusNum[3] = DIR_FstClusLO[1];

      // Convert stored array by modifying bits and then converting to hex
      // Get the cluster number
      result = bitFlip(ClusNum, 4);

      return result;
   }
}
int my_create(UserArgs* userArgs)
{

   return 0;
}
int my_rm(UserArgs* userArgs)
{

   return 0;
}
int my_size(UserArgs* userArgs)
{

   return 0;
}
int my_cd(UserArgs* userArgs)
{

   return 0;
}
int my_ls(UserArgs* userArgs)
{

   return 0;
}
int my_mkdir(UserArgs* userArgs)
{

   return 0;
}
int my_rmdir(UserArgs* userArgs)
{

   return 0;
}
int my_readFile(UserArgs* userArgs)
{

   return 0;
}
int my_writeFile(UserArgs* userArgs)
{

   return 0;
}
int my_close(UserArgs* userArgs)
{

   return 0;
}

char* my_read()
{
   char* res = NULL;
   char in[256];

   fgets(in, sizeof in, stdin);

   if (in[strlen(in)-1] == '\n')
   {
    in[strlen(in)-1] = in[strlen(in)];
   }

   res = (char*)malloc(strlen(in)+1);
   strcpy(res, in);

   return res;
}

UserArgs* my_parse(char* line)
{
   int temp = strlen(line);

   UserArgs* args = NULL;
   args = malloc(sizeof(UserArgs));
   args->argc = 0;

   char * current = NULL;

   current = (char*)malloc(temp);

   current = strtok(line, " ");
   while (current != NULL)
   {
    args->argv[args->argc] = strdup(current);
    args->argc += 1;
    current = strtok(NULL, " ");
   }
   args->argv[args->argc] = NULL;

   free(current);

   return args;
}

void my_prompt(char* dir)
{
   printf("user:/%s/>", dir);
}

int main()
{
    FILE * image;

    char bootSector[512];
    char rootDir[512];
    char dirNames[16];
    char dirInfo[32];
    char * currentDir = (char*)malloc(4);

    currentDir[0] = 'm';
    currentDir[1] = 'n';
    currentDir[2] = 't';
    currentDir[3] = '\0';

    unsigned int BPB_BytesPerSec;
    unsigned int BPB_SecPerClus;
    unsigned int BPB_RsvdSecCnt;
    unsigned int BPB_NumFATS;
    unsigned int BPB_FATSz32;
    unsigned int BPB_RootClus;

    int FirstDataSector;
    int FirstSectorofCluster;
    int offset;
    int count = 0;
    int z;
    int y = 11;
    int q;
    int v;
    int h;
    int N;

    image = fopen("fat32.img", "r");   // Open the image

    fread(bootSector, 1, 512, image);  // Read the boot sector into a buffer

    // Store important values from the boot sector
    BPB_BytesPerSec = getValue(bootSector, 11, 2);
    BPB_SecPerClus = getValue(bootSector, 13, 1);
    BPB_RsvdSecCnt = getValue(bootSector, 14, 2);
    BPB_NumFATS = getValue(bootSector, 16, 1);
    BPB_FATSz32 = getValue(bootSector, 36, 2);
    BPB_RootClus = getValue(bootSector, 44, 4);


    // Calculate the starting location of the root directory

    FirstDataSector = BPB_RsvdSecCnt + (BPB_NumFATS * BPB_FATSz32);

    FirstSectorofCluster = ((BPB_RootClus - 2) * BPB_SecPerClus) + FirstDataSector;

    offset = FirstSectorofCluster * BPB_BytesPerSec;


    // Move the file pointer to the starting location of the root directory
    fseek(image, offset, SEEK_SET);

    // Read the root directory into a new buffer
    fread(rootDir, 1, BPB_BytesPerSec, image);

    // Print out the names of the files and directories
    for(z = 0; z < BPB_BytesPerSec; z += 32)
    {
    	v = z + 11;

    	if((rootDir[v] != 15))
    	{
            	for(q=z;q<z+11;q++){
                    printf("%c",rootDir[q]);
		    // Store the directory names into an array
   		    dirNames[z] = dirNames[z] + rootDir[q];
            	}
            	printf("\n");
   	}
    }

    char * line;

    UserArgs* userArgs = malloc(sizeof(UserArgs*));

    while (exit)
    {
   	 my_prompt(currentDir);
   	 line = my_read();

   	 if (strlen(line) == 0)
   		return;

   	 userArgs = my_parse(line);

   	 if (strcmp(userArgs->argv[0], "exit") == 0)
   	 { return; }

   	 else if (strcmp(userArgs->argv[0], "open") == 0)
   	 {
   	  	if (my_open(userArgs, rootDir, dirNames) > 0)
   	  	{
   		 printf("Error: File did not open\n");
   		 continue;
   	  	}

		int FirstSectorofOpen;

		// Plug the first sector cluster of the file into N
		FirstSectorofOpen = ((BPB_RootClus - (my_open(userArgs, rootDir, dirNames))) * BPB_SecPerClus) + FirstDataSector;

		// Print the contents out in a for loop? 
   	 }
   	 else if (strcmp(userArgs->argv[0], "close") == 0)
   	 {
   	  	if (my_close(userArgs) != 0)
   	  	{
   		 printf("Error: File did not close\n");
   	  	}
   	  	// check the boolean structure of the list of open files
   	  	// if it's in there, remove it
   	  	// if it's not, return an error
   	 }

   	 else if (strcmp(userArgs->argv[0], "create") == 0)
   	 {
   	  	if (my_create(userArgs) != 0)
   	  	{
   		 printf("Error: File was not created\n");
   	  	}

   	 }

   	 else if (strcmp(userArgs->argv[0], "rm") == 0)
   	 {
   	  	if (my_rm(userArgs) != 0)
   	  	{
   		 printf("Error: File was not removed\n");
   	  	}
   	 }

   	 else if (strcmp(userArgs->argv[0], "size") == 0)
   	 {
   	  	if (my_size(userArgs) != 0)
   	  	{
   		 printf("Error: Size not found\n");
   	  	}


   	 }

   	 else if (strcmp(userArgs->argv[0], "cd") == 0)
   	 {
   	  	if (my_cd(userArgs) != 0)
   	  	{
   		 printf("Error: Directory not changed\n");
   	  	}

   	 }

   	 else if (strcmp(userArgs->argv[0], "ls") == 0)
   	 {
   	  	if (my_ls(userArgs) != 0)
   	  	{
   		 printf("Error: No information found\n");
   	  	}
   	 }

   	 else if (strcmp(userArgs->argv[0], "mkdir") == 0)
   	 {
   	  	if (my_mkdir(userArgs) != 0)
   	  	{
   		 printf("Error: Directory not made\n");
   	  	}


   	 }

   	 else if (strcmp(userArgs->argv[0], "rmdir") == 0)
   	 {
   	  	if (my_rmdir(userArgs) != 0)
   	  	{
   		 printf("Error: Director not removed\n");
   	  	}

   	 }

   	 else if (strcmp(userArgs->argv[0], "read") == 0)
   	 {
   	  	if (my_readFile(userArgs) != 0)
   	  	{
   		 printf("Error: File not read\n");
   	  	}

   	 }

   	 else if (strcmp(userArgs->argv[0], "write") == 0)
   	 {
             	if (my_writeFile(userArgs) != 0)
   	  	{
   		 printf("Error: File not written to\n");
   	  	}

   		 // not sure how to implement this one
   	 }


    }

    free(line);
    free(currentDir);

    return 0;

}

// Search through a buffer for values with a certain offset and size
int getValue(char* sector, int offset, int size)
{
    int res;
    int i;
    int x;
    int e;

    unsigned char * hex_array;

    hex_array = (unsigned char*)malloc(size*2);

    // Store values from the buffer into the array in their original hex value
    // Stored in little endian order
    for(i = (size - 1); i >= 0; i--)
    {
   	hex_array[i] = sector[offset];
   	offset = offset + 1;
    }

    res = bitFlip(hex_array, size);

    return res;
}

int bitFlip(unsigned char* hex_array, int size)
{
    int res;
    int e, c, d;
    int f = 0;

    unsigned char * dec_array;

    unsigned char a = 15;
    unsigned char b = 240;

    dec_array = (unsigned char*)malloc(size);

    // Use bitwise operators to modify the binary values correctly
    for(e = 0; e < size; e++)
    {
   	c = hex_array[e] & a;
   	d = (hex_array[e] & b) >> 4;
   	dec_array[f + 1] = c;
   	dec_array[f] = d;
   	f = f + 2;
    }

    res = convert_dec(dec_array, size*2);

    return res;
}

// Convert values from their modified binary to the correct decimal
int convert_dec(unsigned char * dec_array, int size)
{
   int a;
   int temp;
   int expo = size - 1;
   int mult = 1;


   int result = dec_array[size - 1];

   for(a = expo -1; a > -1; a--)
   {
    temp = dec_array[a];

    mult = mult * 16;

    result = result + (temp * mult);
   }

   return result;

}

