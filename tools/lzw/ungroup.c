/*===========================================================================*/
/* ungroup.c: given grouped input file, decode into individual output files  */
/*===========================================================================*/

#include <stdio.h>

/*===========================================================================*/

main(int argc, char *argv[])
{
FILE *output_file;
FILE *input_file;
char output_file_name[81];
int fileLen,i;
unsigned char hi,lo;
unsigned int character;

	if(argc == 1)
	 {
	 	printf("format:\ngroup: <output file>");
	 	exit(1);
	 }

	input_file = fopen(argv[1],"rb");
	if (input_file == NULL)
	 {
	 	printf("Fatal error opening file %s\n",argv[1]);
	 	exit();
	 };

	while((character=getc(input_file)) != (unsigned)EOF)
	 {
	 	i = 0;
	 	while(output_file_name[i++] = character)
	 		character =  getc(input_file);
		output_file = fopen(output_file_name,"wb");

        hi = getc(input_file);
		lo = getc(input_file);
		fileLen = lo + ((int)hi<<8);

		for(i=0;i<fileLen;++i)
			putc(getc(input_file),output_file);

		fclose(output_file);
     }
	fclose(input_file);
}

/*===========================================================================*/
