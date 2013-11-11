/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This program uses the Bouncy Castle APIs PKCS#12 KDF to generate encryption keys + ivs 
* and mac keys for use with compatibility testing.
*
*/


package com.symbian.security;

import java.math.BigInteger;
import java.security.SecureRandom;
import org.bouncycastle.crypto.PBEParametersGenerator;
import org.bouncycastle.crypto.digests.SHA1Digest;
import org.bouncycastle.crypto.generators.PKCS12ParametersGenerator;
import org.bouncycastle.crypto.CipherParameters;
import org.bouncycastle.crypto.params.KeyParameter;
import org.bouncycastle.crypto.params.ParametersWithIV;

public class Pkcs12Pbe {
	private PKCS12ParametersGenerator pgen;

	public Pkcs12Pbe() {
		pgen = new PKCS12ParametersGenerator(new SHA1Digest());
	}

	public static void main(String args[]) {
		try {
			if (args.length < 5) {
				usage();
				System.exit(-1);
				
			}
			int keyLength = Integer.parseInt(args[0]);
			int blockSize = Integer.parseInt(args[1]);
			int iterations = Integer.parseInt(args[2]);
			String salt = args[3];
			String password = args[4];		
			byte[] saltBytes = hexToByteArray(salt);
	
			Pkcs12Pbe pbe = new Pkcs12Pbe();
			pbe.getKey(keyLength, blockSize, iterations, password, saltBytes);			
		}
		catch (Exception e) {
			System.exit(-1);
		}
	}
	
	private static byte[] hexToByteArray(String hex) throws Exception {
		if (hex.length() % 2 != 0) {
			throw new Exception("hexToByteArray: odd number of nibbles");
		}
		StringBuffer hexBuffer = new StringBuffer(hex);
		
		byte[] byteBuffer = new byte[hexBuffer.length() / 2];
		for (int i = 0; i < hexBuffer.length(); i+=2) {
			try {
				byteBuffer[i / 2] = (byte) Integer.parseInt(hexBuffer.substring(i, i+2), 16);
			}
			catch (NumberFormatException e) {
				System.err.println("hexToByteArray: invalid hex string: " + hex);
				throw e;
			}
		}
		return byteBuffer;
	}

	private static void usage() {
		System.err
				.println("Usage: pkcs12pbe <key length> <block_size> <iterations> <salt> <password>\n");
	}
	
	private void getKey(int keyLen, int ivLen, int iterCount, String password,
			byte[] salt) {
		System.out.print("key len = " + keyLen + ", iter count = "
				+ iterCount + ", password = \"" + password + "\", salt = ");		
		printUnformattedByteArray(salt);

		char[] pwChars = password.toCharArray();
		byte[] pwBytes = PBEParametersGenerator.PKCS12PasswordToBytes(pwChars);

		pgen.init(pwBytes, salt, iterCount);
		CipherParameters cp = pgen.generateDerivedParameters(keyLen, ivLen);

		ParametersWithIV ivp = (ParametersWithIV) cp;
		KeyParameter kp = (KeyParameter) ivp.getParameters();

		System.out.print("key ");
		printUnformattedByteArray((kp.getKey()));
		System.out.print("iv ");
		printUnformattedByteArray(ivp.getIV());

		kp = (KeyParameter) pgen.generateDerivedMacParameters(160);
		System.out.print("160bit hmac key ");
		printUnformattedByteArray((kp.getKey()));

	}

	// unformatted hex strings that can be passed as arguments to openssl
	private void printUnformattedByteArray(byte[] a) {
		StringBuffer line = new StringBuffer();
		
		for (int i = 0; i < a.length; i++) {
			line.append(hexStr(a[i], 2));
		}
		System.out.println(line);
	}	

	private String hexStr(int val, int width) {
		StringBuffer result = new StringBuffer();
		while (--width >= 0) {
			int bitPos = 4 * width;
			int nybble = (val & (0xf << bitPos)) >> bitPos;
			result.append(Integer.toHexString(nybble));
		}
		return result.toString();
	}
}
