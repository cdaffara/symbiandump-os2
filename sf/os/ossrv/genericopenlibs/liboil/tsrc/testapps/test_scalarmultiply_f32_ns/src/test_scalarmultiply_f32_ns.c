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

#define LOG_FILE "c:\\logs\\testapps_scalarmultiply_f32_ns_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testapps_scalarmultiply_f32_ns");
    close_log_file();
}

int check(float multiplier)
{
	float dest[1024], src[1024];
	int i;
	
	for(i=0; i<1024; i++)
		src[i] = i*3.4f;
	//Multiplies the constant value multiplier and each source element in src and places the result in dest.
	oil_scalarmultiply_f32_ns(dest, src, &multiplier, 1024);
	for(i=0; i<1024; i++)
	{
		float res = src[i]*multiplier;
		if(dest[i] != res){
			assert_failed = 1;
			std_log(LOG_FILENAME_LINE, "Failed for Multiplier=%f, Iteration=%d, dest[i]=%f, src[i]=%f\n", multiplier, i, dest[i], src[i]);
		}
	}
}

int main(int argc, char* argv[])
{
	float multiplier[] = {-0.3e2, -4567.87e-36, 0, 12.45e-9, 890.3e32, 9876543.767890};
	int i;
	
	std_log(LOG_FILENAME_LINE, "Test Started testapps_scalarmultiply_f32_ns");

	oil_init();
	for(i=0; i<6; i++)
		check(multiplier[i]);
	if(assert_failed)
		std_log(LOG_FILENAME_LINE, "Test Failed.\n");
	else
		std_log(LOG_FILENAME_LINE, "Test Passed.\n");
	
	create_xml(0);
	return 0;
}
