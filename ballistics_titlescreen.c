#include "ballistics_titlescreen.h"

void draw_titlescreen(FATFS *FatFs)
{
//	uint16_t key[37] ={0x8800,0xfc60,0x435e,0xf7bf,0x1f,0xc,0x866f,0x64af,0xfa00,0xd6b6,0xe,0xf7b7,0x6b4b,0xbdfd,0xad6d,0x5ff,0xf800,0x2461,0xf7f7,0x420,0x9e71,0x300,0x9774,0x8dec,0x18d3,0x49cc,0x839f,0x8424,0xcc22,0xffe0,0xdefe,0x6ace,0x7fb,0x5351,0x347a,0x3671,0x24bf};

	uint16_t key[37] = {0x8800,0xfc60,0x435b,0xf7bf,0x1f,0x11,0x867d,0x64bd,0xfa00,0xd6ba,0x19,0xf7be,0x6b4d,0xbdf7,0xad75,0x5ff,0xf800,0x2464,0xf7fe,0x420,0x9e66,0x300,0x9772,0x8df1,0x18ce,0x49d1,0x839f,0x8430,0xcc28,0xffe0,0xdefb,0x6ad9,0x7ef,0x5346,0x346b,0x3666,0x24bf};
	FRESULT fresult;
	FIL file;

	f_mount(FatFs, "", 0);

        fresult = f_open(&file, "myfile.txt", FA_READ);
        if(!fresult)
        {
		char header[20];

                /* Read the pattern header: "XXXYYYLLL" */
                while (f_gets(header, sizeof header, &file))
		{
			unsigned x, y, length;
			sscanf(header, "%3u%3u%3u", &x, &y, &length);

			/* Read the colour indexes from the data "II*Length" */
			char contents[1000];
                        if(f_gets(contents, sizeof contents, &file))
			{
				unsigned i;
				for(i = 0; i < length; i++)
				{
					unsigned colour_index;
					if(sscanf(contents + i * 2, "%2u", &colour_index) != 0)
					{
						draw_pixel(x + i, y, key[colour_index]);
					}
				}
			}
			else
				draw_pixel(319, 239, 0xFFFF);
		}

		f_close(&file);
        }
	else
	{
		ml_printf_at("BALLISTIC BREAKER BOTS v1.0.0\n", 0, 0);
		ml_printf("Error: %d", (int)fresult);
	}
}
