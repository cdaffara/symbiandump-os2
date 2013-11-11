/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <liboil/liboil.h>
#include <limits.h>
#include <liboil/globals.h>

#define LOG_FILE "c:\\logs\\test_splat_u8_ns_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("test_splat_u8_ns");
    close_log_file();
}

int check(uint8_t val)
{
    uint8_t dest_arr[4096] = {0};
	int i;
	//Copies the constant source value val to each element in dest_arr.
	oil_splat_u8_ns(dest_arr, &val, 4096);
	for(i=0; i<4096; i++){
		if(dest_arr[i] != val){
            std_log(LOG_FILENAME_LINE, "Failed at iteration = %d, val=%d, dest_arr[i]=%d\n", i, val, dest_arr[i]);
            create_xml(1);
            exit(0);
		}
	}
}

int main(int argc, char* argv[])
{
	uint8_t test_arr[] = {0, 1, 127, 254, 255};	
	int i;
	
	std_log(LOG_FILENAME_LINE, "Test Started test_splat_u8_ns");
	
	oil_init();
	for(i=0; i<sizeof(test_arr); i++)
		check(test_arr[i]);

	std_log(LOG_FILENAME_LINE, "Test Pass\n");
	create_xml(0);
	return 0;
}
