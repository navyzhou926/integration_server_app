#ifndef _MD5_HEADER
#define _MD5_HEADER



/* Constants for MD5Transform routine.
 */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define SS32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21
/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short  UINT2;

/* UINT4 defines a four byte word */
typedef unsigned int   UINT4;
/* F, G, H and I are basic MD5 functions.
 */

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))


/* ROTATE_LEFT rotates x left n bits.
 */

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))


/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
 */

#define FF(a, b, c, d, x, s, ac) {  (a) += F ((b), (c), (d)) + (x) + (UINT4)(ac);  (a) = ROTATE_LEFT ((a), (s));  (a) += (b);   }

#define GG(a, b, c, d, x, s, ac) {  (a) += G ((b), (c), (d)) + (x) + (UINT4)(ac);  (a) = ROTATE_LEFT ((a), (s));  (a) += (b);   }



#define HH(a, b, c, d, x, s, ac) {  (a) += H ((b), (c), (d)) + (x) + (UINT4)(ac);  (a) = ROTATE_LEFT ((a), (s)); (a) += (b);   }



#define II(a, b, c, d, x, s, ac) {  (a) += I ((b), (c), (d)) + (x) + (UINT4)(ac);  (a) = ROTATE_LEFT ((a), (s));  (a) += (b);   }


//********************************************* //
//Struct Name:	MD5_CTX
//Struct Function:	Main MD5 Struct
//Notes:			
//********************************************* //
typedef struct {
	unsigned int state[4];			//encypted message
	unsigned int  count[2];			//bits of plaintext
	unsigned char buffer[64];
}MD5_CTX;

//********************************************* //
//Function Name	:	MDString
//Function Use	:	Digests a string and prints the result.
//Notes:	
//********************************************* //
unsigned char*  MD5String (char *string,unsigned int length);

//********************************************* //
//Function Name	:	MD5Init
//Function Use	:	
//Notes:	
//********************************************* //
void MD5Init (MD5_CTX *context); 


//********************************************* //
//Function Name	:	MD5Update
//Function Use	:	MD5 block update operation. Continues an MD5 message-digest operation, processing another message block, and updating the  context.	
//Notes:	
//********************************************* //
void MD5Update (MD5_CTX *context, unsigned char *input,unsigned int inputLen);

//********************************************* //
//Function Name	:	MD5Final
//Function Use	:	MD5 finalization. Ends an MD5 message-digest operation, writing the
//					the message digest and zeroizing the context.
//Notes:	
//********************************************* //
void MD5Final (unsigned char digest[16], MD5_CTX *context);


//********************************************* //
//Function Name	:	MD5Final
//Function Use	:	MD5 basic transformation. Transforms state based on block.
//Notes:	
//********************************************* //
void MD5Transform (unsigned int state[4],unsigned char block[64]);

//********************************************* //
//Function Name	:	MD5_memcpy 
//Function Use	:	
//Notes:	Replace "for loop" with standard memcpy if possible. 
//********************************************* //
void MD5_memcpy (POINTER output,POINTER input,unsigned int len);


//********************************************* //
//Function Name	:	MD5_memcpy 
//Function Use	:	Encodes input (UINT4) into output (unsigned char). Assumes len is
//  a multiple of 4. 	
//Notes:	
//********************************************* //
void Encode (unsigned char *output,UINT4 *input,unsigned int len);

//********************************************* //
//Function Name	:	MD5_memcpy 
//Function Use	:	Decodes input (unsigned char) into output (UINT4). Assumes len is
//  a multiple of 4.	
//Notes:
//********************************************* //
void Decode (UINT4 *output,unsigned char *input,unsigned int len);
//********************************************* //
//Function Name	:	MD5_memcpy 
//Function Use	:	
//Notes:	Replace "for loop" with standard memset if possible.
//********************************************* //
void MD5_memset (POINTER output,int value,unsigned int len);

void  MD5String2 (char *string,char* md5_string,unsigned int length);

//2¥Œº”√‹
void  MD5String22 (char *string,char* md5_string,unsigned int length);


#endif

