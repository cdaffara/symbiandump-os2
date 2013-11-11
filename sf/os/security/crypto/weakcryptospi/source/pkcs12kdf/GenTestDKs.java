/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This program generates keys with Bouncy Castle for compatibility testing.
*
*/


import java.security.SecureRandom; 
import org.bouncycastle.crypto.PBEParametersGenerator;
import org.bouncycastle.crypto.digests.SHA1Digest;
import org.bouncycastle.crypto.generators.PKCS12ParametersGenerator;
import org.bouncycastle.crypto.params.KeyParameter;

public class GenTestDKs
{
	public static void main(String[] args)
	{
		PKCS12ParametersGenerator pgen = new PKCS12ParametersGenerator(new SHA1Digest());

		// SB.4: key lengths for defined OIDs
		// (168 for triple DES will first exercise chaining.)
		final int[] keyLens = {40, 128, 168, 368};
		
		// SB.4 iteration count is recommended to be 1024 or more
		final int[] iterCounts = {1, 2, 4, 8, 128, 1024, 1536, 2048};
		
		// SB.4 salt should be same length as hash function output
		// (=160 bits for SHA1.)
		byte[][] salts = new byte[3][];
		salts[0] = new byte[] {'S', 'A', 'L', 'T'};
		System.out.println("4 byte salt");
		printByteArray(salts[0]);
		
		// calls to nextBytes() are only executed once
/*		SecureRandom sr;
		try { sr = SecureRandom.getInstance("SHA1PRNG", "SUN"); }
		catch (Exception e)
		{
			System.err.println("UNABLE TO GET RANDOM SOURCE");
			return;
		}
*/		
//		salts[1] = new byte[160 / 8];
//		sr.nextBytes(salts[1]);
		salts[1] = new byte[]
		{
			(byte) 0x1d, (byte) 0x56, (byte) 0x50, (byte) 0x78,
			(byte) 0xc3, (byte) 0x50, (byte) 0x6f, (byte) 0x89,
			(byte) 0xbd, (byte) 0xa7, (byte) 0x3b, (byte) 0xb6,
			(byte) 0xe3, (byte) 0xe5, (byte) 0xb8, (byte) 0xa3,
			(byte) 0x68, (byte) 0x3d, (byte) 0xd3, (byte) 0x62
		};
		System.out.println("20 byte salt (same size as SHA1 output)");
		printByteArray(salts[1]);
		
//		salts[2] = new byte[200 / 8];
//		sr.nextBytes(salts[2]);
		salts[2] = new byte[]
        {
			(byte) 0xe2, (byte) 0x2c, (byte) 0x7b, (byte) 0x03,
			(byte) 0x16, (byte) 0x3a, (byte) 0xe5, (byte) 0x47,
			(byte) 0xf8, (byte) 0x23, (byte) 0x9d, (byte) 0xa4,
			(byte) 0x0d, (byte) 0x6f, (byte) 0x46, (byte) 0xd7,
			(byte) 0x9e, (byte) 0xa3, (byte) 0xc6, (byte) 0xff,
			(byte) 0xb3, (byte) 0xf0, (byte) 0x4e, (byte) 0xbe,
			(byte) 0x61
        };
		System.out.println("25 byte salt");
		printByteArray(salts[2]);
		
		final String passwds[] = {"0000", "0001", "PSWD", "password", "abcdefghijklmnopqrstuvwxyz"}; 
		
		for (int keyLenIdx = 0; keyLenIdx < keyLens.length; ++keyLenIdx)
		{
			for (int iterIdx = 0; iterIdx < iterCounts.length; ++iterIdx)
			{
				for (int saltIdx = 0; saltIdx < salts.length; ++saltIdx)
				{
					for (int pwdIdx = 0; pwdIdx < passwds.length; ++pwdIdx)
					{
						testKey(pgen, keyLens[keyLenIdx], iterCounts[iterIdx], passwds[pwdIdx], salts[saltIdx]);
					}	// for (int pwdIdx = 0; pwdIdx < passwds.length; ++pwdIdx)
				}	// for (int saltIdx = 0; saltIdx < salts.length; ++saltIdx)
			}	// for (int iterIdx = 0; iterIdx < iterCounts.length; ++iterIdx)
		}	// for (int keyLenIdx = 0; keyLenIdx < keyLens.length; ++keyLenIdx)
	}
	
	private static void testKey(
		PKCS12ParametersGenerator pgen,
		int keyLen, int iterCount, String password, byte[] salt)
	{
		System.out.println(
				"key len = " + keyLen + ", iter count = " + iterCount
			+	", password = \"" + password + "\", salt len = " + salt.length);

		char[] pwChars = password.toCharArray();
		byte[] pwBytes = PBEParametersGenerator.PKCS12PasswordToBytes(pwChars);
		
		pgen.init(pwBytes, salt, iterCount);
		KeyParameter kp = (KeyParameter) pgen.generateDerivedParameters(keyLen);
		printByteArray(kp.getKey());
	}
	
	private static void printByteArray(byte[] a)
	{
		final int BLOCK_SIZE = 16;
		int keyLen = a.length;
		int rowCount = keyLen / BLOCK_SIZE;
		if ((keyLen % BLOCK_SIZE) != 0)
			++rowCount;
		
		for (int row = 0; row < rowCount; ++row)
			{
			int start = row * BLOCK_SIZE;
			int end = Math.min(start + BLOCK_SIZE, keyLen);
			
			StringBuffer line = new StringBuffer("[" + hexStr(start, 4) + "]");
			
			for (int i = start; i < end; ++i)
				line.append(" " + hexStr(a[i], 2));
			System.out.println(line);
			}
		System.out.println();
	}
	
	private static String hexStr(int val, int width)
	{
		StringBuffer result = new StringBuffer();
		while (--width >= 0)
		{
			int bitPos = 4 * width;
			int nybble = (val & (0xf << bitPos)) >> bitPos;
			result.append(Integer.toHexString(nybble));
		}
		
		return result.toString();
	}
}
