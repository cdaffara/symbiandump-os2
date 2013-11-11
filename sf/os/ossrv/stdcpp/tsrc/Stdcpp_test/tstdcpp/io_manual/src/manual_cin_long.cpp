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

int long_test()
 {
 long x;
 unsigned long y;
 char c;

cout<<"Test case for long and unsigned long"<<endl<<endl;

cout<<"Enter the following value  44400"<<endl;
cin>>x;
cout<<"Expected output is 44400"<<endl;
cout<<"Observed output is  "<<x<<endl;

cout<<"Enter the following value  2147483647 "<<endl;
cin>>x;
cout<<"Expected output is  2147483647"<<endl;
cout<<"Observed output is  "<<x<<endl;

cout<<"Enter the following value -2147483648 "<<endl;
cin>>x;
cout<<"Expected output is  -2147483648"<<endl;
cout<<"Observed output is  "<<x<<endl;


cout<<"Enter the following value 4294967295 "<<endl;
cin>>y;
cout<<"Expected output is  4294967295"<<endl;
cout<<"Observed output is  "<<y<<endl;

cout<<"Enter the following value 0 "<<endl;
cin>>y;
cout<<"Expected output is  0"<<endl;
cout<<"Observed output is  "<<y<<endl;

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

failures=long_test();
if(failures)
 cout<<"Fail"<<endl;
else
 cout<<"Pass"<<endl;
return 0;
}