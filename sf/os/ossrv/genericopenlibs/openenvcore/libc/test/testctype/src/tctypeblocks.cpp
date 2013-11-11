/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "tctype.h"

// -----------------------------------------------------------------------------
//Function Name :isalpha_atoz
//API Tested :isalpha
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for alphabets
// -----------------------------------------------------------------------------
    
TInt CTestCtype::isalpha_atoz(  )
    {
    INFO_PRINTF1(_L("In isalpha_atoz"));
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[]=
		{
    	{ 'y' , 1 },
    	{ 'Y' , 1 },
    	{ 'r' , 1 },
    	{ 'Q' , 1 },
    	{ 'q' , 1 },
		};
   TInt i = 0;
    int size = sizeof(arr)/sizeof(st);
    for( i=0; i<size; i++)
    	{
    	
	   	INFO_PRINTF2(_L("Input : %c"),arr[i].input);
   	    INFO_PRINTF2(_L("Expected : %d"),arr[i].expected);
   	   
		int ret = isalpha(arr[i].input);
 		
 	 		
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
    	else
 			{
			INFO_PRINTF2(_L("Result : %d"),ret);
			}               
        }
	return KErrNone;				
        
    }


// -----------------------------------------------------------------------------
//Function Name :isalpha_num
//API Tested :isalpha
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for numbers
// -----------------------------------------------------------------------------    
TInt CTestCtype::isalpha_num(  )
    {
    
    INFO_PRINTF1(_L("In isalpha_num"));
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ '1' , 0 },
    	{ '7' , 0 },
    	{ '5' , 0 },
    	{ '9' , 0 },
    	{ '0' , 0 },
    
		};
   TInt i = 0;
  
    for( i=0; i<5; i++)
    	{
    	
	   	INFO_PRINTF2(_L("Input : %c"),arr[i].input);
   	    INFO_PRINTF2(_L("Expected : %d"),arr[i].expected);
   	   
		int ret = isalpha(arr[i].input);
 		
 	 		
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
    	else
 			{
			INFO_PRINTF2(_L("Result : %d"),ret);
			}               
        }
	return KErrNone;				

        
    }

// -----------------------------------------------------------------------------
//Function Name :isalpha_punc
//API Tested :isalpha
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for punctuation characters
// -----------------------------------------------------------------------------
TInt CTestCtype::isalpha_punc( )
    {
    
    INFO_PRINTF1(_L("In isalpha_punc"));
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ '!' , 0 },
    	{ '%' , 0 },
    	{ '*' , 0 },
    	{ '#' , 0 },
    	{ '~' , 0 },
    
		};
   TInt i = 0;
  
    for( i=0; i<5; i++)
    	{
    
	   	INFO_PRINTF2(_L("Input : %c"),arr[i].input);
   	    INFO_PRINTF2(_L("Expected : %d"),arr[i].expected);
   	   
		int ret = isalpha(arr[i].input);
 		
 	 		
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
    	else
 			{
			INFO_PRINTF2(_L("Result : %d"),ret);
			}               
        }
	return KErrNone;				

    }


// -----------------------------------------------------------------------------
//Function Name :isalpha_escape
//API Tested :isalpha
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for escape sequences
// -----------------------------------------------------------------------------
TInt CTestCtype::isalpha_escape( )
    {
    
    INFO_PRINTF1(_L("In isalpha_escape"));
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ '\n' , 0 },
    	{ '\r' , 0 },
    	{ '\t' , 0 },
    	{ '\a' , 0 },
    	{ '\v' , 0 },
		};
   TInt i = 0;
  

    for( i=0; i<5; i++)
    	{
    		
  	   	INFO_PRINTF2(_L("Input : %c"),arr[i].input);
   	    INFO_PRINTF2(_L("Expected : %d"),arr[i].expected);
   	   
		int ret = isalpha(arr[i].input);
 		
 	 		
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
    	else
 			{
			INFO_PRINTF2(_L("Result : %d"),ret);
			}               
        }
	return KErrNone;				

    }


