/*===========================================================================*/
/* comp.c: data compression stuff */
/*===========================================================================*/
/* LZW compression taken from Dr. Dobb's Journal, Oct. 89 */
/*===========================================================================*/


#include <stdio.h>

#define BITS 12						// 	Setting the number of bits to 12 , 13
#define HASHING_SHIFT BITS-8		// or 14 affects several constants.
#define MAX_VALUE (1 << BITS)-1	    // Note that MS-DOS machines need to
#define MAX_CODE MAX_VALUE-1		// compile their code in large model
									// 14 bits is selected
									
#if BITS == 14
	#define TABLE_SIZE 10841		// The string size table needs to be a
#endif								// prime number that is somewhat larger
#if BITS == 13						// that 2**BITS.
	#define TABLE_SIZE 9029
#endif
#if BITS == 12			
	#define TABLE_SIZE 5021
#endif


void *malloc();

int *code_value;					// this is the code value array
unsigned int *prefix_code;			// this array holds the prefix codes
unsigned char *append_character;	// this array holds the appended chars
unsigned char decode_stack[4000];	// this array holds the decode string


/*===========================================================================*/
/* this program gets a file name from the command line. It compresses the 	*/
/* file, placing its output in a file named test.lzw. It then expands       */
/* test.lzw into test.out. Test.out should then be an exact duplicate of    */
/* the input file                                                           */
/*===========================================================================*/

main(int argc, char *argv[])
{
FILE *output_file;
FILE *lzw_file;
char input_file_name[81];
char output_file_name[81];
int fileLen,compType;


/*** The two buffers are needed for the decompression phase. ***/
	prefix_code = malloc(TABLE_SIZE*sizeof(unsigned int));
	append_character = malloc(TABLE_SIZE*sizeof(unsigned char));
	if (prefix_code == NULL || append_character == NULL)
	 {
	 	printf("Fatal error allocating table space!\n");
	 	exit();
	 }

/* Get the File name, open it ip, and open up the lzw output file */

	if(argc>1)
		strcpy(input_file_name,argv[1]);
	else
	 {
	 	printf("Input file name? ");
	 	scanf("%s",input_file_name);
	 }	
	if(argc>2)
		strcpy(output_file_name,argv[2]);
	else
	 {
	 	printf("Output file name? ");
	 	scanf("%s",output_file_name);
	 }	

/* Now open the files for expansion */

	lzw_file = fopen(input_file_name,"rb");
	output_file = fopen(output_file_name,"wb");
	if(lzw_file == NULL || output_file == NULL)
	 {
	 	printf("Fatal error opening files\n");
	 	exit();
	 }

/* expand the file */

	compType = getc(lzw_file);
	if(compType = 1)
	 {
		fileLen = (getc(lzw_file)<<8) & 0xff00;
		fileLen += getc(lzw_file) & 0xff;

		expand(lzw_file,output_file);
	 }
	else
	 {
	 	printf("unknown compression type\n");
	 }
	fclose(lzw_file);
	fclose(output_file);
	free(prefix_code);
	free(append_character);
}

/*===========================================================================*/
//	This is the hashing routine. It tries to find a match for the prefix+char
//	string in the string table. If it finds it, the index is returned. If
// the string is not found, the first available index in the string table is
// returned instead
/*===========================================================================*/

find_match(int hash_prefix,unsigned int hash_character)
{
int index;
int offset;

	index = (hash_character << 	HASHING_SHIFT) ^ hash_prefix;
	if(index == 0)
		offset = 1;
	else
		offset = TABLE_SIZE - index;
		
	while(1)
	 {
	 	if(code_value[index] == -1)
	 		return(index);
	 	if(prefix_code[index] == hash_prefix && append_character[index] == hash_character)
	 		return(index);
	 	index -= offset;
	 	if(index < 0)
	 		index += TABLE_SIZE;
	 }
}

/*===========================================================================*/
// This is the expansion routine. It takes an LZW format file, and expands
// it to an output file. The code here should be a fairly close match to
// the algorithm in the accompaning article
/*===========================================================================*/

expand(FILE *input,FILE *output)
{
unsigned int next_code;
unsigned int new_code;
unsigned int old_code;
int character;
int counter;
unsigned char *string;
char *decode_string(unsigned char *buffer,unsigned int code);

	next_code = 256;					// this is the next available code to define
	counter = 0;						// Counter is used as a pacifier
	printf("Expanding...\n");
	
	old_code = input_code(input);		// read in the first code, initialize the
	character = old_code;				// character variable, and send the first
	putc(old_code,output);				// code to the output file

	
/* this is the main expansion loop. It Reads in characters from the LZW file */
/* until it sees the special code used to indicate the end of the data */

	while((new_code = input_code(input)) != (MAX_VALUE))
	 {
	 	if(++counter ==1000)				// this section of code prints out
	 	 {									// an asterisk every 1000 characters
	 	 	counter = 0;					// it is just a pacifier.
	 	 	printf("*");
	 	 }

/* this code checks for the special STRING+CHARACTER+STRING+CHARACTER+STRING */
/* case which generates an undefined code. It handles it by decodeing */
/* the last code, adding a single character to the end of the decode string */

	if(new_code >= next_code)
	 {
	 	*decode_stack = character;
	 	string=decode_string(decode_stack+1,old_code);
	 }
	
/* otherwise do a straight decode of the new code */

	else
		string = decode_string(decode_stack,new_code);
		
/* now we output the decode string in reverse order */
	
	character = *string;
	while(string >= decode_stack)
		putc(*string--,output);
		
/* finally, if possible, add a new code to the string table */

	if(next_code <= MAX_CODE)
	 {
	 	prefix_code[next_code]=old_code;
	 	append_character[next_code] = character;
	 	next_code++;
	 }
	old_code = new_code;
	 }
	printf("\n");
}

/*===========================================================================*/
// this routine simply decodes a string from the string table, storing
// it in a buffer. The buffer can then be output in reverse order by
// the expansion program
/*===========================================================================*/

char *decode_string(unsigned char *buffer, unsigned int code)
{
int i;

	i = 0;
	while(code > 255)
	 {
	 	*buffer++ = append_character[code];
	 	code = prefix_code[code];
	 	if (i++ >= 4000)
	 	 {
	 	 	printf("Fatal error during code expansion\n");
	 	 	exit();
	 	 }
	 }
	*buffer=code;
	return(buffer);
}

/*===========================================================================*/
// The following two routines are used to output variable length
// codes. They are written strictly for clarity, and are not
// particularly efficient.
/*===========================================================================*/

input_code(FILE *input)
{
unsigned int return_value;
static int input_bit_count = 0;
static unsigned long input_bit_buffer = 0L;
	
	while(input_bit_count <= 24)
	 {
	 	input_bit_buffer |= (unsigned long) getc(input) << (24-input_bit_count);
		input_bit_count += 8;
	 }
	return_value=input_bit_buffer >> (32-BITS);
	input_bit_buffer <<= BITS;
	input_bit_count -= BITS;
	return(return_value);
}


