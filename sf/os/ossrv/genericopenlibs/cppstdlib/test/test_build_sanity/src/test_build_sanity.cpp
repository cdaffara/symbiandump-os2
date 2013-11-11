// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : test_build_sanity.cpp
// Part of     : standard c++ tests.
// RVCT 2.2 exports VTT for some of the classes, which are not exported in 
// in RVCT 3.1. To support RVCT 3.1, these exports have been made ABSENT
// in def file. 
// These test cases are added to make sure that marking VTT for some
// of the classes ABSENT in def file, should not affect any classes 
// deriving from them.
// 
//

#include<iostream>
#include<fstream>
using namespace std;
class Parent1: virtual public basic_ofstream<char, std::char_traits<char> >
{

public:
	Parent1()
	{
	cout<<"Inside Parent1::Parent1()"<<endl;
	}

};

class Parent2: virtual public basic_ofstream<char, std::char_traits<char> >
{

public:
	Parent2()
	{
	cout<<"Inside Parent2::Parent2()"<<endl;
	}

};

//Diamond shaped inheritance. To test that there is no linking error.
class DiamondChild: public Parent1, public Parent2
	{
	int c;
public:
	DiamondChild(int x):c(x)
	{
	cout<<"DiamondChild::DiamondChild()"<<endl;	
	}
	};

int main()
	{
	cout<<"Inside main"<<endl;
	DiamondChild obj(3);
	char a;
	cin>>a;
	return 0;
	}


