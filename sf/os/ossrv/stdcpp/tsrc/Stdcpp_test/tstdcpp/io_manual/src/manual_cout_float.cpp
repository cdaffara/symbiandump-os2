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


#include<iostream>
using namespace std;

int float_test()
{
 float x;
 char c;
 cout<<"Test case for float "<<endl;
 x=11.5;
 cout<<"The following float value should be 11.5"<<endl;
 cout<<x<<endl;
 
 x=10001.59999;
 cout<<"The following float value should be  10001.59961"<<endl;
 cout<<x<<endl;
 
  cout<<"Press 1 to PASS or 0 to FAIL this sub-case:\r\n";
 do{
    c=getchar();
   }while(c!='1' && c!='0');

if('1'==c)
 return 0;
else
 return 1;
}

int main()
{
 int failures=0; 
 failures=float_test();

if(failures)
 cout<<"Fail"<<endl;
else
 cout<<"Pass"<<endl;

 return 0;
}
