/*===========================================================================*/
/* ihx2bin.c: convert intel hex to binary			 */
/*===========================================================================*/

#include <stdio.h>

/*===========================================================================*/

main(argc,argv)
int argc;
char *argv[];
{
FILE *output_file;
FILE *input_file;
char input_file_name[81];
char outFileName[81];
int i;
char c;

	if(argc < 2)
	 {
		printf("Intel Hex to bin converter\nformat:\nihx2bin: input file \nInput file must have extension .ihx, output file will have extension .bin\n");
		exit(1);
	 }

	sprintf(input_file_name,"%s.ihx",argv[1]);
	input_file = fopen(input_file_name,"r");
	if(input_file == NULL)
	 {
		printf("Fatal error opening file %s\n",input_file_name);
		exit();
	 };

	sprintf(outFileName,"%s.bin",argv[1]);
	output_file = fopen(outFileName,"wb");
	if (output_file == NULL)
	 {
		printf("Fatal error opening file %s\n",outFileName);
		exit();
	 };

	while(DoIntelLine(input_file,output_file));                   /* loop to end of file */

	fclose(input_file);
	fclose(output_file);
}

/*===========================================================================*/

DecodeHexDigit(input)
unsigned char input;
{
    char val;
    input -= '0';
    if(input > 9)
	input -= ( 'A' - ':');
    return(input);
}

/*-------------------------------------------------------------------------*/

int
ReadHexByte(input_file)
FILE *input_file;
{
    char high,val,c;
    if((c = getc(input_file)) == EOF)
	return(0x100);
    high = DecodeHexDigit(c);
    if((c = getc(input_file)) == EOF)
	return(0x100);
    val = DecodeHexDigit(c);       /* # of bytes on line */
    return(val+(high<<4));
}

/*=========================================================================*/

DoIntelLine(input_file,output_file)
FILE *input_file, *output_file;
{
    int count,i,c;
    if(c = getc(input_file) != ':')
     {
		if(getc(input_file) == EOF)
		    return(0);
		printf("invalid intel hex file!\n");
		return(0);
     }

    count = ReadHexByte(input_file);
    ReadHexByte(input_file);              /* skip address */
    ReadHexByte(input_file);

    ReadHexByte(input_file);              /* skip expansion byte */

    for(i=0;i<count;++i)
		if( (c = ReadHexByte(input_file)) != 0x100)
		    putc((char)c,output_file);
		else
		 {
	    	printf("unexpected end of file\n");
	    	return(0);
		 }
    ReadHexByte(input_file);              /* skip checksum */
    getc(input_file);                       /* skip line feed */
    return(1);
}

/*-------------------------------------------------------------------------*/
