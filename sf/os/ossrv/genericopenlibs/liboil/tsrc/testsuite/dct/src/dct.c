/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <liboil/liboil.h>
#include <liboil/liboildebug.h>
#include <liboil/liboilfunction.h>
#include <stdio.h>
#include <stdlib.h>

#include <liboil/globals.h>

#define LOG_FILE "c:\\logs\\testsuite_dct_log.txt"
#include "std_log_result.h"
#include "utils.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("testsuite_dct");
    close_log_file();
}

void test_oil_mdct12_f64()
    {
    //double * d_6, const double * s_12
    double output[6];
    double input[12];
    double linux_output[] = {-55.78726058370923368557,-9.77999142525906961509,8.30695505006328360764,5.45142094924720943538,-5.13339728852725407648,-4.58890662311381625216};
    int i = 0;
    
    for(i=0; i<12; i++)
        {
        input[i] = i+1;
        
        if(i<6)
            output[i] = 0;
        }
    
    oil_mdct12_f64(output, input);
    
    for(i=0; i<6; i++)
        {
        if(comparefloats(output[i], linux_output[i]))
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %15.14f, actual value - %15.14f", i,linux_output[i],output[i]);
            assert_failed = 1; 
            }
        }
    }

void test_oil_imdct12_f64()
    {
    //double * d_12, const double * s_6
    double output[12];
    double input[6];
    double linux_output[] = {-4.33833397075162174872,3.40568926865829446626,-3.31124125233533384005,3.31124125233533384005,-3.40568926865828158768,4.33833397075162441325,-4.80351335916817223648,10.06982028664590167466,-10.35088397845068541869,-10.35088397845079377646,10.06982028664594963629,-4.80351335916817401284};
    int i = 0;
    
    for(i=0; i<12; i++)
        {
        output[i] = 0;
        
        if(i<6)
            input[i] = i + 1;
        }
    
    oil_imdct12_f64(output, input);
    
    for(i=0; i<12; i++)
        {
        if(comparefloats(output[i], linux_output[i]))
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %15.14f, actual value - %15.14f", i,linux_output[i],output[i]);
            assert_failed = 1; 
            }
        }
    }

void test_oil_mdct36_f64()
    {
    //double * d_18, const double * s_36
    double output[18];
    double input[36];
    double linux_output[] = {-485.54509125873983066413,-79.64662612510757355722,67.80760448776190685294,39.77358393387979162981,-36.41398520030525531865,-26.85943249683056777144,25.37958877703877291765,20.73179262231989383736,-19.93414220762220523397,-17.30353399124829749667,16.83079383688368935168,15.24349952210559955290,-14.95531193252001500582,-14.00077432972704016834,13.83319274912650698184,13.31880337261293334450,-13.24130282007020475987,-13.07848679902590305346};
    int i = 0;
    
    for(i=0; i<36; i++)
        {
        input[i] = i+1;
        
        if(i<18)
            output[i] = 0;
        }
    
    oil_mdct36_f64(output, input);
    
    for(i=0; i<18; i++)
        {
        if(comparefloats(output[i], linux_output[i]))
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %15.14f, actual value - %15.14f", i,linux_output[i],output[i]);
            assert_failed = 1; 
            }
        }
    }

void test_oil_imdct36_f64()
    {
    //double * d_36, const double * s_18
    double output[36];
    double input[18];
    double linux_output[] = {-12.85687531711769970855,11.41757120437667261115,-11.15646549643737905910,10.28156353478235018883,-10.12421323499659031597,9.61624441673711061185,-9.53135483275377914936,9.29662075491665795823,-9.26973797227096696361,9.26973797227097051632,-9.29662075491650519155,9.53135483275377559664,-9.61624441673730778746,10.12421323499673242452,-10.28156353478229512177,11.15646549643748208780,-11.41757120437673655999,12.85687531711769793219,-13.28789667691522780046,15.72998203713914833202,-16.48536211104631377111,21.07260369137263467110,-22.59425383526346919894,33.39775527158744949929,-37.52696988457606863676,85.41533571580391992484,-80.79110748109037842823,-80.79110748109157214003,85.41533571580409045509,-37.52696988457621074531,33.39775527158775503267,-22.59425383526349762064,21.07260369137274835794,-16.48536211104587678733,15.72998203713892451105,-13.28789667691528109117};
    int i = 0;
    
    for(i=0; i<36; i++)
        {
        output[i] = 0;
        
        if(i<18)
            input[i] = i+1;
        }
    
    oil_imdct36_f64(output, input);
    
    for(i=0; i<36; i++)
        {
        if(comparefloats(output[i], linux_output[i]))
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %15.14f, actual value - %15.14f", i,linux_output[i],output[i]);
            assert_failed = 1; 
            }
        }
    }

