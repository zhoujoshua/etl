/******************************************************************************
 *  MD5.H - header file for MD5.C                                             *
 *  Copyright (C) 2001-2002 by ShadowStar.                                    *
 *  Use and modify freely.                                                    *
 *  http://shadowstar.126.com/                                                *
 ******************************************************************************
 */
#ifndef _MD5_H
#define _MD5_H

#ifdef __alpha
typedef unsigned int uint32;
#else
typedef unsigned long uint32;
#endif

struct MD5Context {
	uint32 buf[4];
	uint32 bits[2];
	unsigned char in[64];
};


/*
* This is needed to make RSAREF happy on some MS-DOS compilers.
*/
typedef struct MD5Context MD5_CTX;

//---------------------------------------------------------------------------
class TMD5
{
public :
	void Init(MD5_CTX *context);
	void Update(MD5_CTX *context, const unsigned char *input, unsigned int inputLen);
	void Final(unsigned char digest[16], MD5_CTX *context);
	void MessageDigest(const unsigned char *szInput, unsigned int inputLen,
			unsigned char szOutput[16], int iIteration = 1);
private :
	MD5_CTX context;	
	void Transform(uint32 state[4], uint32 block[16]);
};

#endif

