#include "os.h"
#include "ml_filesystem.h"

uint8_t recentChanges(FATFS *FatFs, FIL *file, char *filename, uint8_t linecount)
{
	char line[64]; 			/* Line buffer */
	uint8_t linesRead = 0;
	FRESULT fresult;	

	f_mount(FatFs, "", 0);

	fresult = f_open(file, filename, FA_READ);
	if(fresult == FR_OK)
	{
		/* First pass counts the lines. */
		while (f_gets(line, sizeof line, file))
			linesRead++;

		/* Second pass reads from startPointer to end of file. */
		uint8_t nthLine = 0;
		if(linesRead > linecount)
			nthLine = linesRead - linecount;
		linesRead = 0;

		fresult = f_lseek(file, 0); /* Move to start of file */
		while(f_gets(line, sizeof line, file))
		{
			if(linesRead++ >= nthLine)
				ml_printf("%u: %s", linesRead, line);
		}
		
		ml_printf("Finished reading file.");

		return fresult;
	}
	else
	{
		return (uint8_t)fresult;
	}
}