void test_oil_dct36_f32()
    {
    //float * d_36, int dstr, const float * s_36, int sstr
    float output[36];
    float input[36];
    float linux_output[] = {2.000000,3.000000,4.000000,5.000000,6.000000,7.000000,8.000000,9.000000,10.000000,11.000000,12.000000,13.000000,14.000000,15.000000,16.000000,17.000000,18.000000,19.000000,20.000000,21.000000,22.000000,23.000000,24.000000,25.000000,26.000000,27.000000,28.000000,29.000000,30.000000,31.000000,32.000000,33.000000,34.000000,35.000000,36.000000,37.000000};
    int i = 0;
    
    for(i=0; i<36; i++)
        {
        output[i] = i+2;
        input[i] = i+1;
        }
    
    oil_dct36_f32(output, 1, input, 2);
    
    for(i=0; i<36; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %f, actual value - %f", i,linux_output[i],output[i]);
            assert_failed = 1; 
            }
        }
    }

void test_oil_fdct8x8s_s16()
    {
    //int16_t * d_8x8, int ds, const int16_t * s_8x8, int ss
    int16_t output[64];
    int16_t input[64];
    int16_t linux_output[] = {13824,-4608,-1024,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i = 0;
    
    for(i=0; i<64; i++)
        {
        output[i] = 0;
        input[i] = i+1;
        }
    
    oil_fdct8x8s_s16(output, 1, input, 2);
    
    for(i=0; i<64; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1; 
            }
        }
    }

void test_oil_fdct8x8theora()
    {
    //const int16_t * s_8x8, int16_t * d_8x8
    int16_t output[64];
    int16_t input[64];
    int16_t linux_output[] = {1036,-62,0,-5,0,-5,0,-5,-582,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-61,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,-20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-6,0,0,0,0,0,0,0};
    int i = 0;
    
    for(i=0; i<64; i++)
        {
        output[i] = 0;
        input[i] = i+1;
        }
    
    oil_fdct8x8theora(input, output);
    
    for(i=0; i<64; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1; 
            }
        }
    }

void test_oil_idct8x8lim10_s16()
    {
    //int16_t * d_8x8, int dstr, const int16_t * s_8x8, int sstr
    int16_t output[64];
    int16_t input[64];
    int16_t linux_output[] = {1292,-771,768,-511,-1,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i = 0;
    
    for(i=0; i<64; i++)
        {
        output[i] = 0;
        input[i] = i+1;
        }
    
    oil_idct8x8lim10_s16(output, 1, input, 2);
    
    for(i=0; i<64; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1; 
            }
        }
    }

void test_oil_idct8theora_s16()
    {
    //int16_t * d_8, int dstr, const int16_t * s_8, int sstr
    int16_t output[8];
    int16_t input[8];
    int16_t linux_output[] = {16,-18,6,-5,5,-2,2,0};
    int i = 0;
    
    for(i=0; i<8; i++)
        {
        output[i] = 0;
        input[i] = i+1;
        }
    
    oil_idct8theora_s16(output, 1, input, 2);
    
    for(i=0; i<8; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1; 
            }
        }
    }

void test_oil_idct8x8theora_s16()
    {
    //int16_t * d_8x8, int dstr, const int16_t * s_8x8, int sstr
    int16_t output[64];
    int16_t input[64];
    int16_t linux_output[] = {157,-123,50,-36,26,-14,10,1,-110,51,-30,12,-16,2,-10,-2,52,-31,10,-10,10,0,6,0,-34,19,-11,6,-7,1,-3,-2,26,-15,7,-4,5,-1,3,0,-14,5,-2,2,0,-2,0,-2,12,-9,2,0,6,0,4,0,-1,-1,2,-2,4,2,-2,1};
    int i = 0;
    
    for(i=0; i<64; i++)
        {
        output[i] = 0;
        input[i] = i+1;
        }
    
    oil_idct8x8theora_s16(output, 1, input, 2);
    
    for(i=0; i<64; i++)
        {
        if(output[i] != linux_output[i])
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %d, actual value - %d", i,linux_output[i],output[i]);
            assert_failed = 1; 
            }
        }
    }

