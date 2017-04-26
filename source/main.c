
#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

const char * getRegion()
{
    const char *regions[] = 
	{
        "JPN",
        "USA",
        "EUR",
        "AUS",
        "CHN",
        "KOR",
        "TWN",
        "Unknown"
    };

    u8 region = 0;
    CFGU_SecureInfoGetRegion(&region);

    if (region < 7)
        return regions[region];
    else
        return regions[7];
}

u32 titleCount(FS_MediaType mediaType)
{
	u32 count = 0;
	
	AM_GetTitleCount(mediaType, &count);

    return count;
}

/* credit to https://github.com/videah or wherever they got the ascii art from */
char const *asciiart = 
"\n\n\n\n\n\n"
"                 ######\n"
"               ####  ####\n"
"              #####  #####\n"
"             ## ##    ## ##\n"
"             ##          ##\n"
"            ####        ####\n"
"            ####  ####  ####\n"
"            #### ###### ####\n"
"            #### ###### ####\n"
"            ###  ######  ###\n"
"            #     ####     #\n"
"            #  ##########  #\n"
"            ####  #  #  ####\n"
"             ##   #  #   ## \n"
"              #          #\n"
"              ##        ##\n"
"               ##########\n";

char const *specs[5][6] = {"O3DS", "O3DS XL", "N3DS", "2DS", "N3DS XL", "Unknown",
							"3.53in @ 800x240px", "4.88in @ 800x240px", "3.88in @ 800x240px", "3.53in @ 800x240px", "4.88in @ 800x240px", "Unknown",
							"3.00in @ 320x240px", "4.18in @ 320x240px", "3.33in @ 320x240px", "3.00in @ 320x240px", "4.18in @ 320x240px", "Unknown",
							"ARM11 2x MPCore 268MHz", "ARM11 2x MPCore 268MHz", "ARM11 4x MPCore 804MHz","ARM11 2x MPCore 268MHz",  "ARM11 4x MPCore 804MHz", "Unknown",
							"128MB", "128MB","256MB", "128MB", "256MB", "Unknown"};

int main(int argc, char **argv)
{
	gfxInitDefault();
	cfguInit();
	amInit();
	fsInit();
	
	PrintConsole topScreen, bottomScreen;
	consoleInit(GFX_TOP, &topScreen);
	consoleInit(GFX_BOTTOM, &bottomScreen);
	
	bool didit = false;
	consoleSelect(&topScreen);
	printf("\x1b[14;16HPress A to start.\n\x1b[16;15H(Max 20 characters)");


	while (aptMainLoop())
	{
		hidScanInput();

		u32 kDown = hidKeysDown();

		if (kDown & KEY_START) break;
		
		static SwkbdState swkbd;
		static char mybuf[21];
		SwkbdButton button = SWKBD_BUTTON_NONE;
	
		if (kDown & KEY_A && !didit)
		{
		consoleInit(GFX_TOP, &topScreen);
		swkbdInit(&swkbd, SWKBD_TYPE_WESTERN, 1, -1);
		swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, SWKBD_FILTER_BACKSLASH, 1);
		swkbdSetHintText(&swkbd, "Please type in a username");
		button = swkbdInputText(&swkbd, mybuf, sizeof(mybuf));
		didit = true;
		}
	
		if (button != SWKBD_BUTTON_NONE && didit) 
		{
			/* variables */
			u32 os_ver = osGetKernelVersion();
			/* Thanks @joel16 for some of these functions */
			char *str_ver = (char *)malloc(sizeof(char) * 255), *str_sysver = (char *)malloc(sizeof(char) * 255);
			OS_VersionBin *nver = (OS_VersionBin *)malloc(sizeof(OS_VersionBin)), *cver = (OS_VersionBin *)malloc(sizeof(OS_VersionBin));
			s32 ret;
			memset(nver, 0, sizeof(OS_VersionBin));
			memset(cver, 0, sizeof(OS_VersionBin));
			ret = osGetSystemVersionData(nver, cver);
			u8 model = 0;
			CFGU_GetSystemModel(&model);
			u32 installedTitles = titleCount(MEDIATYPE_SD);
			FS_ArchiveResource	resource = {0};
			
			
			/* Does it count as code if it looks like pain */
			consoleSelect(&bottomScreen);
			printf("\x1b[0;11H%s", asciiart);
			
			consoleSelect(&topScreen);
			printf("\x1b[3;8H%s@3ds", mybuf);
			printf("\x1b[5;8H\x1b[34mModel: \x1b[0m%s - %s", specs[0][model], getRegion());

			if (ret)
				printf("\x1b[33;1m*\x1b[0m osGetSystemVersionData returned 0x%08liX\n\n", ret);

			snprintf(str_sysver, 100, "\x1b[7;8H\x1b[34mSystem Version: \x1b[0m%d.%d.%d-%d",
					cver->mainver,
					cver->minor,
					cver->build,
					nver->mainver
					);

			if (!ret) printf(str_sysver);
			
			snprintf(str_ver, 255, "\x1b[9;8H\x1b[34mKernel version: \x1b[0m%lu.%lu-%lu",
			GET_VERSION_MAJOR(os_ver),
			GET_VERSION_MINOR(os_ver),
			GET_VERSION_REVISION(os_ver)
			);

			printf(str_ver);

			printf("\x1b[11;8H\x1b[34mPackages: \x1b[0m%i", (int)installedTitles);
			
			printf("\x1b[13;8H\x1b[34mUpper Screen: \x1b[0m%s", specs[1][model]);
			printf("\x1b[15;8H\x1b[34mLower Screen: \x1b[0m%s", specs[2][model]);
			
			printf("\x1b[17;8H\x1b[34mGPU: \x1b[0mDMP PICA 268MHz");
			
			printf("\x1b[19;8H\x1b[34mARM9 CPU: \x1b[0mARM946 134MHz");
			printf("\x1b[21;8H\x1b[34mARM11 CPU: \x1b[0m%s", specs[3][model]);
			
			printf("\x1b[23;8H\x1b[34mFC RAM: \x1b[0m%s", specs[4][model]);
			
			FSUSER_GetArchiveResource(&resource, SYSTEM_MEDIATYPE_SD);
			printf("\x1b[25;8H\x1b[34mSD Space: \x1b[0m%.1fMB/%.1fMB",
			(((u64) resource.freeClusters * (u64) resource.clusterSize) / 1024.0 / 1024.0),
			(((u64) resource.totalClusters * (u64) resource.clusterSize) / 1024.0 / 1024.0));
			
			free(nver);
			free(cver);
			free(str_ver);
			free(str_sysver);
			
			
		
			
		}
			
		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();

		gspWaitForVBlank();
	}
	gfxExit();
	cfguExit();
	amExit();
	fsExit();
	return 0;
}
