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

 cout<<"Enter the following value  11.5"<<endl;
 cin>>x;
 cout<<"Expected output is 11.5"<<endl;
 cout<<"Observed output is  "<<x<<endl;

 cout<<"Enter the following value  10001.59999"<<endl;
 cin>>x;
 cout<<"Expected output is 10001.59961"<<endl;
 cout<<"Observed output is  "<<x<<endl<<endl;


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