void test_oil_imdct32_f32()
    {
    //float * d_32, const float * s_32
    float output[32];
    float input[32];
    float expected_output[] = {528.00000000000000000000,-207.42237854003906250000,-0.00000000000002593932,-22.97223091125488281250,-0.00000000000002312733,-8.21512222290039062500,0.00000000000000710542,-4.14796209335327148438,-0.00000000000003045654,-2.47257304191589355469,0.00000000000000630225,-1.62263202667236328125,0.00000000000008801467,-1.13173198699951171875,-0.00000000000003877801,-0.82146918773651123047,0.00000000000035616302,-0.61161065101623535156,0.00000000000014964419,-0.46168053150177001953,0.00000000000006150462,-0.34939777851104736328,-0.00000000000018143386,-0.26159864664077758789,0.00000000000003763656,-0.19001007080078125000,-0.00000000000012429814,-0.12911282479763031006,-0.00000000000002208823,-0.07497953623533248901,-0.00000000000003485602,-0.02459304779767990112};
    int i = 0;
    
    for(i=0; i<32; i++)
        {
        input[i] = i+1;
        output[i] = 0;
        }
    
    oil_imdct32_f32(output, input);
    
    for(i=0; i<32; i++)
        {
        if(comparefloats(output[i], expected_output[i]))
            {
            std_log(LOG_FILENAME_LINE, "output[%d]: expected value - %15.14f, actual value - %15.14f", i,expected_output[i],output[i]);
            assert_failed = 1; 
            }
        }
    }

int main (int argc, char *argv[])
{
    oil_init ();
 
	std_log(LOG_FILENAME_LINE,"START oil_mdct12_f64 TEST");
    test_oil_mdct12_f64(); //fails because of float precision diff
	std_log(LOG_FILENAME_LINE,"END TEST\n");

	std_log(LOG_FILENAME_LINE,"START oil_imdct12_f64 TEST");
    test_oil_imdct12_f64(); //fails because of float precision diff
	std_log(LOG_FILENAME_LINE,"END TEST\n");

	std_log(LOG_FILENAME_LINE,"START oil_mdct36_f64 TEST");
    test_oil_mdct36_f64(); //fails because of float precision diff
	std_log(LOG_FILENAME_LINE,"END TEST\n");

	std_log(LOG_FILENAME_LINE,"START oil_imdct36_f64 TEST");
    test_oil_imdct36_f64(); //fails because of float precision diff
	std_log(LOG_FILENAME_LINE,"END TEST\n");
    
	std_log(LOG_FILENAME_LINE,"START oil_dct36_f32 TEST");
    test_oil_dct36_f32();
	std_log(LOG_FILENAME_LINE,"END TEST\n");

	std_log(LOG_FILENAME_LINE,"START oil_fdct8x8s_s16 TEST");
    test_oil_fdct8x8s_s16();
	std_log(LOG_FILENAME_LINE,"END TEST\n");

	std_log(LOG_FILENAME_LINE,"START oil_fdct8x8theora TEST");
    test_oil_fdct8x8theora();
	std_log(LOG_FILENAME_LINE,"END TEST\n");

	std_log(LOG_FILENAME_LINE,"START oil_idct8x8lim10_s16 TEST");
    test_oil_idct8x8lim10_s16();
	std_log(LOG_FILENAME_LINE,"END TEST\n");

	std_log(LOG_FILENAME_LINE,"START oil_idct8theora_s16 TEST");
    test_oil_idct8theora_s16();
	std_log(LOG_FILENAME_LINE,"END TEST\n");

	std_log(LOG_FILENAME_LINE,"START oil_idct8x8theora_s16 TEST");
    test_oil_idct8x8theora_s16();
	std_log(LOG_FILENAME_LINE,"END TEST\n");

	std_log(LOG_FILENAME_LINE,"START oil_imdct32_f32 TEST");
    test_oil_imdct32_f32(); //getting different output than linux
    std_log(LOG_FILENAME_LINE,"END TEST\n");

    if(assert_failed)
      std_log(LOG_FILENAME_LINE,"Test Failed");
    else
      std_log(LOG_FILENAME_LINE,"Test Successful");
    
    create_xml(0);
    return 0;
}

