/*===========================================================================*/
/* group.c: given list of input files, encode into one output file 			 */
/*===========================================================================*/

#include <stdio.h>

/*===========================================================================*/

main(int argc, char *argv[])
{
FILE *output_file;
FILE *input_file;
char input_file_name[81];
int fileLen,i;

	if(argc == 1)
	 {
	 	printf("format:\ngroup: <output file>");
	 	exit(1);
	 }

	output_file = fopen(argv[1],"wb");
	if (output_file == NULL)
	 {
	 	printf("Fatal error opening file %s\n",argv[1]);
	 	exit();
	 };


 	printf("Input file name? ");
 	scanf("%s",input_file_name);

	while(input_file_name[0] != '*')
	 {
		input_file = fopen(input_file_name,"rb");

		fseek(input_file,0,SEEK_END);
		fileLen = ftell(input_file);						// find out how long file is
		rewind(input_file);

		fputs(input_file_name,output_file);
		putc(0,output_file);
		putc(fileLen>>8,output_file);
		putc(fileLen & 0xff,output_file);

		for(i=0;i<fileLen;++i)
			putc(getc(input_file),output_file);

		fclose(input_file);
		
	 	printf("Input file name? ");
 		scanf("%s",input_file_name);
     }
	fclose(output_file);
}

/*===========================================================================*/
