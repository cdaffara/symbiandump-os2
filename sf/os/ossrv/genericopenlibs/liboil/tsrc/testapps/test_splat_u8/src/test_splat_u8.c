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
#include <liboil/globals.h>

#define LOG_FILE "c:\\logs\\test_splat_u8_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("test_splat_u8");
    close_log_file();
}

int flag = 0;

int check(uint8_t val, uint8_t dstr)
{
	int i;
	int num = 4095/dstr;
	uint8_t dest_arr[4096] = {0};
	//Copies the constant source value val to each element in dest_arr with dstr as stride distance.
	oil_splat_u8(dest_arr, dstr, &val, num+1);
	for(i=0; i<4096; i++){
		if(i%dstr == 0)
			flag |= (dest_arr[i] == val)? 0 : 1;
		else
			flag |= (dest_arr[i] == 0)? 0 : 1;
		if (flag)
			std_log(LOG_FILENAME_LINE, "Failed at :: i=%d, num=%d, dest_arr[i]=%d, dstr=%d\n", i, num, dest_arr[i], dstr);
	}
}

int main(int argc, char* argv[])
{
	uint8_t dstr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	uint8_t test_arr[] = {0, 1, 127, 254, 255};
	int j, i;
	
	std_log(LOG_FILENAME_LINE, "Test Started test_splat_u8");

	oil_init();
	for(j=0; j<10; j++)
		for(i=0; i<5; i++)
			check(test_arr[i], dstr[j]);
	
	if(flag){
		std_log(LOG_FILENAME_LINE, "Test Fail");
		assert_failed = 1;
	}
	else
		std_log(LOG_FILENAME_LINE, "Test Pass");
	
	create_xml(0);
	return 0;
}