// -----------------------------------------------------------------------------
//Function Name :isdigit_digit
//API Tested :isdigit
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for numbers 0-9
// -----------------------------------------------------------------------------
TInt CTestCtype::isdigit_digit( )
    {
    
    INFO_PRINTF1(_L("In isdigit_digit"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ '8' , 1 },
    	{ '5' , 1 },
    	{ '9' , 1 },
    	{ '2' , 1 },
    	{ '0' , 1 },
		};
    TInt i ;

    		
    for( i=0; i<5; i++)
   		{
    	
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isdigit(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }


// -----------------------------------------------------------------------------
//Function Name :isdigit_punc
//API Tested :isdigit
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for punctuation characters
// -----------------------------------------------------------------------------    
TInt CTestCtype::isdigit_punc( )
    {
    
    INFO_PRINTF1(_L("In isdigit_punc"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ '*' , 0 },
    	{ ',' , 0 },
    	{ '/' , 0 },
    	{ '^' , 0 },
    	{ '{' , 0 },
		};
    TInt i ;

    		
    for( i=0; i<5; i++)
   		{
    	   
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isdigit(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
			}           
        }
    return KErrNone;				    
    }
    

// -----------------------------------------------------------------------------
//Function Name :isdigit_alpha
//API Tested :isdigit
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for alphabets
// -----------------------------------------------------------------------------
    
TInt CTestCtype::isdigit_alpha( )
    {
    
    INFO_PRINTF1(_L("In isdigit_alpha"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ 'd' , 0 },
    	{ 'u' , 0 },
    	{ 'n' , 0 },
    	{ 'e' , 0 },
    	{ 'o' , 0 },
		};
    TInt i ;

    		
    for( i=0; i<5; i++)
   		{
    	   
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isdigit(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
			}           
        }
    return KErrNone;				    
    }


// -----------------------------------------------------------------------------
//Function Name :isdigit_escape
//API Tested :isdigit
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for escape sequences
// -----------------------------------------------------------------------------
TInt CTestCtype::isdigit_escape( )
    {
    
    INFO_PRINTF1(_L("In isdigit_escape"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
		{ '\n' , 0 },
    	{ '\r' , 0 },
    	{ '\t' , 0 },
    	{ '\v' , 0 },
    	{ '\"' , 0 },
		};
    TInt i ;

    		
    for( i=0; i<5; i++)
   		{
    	
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isdigit(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
			}           
        }
    return KErrNone;				    
    }


// -----------------------------------------------------------------------------
//Function Name :isalnum_num
//API Tested :isalnum
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for numbers 0-9
// -----------------------------------------------------------------------------    
TInt CTestCtype::isalnum_num( )
    {
    
    INFO_PRINTF1(_L("In isalnum_num"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ '8' , 1 },
    	{ '5' , 1 },
    	{ '9' , 1 },
    	{ '2' , 1 },
    	{ '0' , 1 },
		};
    TInt i ;

    		
    for( i=0; i<5; i++)
   		{
    	INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isalnum(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
    

// -----------------------------------------------------------------------------
//Function Name :isalnum_alpha
//API Tested :isalnum
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for alphabets
// -----------------------------------------------------------------------------        
TInt CTestCtype::isalnum_alpha( )
    {
    
    INFO_PRINTF1(_L("In isalnum_alpha"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ 'r' , 1 },
    	{ 'z' , 1 },
    	{ 'a' , 1 },
    	{ 'q' , 1 },
    	{ 'm' , 1 },
		};
    TInt i ;

    		
    for( i=0; i<5; i++)
   		{
    	   
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isalnum(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :isalnum_punc
//API Tested :isalnum
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for punctuation characters
// -----------------------------------------------------------------------------    
    
TInt CTestCtype::isalnum_punc( )
    {
    
    
    INFO_PRINTF1(_L("In isalnum_alpha"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ '*' , 0 },
    	{ '&' , 0 },
    	{ '^' , 0 },
    	{ '?' , 0 },
    	{ '/' , 0 },
		};
    TInt i ;

    		
    for( i=0; i<5; i++)
   		{
        
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isalnum(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
    

// -----------------------------------------------------------------------------
//Function Name :isalnum_escape
//API Tested :isalnum
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for escape sequences
// -----------------------------------------------------------------------------        
TInt CTestCtype::isalnum_escape( )
    {
    
    INFO_PRINTF1(_L("In isalnum_alpha"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ '\\' , 0 },
    	{ '\t' , 0 },
    	{ '\f' , 0 },
    	{ '\?' , 0 },
    	{ '\n' , 0 },
		};
    TInt i ;

    		
    for( i=0; i<5; i++)
   		{
    
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isalnum(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
    

// -----------------------------------------------------------------------------
//Function Name :ispunct_punct
//API Tested :ispunct
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for punctuation characters
// -----------------------------------------------------------------------------    
    
TInt CTestCtype::ispunct_punct( )
    {
    
    INFO_PRINTF1(_L("In ispunct"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[30]=
		{
    	{ '!' , 1 },
    	{ '"' , 1 },
    	{ '#' , 1 },
    	{ '%' , 1 },
    	{ '\'' ,1 },
    	{ '(' , 1 },
    	{ ')' , 1 },
    	{ '*' , 1 },
    	{ '+' , 1 },
    	{ ',' , 1 },
    	{ '-' , 1 },
    	{ '.' , 1 },
    	{ '/' , 1 },
    	{ ':' , 1 },
    	{ ';' , 1 },
    	{ '<' , 1 },
    	{ '=' , 1 },
    	{ '>' , 1 },
    	{ '?' , 1 },
    	{ '@' , 1 },
    	{ '[' , 1 },
    	{ '\\' ,1 },
    	{ ']' , 1 },
    	{ '^' , 1 },
    	{ '_' , 1 },
    	{ '\'' ,1 },
    	{ '{' , 1 },
    	{ '|' , 1 },
    	{ '}' , 1 },
    	{ '~' , 1 },
		};
    
    TInt i ;
    for( i=0; i<30; i++)
   		{
        
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = ispunct(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :ispunct_alpha_num
//API Tested :ispunct
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for alphanumeric characters
// -----------------------------------------------------------------------------    


TInt CTestCtype::ispunct_alpha_num( )
    {
   
    INFO_PRINTF1(_L("In ispunct_alpha_num"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[12]=
		{
    	{ 'V' , 0 },
    	{ 'u' , 0 },
    	{ '8' , 0 },
    	{ '6' , 0 },
    	{ 'P' , 0 },
    	{ 'Q' , 0 },
    	{ 'q' , 0 },
    	{ 'A' , 0 },
    	{ '5' , 0 },
    	{ 'N' , 0 },
    	{ 'j' , 0 },
    	{ 'r' , 0 },
		};
    
    TInt i ;

    for( i=0; i<12; i++)
   		{
    
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = ispunct(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :ispunct_cntrl
//API Tested :ispunct
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for control characters
// -----------------------------------------------------------------------------    

TInt CTestCtype::ispunct_cntrl( )
    {
 
    INFO_PRINTF1(_L("In ispunct_cntrl"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ '\n' , 0 },
    	{ '\r' , 0 },
    	{ '\t' , 0 },
    	{ '\f' , 0 },
    	{ '\a' , 0 },
    
		};
    
    TInt i ;
    for( i=0; i<5; i++)
   		{
   
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = ispunct(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
//Function Name :isupper_upper
//API Tested :isupper
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for characters in upper case
// -----------------------------------------------------------------------------       
TInt CTestCtype::isupper_upper( )
    {
    
    INFO_PRINTF1(_L("In isupper"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ 'Q' , 1 },
    	{ 'E' , 1 },
    	{ 'G' , 1 },
    	{ 'T' , 1 },
    	{ 'B' , 1 },
		};
    TInt i ;
    for( i=0; i<5; i++)
   		{
    
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isupper(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :isupper_lower
//API Tested :isupper
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for characters in lower case
// -----------------------------------------------------------------------------       
TInt CTestCtype::isupper_lower( )
    {
    
    INFO_PRINTF1(_L("In isupper"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ 'q' , 0 },
    	{ 'g' , 0 },
    	{ 'i' , 0 },
    	{ 't' , 0 },
    	{ 'm' , 0 },
		};
    TInt i ;

    for( i=0; i<5; i++)
   		{
   
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isupper(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :isupper_punc
//API Tested :isupper
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for punctuation characters.
// -----------------------------------------------------------------------------       
TInt CTestCtype::isupper_punc( )
    {
    
    INFO_PRINTF1(_L("In isupper"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ '^' , 0 },
    	{ '%' , 0 },
    	{ '/' , 0 },
    	{ '|' , 0 },
    	{ '*' , 0 },
		};
    TInt i ;

    for( i=0; i<5; i++)
   		{
    	
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isupper(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }


// -----------------------------------------------------------------------------
//Function Name :isupper_escape
//API Tested :isupper
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for escape sequences
// -----------------------------------------------------------------------------       
TInt CTestCtype::isupper_escape( )
    {
    
 
    INFO_PRINTF1(_L("In isupper"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ '\n' , 0 },
    	{ '\?' , 0 },
    	{ '\a' , 0 },
    	{ '\r' , 0 },
    	{ '\t' , 0 },
		};
    TInt i ;

    for( i=0; i<5; i++)
   		{
    
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isupper(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }


// -----------------------------------------------------------------------------
//Function Name :islower_upper
//API Tested :islower
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for characters in upper case
// -----------------------------------------------------------------------------       
TInt CTestCtype::islower_upper( )
    {
    
   	INFO_PRINTF1(_L("In islower_upper"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ 'Q' , 0 },
    	{ 'E' , 0 },
    	{ 'G' , 0 },
    	{ 'T' , 0 },
    	{ 'B' , 0 },
		};
    TInt i ;
	TChar ch('^');
	TBool res = ch.IsPunctuation();
   	INFO_PRINTF2( _L("IsPunctuation returned %d"), res );
    for( i=0; i<5; i++)
   		{
    	
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = islower(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :islower_lower
//API Tested :islower
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for characters in lower case
// -----------------------------------------------------------------------------       
TInt CTestCtype::islower_lower( )
    {
    
    INFO_PRINTF1(_L("In islower_lower"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ 'q' , 1 },
    	{ 'g' , 1 },
    	{ 'i' , 1 },
    	{ 't' , 1 },
    	{ 'm' , 1 },
		};
    TInt i ;

    for( i=0; i<5; i++)
   		{
    	
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = islower(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }


// -----------------------------------------------------------------------------
//Function Name :islower_punc
//API Tested :islower
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for punctuation characters
// -----------------------------------------------------------------------------       
TInt CTestCtype::islower_punc( )
    {
    
    INFO_PRINTF1(_L("In islower_punc"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ '^' , 0 },
    	{ '%' , 0 },
    	{ '/' , 0 },
    	{ '|' , 0 },
    	{ '*' , 0 },
		};
    TInt i ;

    for( i=0; i<5; i++)
   		{
    
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = islower(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :islower_escape
//API Tested :islower
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for escape characters
// -----------------------------------------------------------------------------       

TInt CTestCtype::islower_escape( )
    {
    
    INFO_PRINTF1(_L("In islower_escape"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ '\n' , 0 },
    	{ '\?' , 0 },
    	{ '\v' , 0 },
    	{ '\r' , 0 },
    	{ '\t' , 0 },
		};
    TInt i ;

    for( i=0; i<5; i++)
   		{
       
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = islower(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }


// -----------------------------------------------------------------------------
//Function Name :isxdigit_xdigit
//API Tested :isxdigit
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for characters used in representing
//hex numbers
// -----------------------------------------------------------------------------       
TInt CTestCtype::isxdigit_xdigit( )
    {
    
    INFO_PRINTF1(_L("In isxdigit"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[8]=
		{
    	{ 'F' , 1 },
    	{ 'a' , 1 },
    	{ 'C' , 1 },
    	{ 'f' , 1 },
    	{ '9' , 1 },
    	{ '2' , 1 },
    	{ 'B' , 1 },
    	{ 'e' , 1 },
		};
    TInt i ;

    for( i=0; i<8; i++)
   		{
    	//TestModuleIf().Printf( i, _L("Tctype"), 
                              //_L("Param[%i]: %c"), i, arr[i].input);
    
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isxdigit(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :isxdigit_escape
//API Tested :isxdigit
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for escape characters
// -----------------------------------------------------------------------------   

TInt CTestCtype::isxdigit_escape( )
    {
    
    INFO_PRINTF1(_L("In isxdigit_escape"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[8]=
		{
    	{ '\n' , 0 },
    	{ '\t' , 0 },
    	{ '\?' , 0 },
    	{ '\"' , 0 },
    	{ '\'' , 0 },
    	{ '\r' , 0 },
    	{ '\v' , 0 },
    	{ '\\' , 0 },
		};
    TInt i ;

    for( i=0; i<8; i++)
   		{
    
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isxdigit(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :isxdigit_punc
//API Tested :isxdigit
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for punctuation characters
// -----------------------------------------------------------------------------   


TInt CTestCtype::isxdigit_punc( )
    {
    
    INFO_PRINTF1(_L("In isxdigit_punc"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[8]=
		{
    	{ '#' , 0 },
    	{ '^' , 0 },
    	{ '/' , 0 },
    	{ '|' , 0 },
    	{ '&' , 0 },
    	{ '~' , 0 },
    	{ '.' , 0 },
    	{ ',' , 0 },
		};
    TInt i ;

    for( i=0; i<8; i++)
   		{
    	
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isxdigit(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :isxdigit_other_alpha
//API Tested :isxdigit
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for characters other than ones 
// used for representing hex numbers
// -----------------------------------------------------------------------------   

TInt CTestCtype::isxdigit_other_alpha( )
    {
    
    INFO_PRINTF1(_L("In isxdigit_other_alpha"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[8]=
		{
    	{ 't' , 0 },
    	{ 'j' , 0 },
    	{ 'x' , 0 },
    	{ 'n' , 0 },
    	{ 'h' , 0 },
    	{ 'l' , 0 },
    	{ 'p' , 0 },
    	{ 'y' , 0 },
		};
    TInt i ;

    for( i=0; i<8; i++)
   		{
       
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isxdigit(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :toupper_lower
//API Tested :toupper
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for lower-case characters
// -----------------------------------------------------------------------------   


TInt CTestCtype::toupper_lower( )
    {
    
    INFO_PRINTF1(_L("In toupper"));
	
	struct st
		{
		char input;
		int  expected;
		};
		
	struct st arr[5]=
		{
    	{ 'q' , 'Q' },
    	{ 'g' , 'G' },
    	{ 'i' , 'I' },
    	{ 't' , 'T' },
    	{ 'm' , 'M' },
		};
    TInt i ;

    for( i=0; i<5; i++)
   		{
    	
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = toupper(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :toupper_upper
//API Tested :toupper
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for upper-case characters
// -----------------------------------------------------------------------------   


TInt CTestCtype::toupper_upper( )
    {
    
    INFO_PRINTF1(_L("In toupper_upper"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ 'Q' , 'Q' },
    	{ 'G' , 'G' },
    	{ 'I' , 'I' },
    	{ 'T' , 'T' },
    	{ 'M' , 'M' },
		};
    TInt i ;

    for( i=0; i<5; i++)
   		{
    	
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = toupper(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :toupper_punc
//API Tested :toupper
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for punctuation characters
// -----------------------------------------------------------------------------   


TInt CTestCtype::toupper_punc( )
    {
    
    INFO_PRINTF1(_L("In toupper_punc"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ '/' , '/' },
    	{ '?' , '?' },
    	{ '&' , '&' },
    	{ '*' , '*' },
    	{ '>' , '>' },
		};
    TInt i ;

    for( i=0; i<5; i++)
   		{
    	   
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = toupper(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :toupper_escape
//API Tested :toupper
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for escape characters
// -----------------------------------------------------------------------------   

TInt CTestCtype::toupper_escape( )
    {
    
    INFO_PRINTF1(_L("In toupper_escape"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ '\\' , '\\' },
    	{ '\t' , '\t' },
    	{ '\?' , '\?' },
    	{ '\n' , '\n' },
    	{ '\"' , '\"' },
		};
    TInt i ;

    for( i=0; i<5; i++)
   		{
    	
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = toupper(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :tolower_upper
//API Tested :tolower
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for upper-case characters
// -----------------------------------------------------------------------------   

TInt CTestCtype::tolower_upper( )
    {
    
    INFO_PRINTF1(_L("In tolower_upper"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ 'Q','q' },
    	{ 'G','g' },
    	{ 'I','i' },
    	{ 'T','t' },
    	{ 'M','m' },
		};
    TInt i ;

    for( i=0; i<5; i++)
   		{
    	
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = tolower(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :tolower_lower
//API Tested :tolower
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for lower-case characters
// -----------------------------------------------------------------------------   

TInt CTestCtype::tolower_lower( )
    {
    
    INFO_PRINTF1(_L("In tolower"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ 'q' , 'q' },
    	{ 'f' , 'f' },
    	{ 'y' , 'y' },
    	{ 'l' , 'l' },
    	{ 'n' , 'n' },
		};
    TInt i ;

    for( i=0; i<5; i++)
   		{
    	
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = tolower(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :tolower_punc
//API Tested :tolower
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for punctuation characters
// -----------------------------------------------------------------------------   

TInt CTestCtype::tolower_punc( )
    {
    
    INFO_PRINTF1(_L("In tolower_punc"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ '/' , '/' },
    	{ '?' , '?' },
    	{ '&' , '&' },
    	{ '*' , '*' },
    	{ '>' , '>' },
		};
    TInt i ;

    for( i=0; i<5; i++)
   		{
    	
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = tolower(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :tolower_escape
//API Tested :tolower
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for escape characters
// -----------------------------------------------------------------------------   

TInt CTestCtype::tolower_escape( )
    {
    
    INFO_PRINTF1(_L("In tolower_escape"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[5]=
		{
    	{ '\\' , '\\' },
    	{ '\t' , '\t' },
    	{ '\?' , '\?' },
    	{ '\n' , '\n' },
    	{ '\"' , '\"' },
		};
    TInt i ;

    for( i=0; i<5; i++)
   		{
        
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = tolower(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :isspace_space
//API Tested :isspace
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for characters that introduce space
// -----------------------------------------------------------------------------   

TInt CTestCtype::isspace_space( )
    {
    
    INFO_PRINTF1(_L("In isspace"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[6]=
		{
    	{ '\n' , 1 },
    	{ '\t' , 1 },
    	{ '\r' , 1 },
    	{ '\v' , 1 },
    	{ '\f' , 1 },
    	{ ' '  , 1 },
		};
    TInt i ;

    for( i=0; i<6; i++)
   		{
    
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isspace(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :isspace_punc
//API Tested :isspace
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for punctuation characters
// -----------------------------------------------------------------------------   

TInt CTestCtype::isspace_punc( )
    {
    
    INFO_PRINTF1(_L("In isspace_punc"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[6]=
		{
    	{ '%' , 0 },
    	{ '+' , 0 },
    	{ '.' , 0 },
    	{ '@' , 0 },
    	{ '~' , 0 },
    	{ '^' , 0 },
		};
    TInt i ;

    for( i=0; i<6; i++)
   		{
    
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isspace(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :isspace_alpha
//API Tested :isspace
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for alphabets
// -----------------------------------------------------------------------------   

TInt CTestCtype::isspace_alpha( )
    {
    
    INFO_PRINTF1(_L("In isspace_alpha"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[6]=
		{
    	{ 'C' , 0 },
    	{ 'g' , 0 },
    	{ 'k' , 0 },
    	{ 'I' , 0 },
    	{ 'U' , 0 },
    	{ 'r' , 0 },
		};
    TInt i ;

    for( i=0; i<6; i++)
   		{
    
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isspace(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :isspace_num
//API Tested :isspace
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for numbers
// -----------------------------------------------------------------------------   

TInt CTestCtype::isspace_num( )
    {
    
    INFO_PRINTF1(_L("In isspace_num"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
	struct st arr[6]=
		{
    	{ '7' , 0 },
    	{ '4' , 0 },
    	{ '5' , 0 },
    	{ '1' , 0 },
    	{ '0' , 0 },
    	{ '9' , 0 },
		};
    TInt i ;

    for( i=0; i<6; i++)
   		{
   
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isspace(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :iscntrl_cntrl
//API Tested :iscntrl
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for control characters 
// -----------------------------------------------------------------------------   

TInt CTestCtype::iscntrl_cntrl( )
    {
    
    INFO_PRINTF1(_L("In iscntrl"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[7]=
		{
    	{ '\n' , 1 },
    	{ '\t' , 1 },
    	{ '\r' , 1 },
    	{ '\v' , 1 },
    	{ '\f' , 1 },
    	{ '\b' , 1 },
    	{ '\a' , 1 },
    	
		};
    TInt i ;

    for( i=0; i<7; i++)
   		{
    	    
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = iscntrl(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :iscntrl_other_escape
//API Tested :iscntrl
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for escape sequences other than 
// control characters 
// -----------------------------------------------------------------------------   

TInt CTestCtype::iscntrl_other_escape( )
    {
    
    INFO_PRINTF1(_L("In iscntrl_other_escape"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[4]=
		{
    	{ '\\' , 0 },
    	{ '\"' , 0 },
    	{ '\?' , 0 },
    	{ '\'' , 0 },
    	
		};
    TInt i ;

    for( i=0; i<4; i++)
   		{
    
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = iscntrl(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :iscntrl_alpha_num
//API Tested :iscntrl
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for alphanumeric characters
// -----------------------------------------------------------------------------   

TInt CTestCtype::iscntrl_alpha_num( )
    {
    
    INFO_PRINTF1(_L("In iscntrl_alpha_num"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[7]=
		{
    	{ 'A' , 0 },
    	{ 'j' , 0 },
    	{ 'o' , 0 },
    	{ 'U' , 0 },
    	{ '8' , 0 },
    	{ '0' , 0 },
    	{ '2' , 0 },
    	
		};
    TInt i ;

    for( i=0; i<7; i++)
   		{
      
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = iscntrl(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :isprint_cntrl
//API Tested :isprint
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for control characters 
// -----------------------------------------------------------------------------   

TInt CTestCtype::isprint_cntrl( )
    {
    
    INFO_PRINTF1(_L("In isprint_cntrl"));
	
	struct st
		{
		char input;
		int  expected;
		};
		
    struct st arr[7]=
		{
    	{ '\n' , 0 },
    	{ '\t' , 0 },
    	{ '\r' , 0 },
    	{ '\v' , 0 },
    	{ '\f' , 0 },
    	{ '\b' , 0 },
    	{ '\a' , 0 },
    	
		};
    TInt i ;

    for( i=0; i<7; i++)
   		{
        
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isprint(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :isprint_alpha_num
//API Tested :isprint
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for alphanumeric characters 
// -----------------------------------------------------------------------------   

TInt CTestCtype::isprint_alpha_num( )
    {
    
    INFO_PRINTF1(_L("In isprint_alpha_num"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[7]=
		{
    	{ 'n' , 1 },
    	{ 'T' , 1 },
    	{ 'O' , 1 },
    	{ '6' , 1 },
    	{ 'I' , 1 },
    	{ '9' , 1 },
    	{ 'x' , 1 },
    	
		};
    TInt i ;

    for( i=0; i<7; i++)
   		{
   
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isprint(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :isprint_xdigit
//API Tested :isprint
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for characters used for representing
// hexadecimal numbers
// -----------------------------------------------------------------------------   

TInt CTestCtype::isprint_xdigit( )
    {
    
    INFO_PRINTF1(_L("In isprint_xdigit"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[7]=
		{
    	{ '4' , 1 },
    	{ '9' , 1 },
    	{ 'D' , 1 },
    	{ '1' , 1 },
    	{ 'F' , 1 },
    	{ 'f' , 1 },
    	{ 'a' , 1 },
    	
		};
    TInt i ;

    for( i=0; i<7; i++)
   		{
    	    
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isprint(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :isprint_punc
//API Tested :isprint
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for punctuation characters 
// -----------------------------------------------------------------------------   

TInt CTestCtype::isprint_punc( )
    {
    
    INFO_PRINTF1(_L("In isprint_punc"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[7]=
		{
    	{ ';' , 1 },
    	{ ' ' , 1 },
    	{ '/' , 1 },
    	{ '&' , 1 },
    	{ '^' , 1 },
    	{ '#' , 1 },
    	{ '@' , 1 },
    	
		};
    TInt i ;

    for( i=0; i<7; i++)
   		{
 
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isprint(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :isgraph_escape
//API Tested :isgraph
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for escape sequences 
// -----------------------------------------------------------------------------   

TInt CTestCtype::isgraph_escape( )
    {
    
    INFO_PRINTF1(_L("In isgraph_cntrl"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[7]=
		{
    	{ '\n' , 0 },
    	{ '\t' , 0 },
    	{ '\r' , 0 },
    	{ '\v' , 0 },
    	{ '\f' , 0 },
    	{ '\b' , 0 },
    	{ '\a' , 0 },
    	
		};
    TInt i ;

    for( i=0; i<7; i++)
   		{
    
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isgraph(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :isgraph_alpha_num
//API Tested :isgraph
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for alphanumeric characters
// -----------------------------------------------------------------------------   

TInt CTestCtype::isgraph_alpha_num( )
    {
    
    INFO_PRINTF1(_L("In isgraph_alpha_num"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[7]=
		{
    	{ 'n' , 1 },
    	{ 'T' , 1 },
    	{ 'O' , 1 },
    	{ '6' , 1 },
    	{ 'I' , 1 },
    	{ '9' , 1 },
    	{ 'x' , 1 },
    	
		};
    TInt i ;

    for( i=0; i<7; i++)
   		{
  
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isgraph(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :isgraph_xdigit
//API Tested :isgraph
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for characters used for representing
// hexadecimal numbers
// -----------------------------------------------------------------------------   

TInt CTestCtype::isgraph_xdigit( )
    {
    
    INFO_PRINTF1(_L("In isgraph_xdigit"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[7]=
		{
    	{ '4' , 1 },
    	{ '9' , 1 },
    	{ 'D' , 1 },
    	{ '1' , 1 },
    	{ 'F' , 1 },
    	{ 'f' , 1 },
    	{ 'a' , 1 },
    	
		};
    TInt i ;

    for( i=0; i<7; i++)
   		{
    
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isgraph(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :isgraph_punc
//API Tested :isgraph
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for punctuation characters
// -----------------------------------------------------------------------------   

TInt CTestCtype::isgraph_punc( )
    {
    
    INFO_PRINTF1(_L("In isgraph_punc"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[7]=
		{
    	{ ';' , 1 },
    	{ '.' , 1 },
    	{ '/' , 1 },
    	{ '&' , 1 },
    	{ '^' , 1 },
    	{ '#' , 1 },
    	{ '@' , 1 },
    	
		};
    TInt i ;

    for( i=0; i<7; i++)
   		{

   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isgraph(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :isgraph_alpha_num
//API Tested :isgraph
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for alphanumeric characters
// -----------------------------------------------------------------------------   

TInt CTestCtype::isgraph_space( )
    {
    INFO_PRINTF1(_L("In isgraph_space"));
	char space = ' ';
	int expected = 0;
    INFO_PRINTF2(_L("Input %c"), space );
    INFO_PRINTF2(_L("Expected %d"), expected );
	TInt ret = isgraph(space);
	if( (!ret) != (!expected) )
  		{
  		ERR_PRINTF1(_L("Error"));
  		return KErrGeneral;		
  		}
  	else
		{
		INFO_PRINTF2(_L("Result : %d"),ret);
					
		}           
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :isascii_ascii
//API Tested :isascii
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for ascii characters
// -----------------------------------------------------------------------------   

TInt CTestCtype::isascii_ascii( )
    {
    
    INFO_PRINTF1(_L("In isascii"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[6]=
		{
    	{ 's' ,  1 },
    	{ '9' ,  1 },
    	{ '\r' , 1 },
    	{ '\\' , 1 },
    	{ '\"' , 1 },
    	{ ' '  , 1 },
		};
    TInt i ;

    for( i=0; i<6; i++)
   		{
   
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isascii(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//Function Name :isascii_bad_param
//API Tested :isascii
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for non-ascii characters-out of bounds
// -----------------------------------------------------------------------------   

TInt CTestCtype::isascii_bad_param( )
    {
    
    INFO_PRINTF1(_L("In isascii_bad_param"));
	
	struct st
		{
		int input;
		int  expected;
		};
	
    struct st arr[4]=
		{
    	{ 128  ,  0 },
    	{ 255  ,  0 },
    	{ 170  ,  0 },
    	{ 222  ,  0 },
		};
    TInt i ;

    for( i=0; i<4; i++)
   		{

   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 
 		TInt ret = isascii(arr[i].input);

  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :toascii_ascii
//API Tested :toascii
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for ascii characters
// -----------------------------------------------------------------------------   

TInt CTestCtype::toascii_ascii( )
    {

    INFO_PRINTF1(_L("In toascii"));
	
	struct st
		{
		char input;
		int  expected;
		};
	
    struct st arr[6]=
		{
    	{ 's' ,  's'  },
    	{ '9' ,  '9'  },
    	{ '\r' , 'r'  },
    	{ '\\' , '\\' },
    	{ '\"' , '\"' },
    	{ ' '  , ' '  },
		};
    TInt i ;

    for( i=0; i<6; i++)
   		{
  
   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = isascii(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
//Function Name :isascii_bad
//API Tested :isascii
//TD TestCase Number :1_19_2_
//TD TestCase Identifier :
//TestCase Description:testing the behaviour for non-ascii characters- out of bounds
// -----------------------------------------------------------------------------   

TInt CTestCtype::toascii_bad( )
    {
    
    INFO_PRINTF1(_L("In isascii"));
	
	struct st
		{
		int input;
		int  expected;
		};
	
    struct st arr[6]=
		{
    	{ 222 ,  94  },
    	{ 255 ,  127 },
    	{ 166 ,  38  },
    	{  55 ,  55  },
    	{ 199 ,  71  },
    	{ ' '  , 32  },
		};
    TInt i ;

    for( i=0; i<6; i++)
   		{

   		INFO_PRINTF2(_L("Input %c"), arr[i].input );
  	    INFO_PRINTF2(_L("Expected %d"), arr[i].expected );
 		TInt ret = toascii(arr[i].input);
  		if( (!ret) != (!arr[i].expected) )
	  		{
	  		ERR_PRINTF1(_L("Error"));
	  		return KErrGeneral;		
	  		}
	  	else
			{
			INFO_PRINTF2(_L("Result : %d"),ret);
						
			}           
        }
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CTestCtype::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//


// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
