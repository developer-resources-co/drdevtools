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
FILE *input_file;
FILE *lzw_file;
char input_file_name[81];
char output_file_name[81];
int fileLen;

/*** The Three buffers are needed for the compression phase. ***/
	code_value = malloc(TABLE_SIZE*sizeof(unsigned int));
	prefix_code = malloc(TABLE_SIZE*sizeof(unsigned int));
	append_character = malloc(TABLE_SIZE*sizeof(unsigned char));
	if (code_value == NULL || prefix_code == NULL || append_character == NULL)
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
	input_file = fopen(input_file_name,"rb");

	if(argc>2)
		strcpy(output_file_name,argv[2]);
	else
	 {
	 	printf("Output file name? ");
	 	scanf("%s",output_file_name);
	 }


	lzw_file = fopen(output_file_name,"wb");
	if (input_file == NULL || lzw_file == NULL)
	 {
	 	printf("Fatal error opening files\n");
	 	exit();
	 };
	
/* Compress the file */

	fseek(input_file,0,SEEK_END);
	fileLen = ftell(input_file);						// find out how long file is
	rewind(input_file);

	putc(1,lzw_file);						// compression type 1, lzw
	putc(fileLen>>8,lzw_file);
	putc(fileLen & 0xff,lzw_file);

	compress(input_file,lzw_file);
	fclose(input_file);
	fclose(lzw_file);
	free(code_value);
	free(prefix_code);
	free(append_character);
}

/*===========================================================================*/
/* this is the compression routine. The code should be a fairly close */
/* match to the algorithm accompanying the article                    */
/*===========================================================================*/

compress(FILE *input, FILE *output)
{
unsigned int next_code;
unsigned int character;
unsigned int string_code;
unsigned int index;
int i,alreadyPrinted = 0;

	next_code = 256;		/* next code is the next available string code */
	for(i=0;i<TABLE_SIZE; i++)				// Clear out the string table before starting
		code_value[i] = -1;
	i=0;
	printf("Compressing...\n");
	string_code = getc(input);		// Get the first code */
	
/* This is the main loop where it all happens. This loop runs until all of */
/* the input has been exhausted. Note that is stops adding codes to the */
/* table after all of the possible codes have been defined. */

	while((character=getc(input)) != (unsigned)EOF)
	 {
	 	if(++i==1000)					// print a * every 1000
	 	 {  							// input characters. This is
	 		i = 0;						// just a pacifier.	
			printf("*");
	 	 }
		index = find_match(string_code,character);		// see if the string is in
		if(code_value[index] != -1)						// the table. If it is,
			string_code = code_value[index];			// get the code value. If
		else											// the string is not in the
		 {												// table, try to add it.
		 	if(next_code <= MAX_CODE)
		 	 {
		 	 	code_value[index] = next_code++;
		 	 	prefix_code[index] = string_code;
		 	 	append_character[index] = character;
		 	 }
		 	else if(!alreadyPrinted)
		 	 {
		 		printf("Code Table Full\n");
		 		alreadyPrinted = 1;
		 	 }
		 	output_code(output,string_code);			// When a string is found
		 	string_code = character;					// that is not in the table
         }												// I output the last string
	 }													// after adding the new one
	
/* end of main loop */

	output_code(output,string_code);					// Output the last code
	output_code(output,MAX_VALUE);					// output the end of buffer code
	output_code(output,0);							// this code flushes the output buffer
	printf("\n");
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

output_code(FILE *output, unsigned int code)
{
static int output_bit_count = 0;
static unsigned long output_bit_buffer = 0L;
	output_bit_buffer |= (unsigned long) code << (32-BITS-output_bit_count);
	output_bit_count += BITS;
	while(output_bit_count >= 8)
	 {
	 	putc(output_bit_buffer >> 24, output);
	 	output_bit_buffer <<= 8;
	 	output_bit_count -= 8;
	 }
}

/*===========================================================================*/
