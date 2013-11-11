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
#define TEST_ARRAY_SIZE 16

#define LOG_FILE "c:\\logs\\test_merge_linear_argb_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("test_merge_linear_argb");
    close_log_file();
}

int main(int argc, char* argv[])
{
	uint32_t src1[] = {0x00000000, 0x00000000, 0xFF00FF00, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xF0F0F0F0, 0x0F0F0F0F, 0xFFFFFFFF, 0xFFFFFFFF, 0xABCD0123, 0x45678901, 0x1A2B3C4D, 0x8A9BD1CE, 0xABC0000F, 0xFFFF0000};
	uint32_t src2[] = {0x00000000, 0xFF000000, 0x00FF0000, 0x0000FF00, 0xF0F000FF, 0xF0F0F0F0, 0x0F0F0F0F, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0xBCDE1234, 0x00000000, 0x09876543, 0x0F0A0BFF, 0x6743B0AA, 0x0000FFFF};
	uint32_t dest[TEST_ARRAY_SIZE];
	uint32_t src3[] = {0, 1, 127, 200, 80, 254, 255};
	uint32_t linux_result[] = { 0x00000000, 0x00000000, 0xff00ff00, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xf0f0f0f0, 0x0f0f0f0f, 0xffffffff, 0xffffffff, 0xabcd0123, 0x45678901, 0x1a2b3c4d, 0x8a9bd1ce, 0xabc0000f, 0xffff0000, 
                                0x00000000, 0x00000000, 0xfe00fe00, 0x00fe0000, 0x0000fe00, 0x000000fe, 0xefefefef, 0x0f0f0f0f, 0xfefefefe, 0xffffffff, 0xabcd0123, 0x44668800, 0x192b3c4c, 0x899ad0ce, 0xaabf000f, 0xfefe0000, 
                                0x00000000, 0x7e000000, 0x807e8000, 0x00807e00, 0x7777807e, 0x777777f7, 0x80808080, 0x86868686, 0x80808080, 0xffffffff, 0xb3d5092b, 0x22334500, 0x11585048, 0x4c536ee6, 0x8981575b, 0x80807e7e, 
                                0x00000000, 0xc7000000, 0x37c73700, 0x0037c700, 0xbbbb37c7, 0xbbbbbbf3, 0x40404040, 0xcacacaca, 0x37373737, 0xffffffff, 0xb8da0e30, 0x0f161d00, 0x0c725c45, 0x292936f4, 0x755e8988, 0x3737c7c7, 
                                0x00000000, 0x4f000000, 0xaf4faf00, 0x00af4f00, 0x4b4baf4f, 0x4b4b4bfa, 0xa9a9a9a9, 0x5a5a5a5a, 0xafafafaf, 0xffffffff, 0xb0d20628, 0x2f465e00, 0x14474849, 0x636d93dd, 0x9598373f, 0xafaf4f4f, 
                                0x00000000, 0xfd000000, 0x01fd0100, 0x0001fd00, 0xeeee01fd, 0xeeeeeef0, 0x10101010, 0xfdfdfdfd, 0x01010101, 0xffffffff, 0xbbdd1133, 0x00000100, 0x09866443, 0x0f0b0cfe, 0x6743aea8, 0x0101fdfd, 
                                0x00000000, 0xfe000000, 0x00fe0000, 0x0000fe00, 0xefef00fe, 0xefefeff0, 0x0f0f0f0f, 0xfefefefe, 0x00000000, 0xffffffff, 0xbbdd1133, 0x00000000, 0x09866443, 0x0f0a0bfe, 0x6743afa9, 0x0000fefe, };
	int i, k;

	std_log(LOG_FILENAME_LINE, "Test started test_merge_linear_argb");
	oil_init();
	
    for(i=0; i<(sizeof(src3)/sizeof(uint32_t)); i++){
        oil_merge_linear_argb(dest, src1, src2, &src3[i], TEST_ARRAY_SIZE);
        for(k=0; k<TEST_ARRAY_SIZE; k++){
            if(linux_result[i*TEST_ARRAY_SIZE + k] != dest[k]){
                std_log(LOG_FILENAME_LINE, "Failed at : src3=0x%x iteration=%d \t k=%d\tsrc1[k]=0x%x\tsrc2[k]=0x%x\tdest[k]=0x%x,\t", src3[i], i, k, src1[k], src2[k], dest[k]);
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
