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

#define TEST_ARRAY_SIZE 12

#define LOG_FILE "c:\\logs\\test_merge_linear_u8_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("test_merge_linear_u8");
    close_log_file();
}

int main(int argc, char* argv[])
{
	uint8_t src1[] = {0,   0,   1, 1, 100, 190,  12, 145, 254,   0, 255, 255};
	uint8_t src2[] = {255, 0, 255, 1, 128, 234, 130, 160, 254, 254, 255,   0};
	uint8_t dest[TEST_ARRAY_SIZE];
	uint8_t linux_result[] = {  0,   0,   1,   1, 100, 190,  12, 145, 254,   0, 255, 255,
                                0,   0,   1,   1, 100, 190,  12, 145, 254,   0, 255, 254, 
                                126,   0, 127,   1, 113, 211,  70, 152, 254, 126, 255, 128, 
                                199,   0, 199,   1, 121, 224, 104, 156, 254, 198, 255,  55,  
                                79,   0,  80,   1, 108, 203,  48, 149, 254,  79, 255, 175, 
                                253,   0, 253,   1, 127, 233, 129, 159, 254, 252, 255,   1, 
                                254,   0, 254,   1, 127, 233, 129, 159, 254, 253, 255,   0, };
	
	uint32_t src3[] = {0, 1, 127, 200, 80, 254, 255};
	int i, j;
	
	std_log(LOG_FILENAME_LINE, "Test started test_merge_linear_u8");
	oil_init();

	for(i=0; i<(sizeof(src3)/sizeof(uint32_t)); i++){
		oil_merge_linear_u8(dest, src1, src2, &src3[i], TEST_ARRAY_SIZE);
		for(j=0; j<TEST_ARRAY_SIZE; j++){            
            if(linux_result[i*TEST_ARRAY_SIZE + j] != dest[j]){
                std_log(LOG_FILENAME_LINE,"Failed at :: For src3=%d iteration=%d\t j=%d\tsrc1[j]=%04d\tsrc2[j]=%04d\tdest[j]=%04d\n", src3[i], i, j, src1[j], src2[j], dest[j]);
                assert_failed = 1;
            }
		}
	}
	
	if(assert_failed)
	    std_log(LOG_FILENAME_LINE,"Test Fail");
	else
	    std_log(LOG_FILENAME_LINE,"Test Successful");
	create_xml(0);
	return 0;
}
