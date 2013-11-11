// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <liboil/liboil.h>
#include <liboil/liboilfunction.h>
#include <stdio.h>
#include <string.h>
#include <globals.h>

#define LOG_FILE "c:\\logs\\testsuite_recon8x8_log.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#define MAX_SIZE 64

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_recon8x8");
    close_log_file();
}

void test_recon8x8_intra()
    {
    int16_t src[MAX_SIZE];
    uint8_t dest[MAX_SIZE],check[MAX_SIZE]={138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201};
    int i;
    
    for(i=0;i<MAX_SIZE;i++)
        {
         src[i]=i+10;
         dest[i]=0;
        }
    oil_recon8x8_intra(dest,8,src);
    
    for(i=0;i<MAX_SIZE;i++)
        if(check[i] == dest[i])
            {
            std_log(LOG_FILENAME_LINE,"oil_recon8x8_intra successful, dest[%d] = %d",i,dest[i]);
            }
        else
            {
            assert_failed=1;
            std_log(LOG_FILENAME_LINE,"oil_recon8x8_intra unsuccessful, Expected =%d,Obtained =%d",check[i],dest[i]);
            }
    }


void test_recon8x8_inter()
    {
    int16_t src2[MAX_SIZE];
    uint8_t src1[MAX_SIZE],dest[MAX_SIZE],check[MAX_SIZE]={5,7,9,11,13,15,17,19,15,17,19,21,23,25,27,29,25,27,29,31,33,35,37,39,35,37,39,41,43,45,47,49,45,47,49,51,53,55,57,59,55,57,59,61,63,65,67,69,65,67,69,71,73,75,77,79,75,77,79,81,83,85,87,89};
    int i;
    
    for(i=0;i<MAX_SIZE;i++)
        {
         src1[i]=i+3;
         src2[i]=i+2;
         dest[i]=0;
        }
    oil_recon8x8_inter(dest,8,src1,2,src2);
    
    for(i=0;i<MAX_SIZE;i++)
        if(check[i] == dest[i])
            {
            std_log(LOG_FILENAME_LINE,"oil_recon8x8_inter successful, dest[%d] = %d",i,dest[i]);
            }
        else
            {
            assert_failed=1;
            std_log(LOG_FILENAME_LINE,"oil_recon8x8_intra unsuccessful, Expected =%d,Obtained =%d",check[i],dest[i]);
            }
    }

void test_recon8x8_inter2()
    {
    uint8_t src1[MAX_SIZE],src2[MAX_SIZE];
    int16_t src3[MAX_SIZE];
    uint8_t dest[MAX_SIZE],check[MAX_SIZE]={2,4,6,8,10,12,14,16,12,14,16,18,20,22,24,26,21,23,25,27,29,31,33,35,31,33,35,37,39,41,43,45,40,42,44,46,48,50,52,54,50,52,54,56,58,60,62,64,59,61,63,65,67,69,71,73,69,71,73,75,77,79,81,83};
    int i;
    
    for(i=0;i<MAX_SIZE;i++)
        {
         src1[i]=i+3;
         src2[i]=i+2;
         src3[i]=i;
         dest[i]=0;
        }
    oil_recon8x8_inter2(dest,8,src1,2,src2,1,src3);
    
    for(i=0;i<MAX_SIZE;i++)
        if(check[i] == dest[i])
            {
            std_log(LOG_FILENAME_LINE,"oil_recon8x8_inter2 successful, dest[%d] = %d",i,dest[i]);
            }
        else
            {
            assert_failed=1;
            std_log(LOG_FILENAME_LINE,"oil_recon8x8_intra unsuccessful, Expected =%d,Obtained =%d",check[i],dest[i]);
            }
    }


int main()
    {
    std_log(LOG_FILENAME_LINE,"Test started testsuite_recon8x8");
    oil_init ();
    
    std_log(LOG_FILENAME_LINE,"oil_recon8x8_intra");
    test_recon8x8_intra();
    
    std_log(LOG_FILENAME_LINE,"oil_recon8x8_inter");
    test_recon8x8_inter();
    
    std_log(LOG_FILENAME_LINE,"oil_recon8x8_inter2");
    test_recon8x8_inter2();
    
    if(assert_failed)
           std_log(LOG_FILENAME_LINE,"Test Fail");
    else
           std_log(LOG_FILENAME_LINE,"Test Successful");
    create_xml(0);
    return 0;
    }
