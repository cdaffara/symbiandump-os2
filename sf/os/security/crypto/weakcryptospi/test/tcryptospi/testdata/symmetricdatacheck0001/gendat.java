/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


   import java.security.*;
   import javax.crypto.*;
   import javax.crypto.spec.*;
   import java.io.*;
   import java.io.*;
   import java.security.spec.*;
   import javax.net.ssl.SSLServerSocket;
   import javax.net.ssl.SSLServerSocketFactory;
   import javax.net.ssl.SSLSocket;
   
   /**
   * This program generates a AES key, retrieves its raw bytes, and
   * then reinstantiates a AES key from the key bytes.
   * The reinstantiated key is used to initialize a AES cipher for
   * encryption and decryption.
   */

   public class GENDAT {

     /**
     * Turns array of bytes into string
     *
     * @param buf	Array of bytes to convert to hex string
     * @return	Generated hex string
     */
     public static String asHex (byte buf[]) {
      StringBuffer strbuf = new StringBuffer(buf.length * 2);
      int i;

      for (i = 0; i < buf.length; i++) {
       if (((int) buf[i] & 0xff) < 0x10)
	    strbuf.append("0");

       strbuf.append(Long.toString((int) buf[i] & 0xff, 16));
      }

      return strbuf.toString();
     }

     public static void main(String[] args) throws Exception {
    String algo="";
    String oper="";
    String pad="";
    String composite="";
    
    String inContents="";
    String outContents="";
    String keyContents="";
    String ivContents="";
    
    String inPath = args[0];
    String outPath = args[1];
    String keyPath = args[2];
    String ivPath = "";
    
	algo = args[3];
	composite = args[4];

    if(args.length>=6)
    {
    	ivPath = args[5];
    	
        BufferedReader ivFile = new BufferedReader(new FileReader(ivPath));
        ivContents = ivFile.readLine();
        ivFile.close();
    }

	
    // read in the file contents
    BufferedReader inFile = new BufferedReader(new FileReader(inPath));
    inContents = inFile.readLine();
    inFile.close();
    
    BufferedReader keyFile = new BufferedReader(new FileReader(keyPath));
    keyContents = keyFile.readLine();
    keyFile.close();
    



    // Get the KeyGenerator
	//	KeyGenerator kgen = KeyGenerator.getInstance(algo);
	//	kgen.init(56); // 192 and 256 bits may not be available


       // Generate the secret key specs.
   //    SecretKey skey = kgen.generateKey();
     //  byte[] raw = skey.getEncoded();

		byte[] raw = keyContents.getBytes();
		
       SecretKeySpec skeySpec = new SecretKeySpec(raw, algo);

       Cipher cipher = Cipher.getInstance(composite);
       
       byte[] ivBytes = ivContents.getBytes();
       
       if(ivBytes.length > 0)
       {
    	   if(algo.equals("RC2"))
    	   {
    		   RC2ParameterSpec rc2Spec = new RC2ParameterSpec(64, ivBytes);
    	       cipher.init(Cipher.ENCRYPT_MODE, skeySpec, rc2Spec );
    	   }
    	   else
    	   {     
    		   AlgorithmParameterSpec paramSpec = new IvParameterSpec(ivBytes);
    		   cipher.init(Cipher.ENCRYPT_MODE, skeySpec, paramSpec );
    	   }
       }
       else
       {
           cipher.init(Cipher.ENCRYPT_MODE, skeySpec);
       }
       FileOutputStream outFile = new FileOutputStream(outPath);
       PrintStream p = new PrintStream( outFile );
       
       byte[] encrypted =
         cipher.doFinal(inContents.getBytes());
       System.out.print("encrypted string: " + asHex(encrypted));
       p.print(asHex(encrypted));
       
       p.close();
    

     }
   }
