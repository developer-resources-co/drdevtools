//=============================================================================

int
LineLen(char *text)
{
	int len = 0;
	if(*text == 0)
		return(0);
	while(*text)
	 {
		len++;
		if(*text == '\n')
			return(len);
		text++;
	 }
	return(len);
}

//=============================================================================

int
CountLines(char *text)
{
	int lines = 1;
	if(*text == 0)
		return(0);
	while(*text)
	 {
		if(*text == '\n')
			lines++;
		text++;
	 }
	return(lines);
}


int
WidestLine(char *text)
{
	int count = 0,maxCount = 0;

	while(*text)
	 {
		if(*text == '\n')
		 {
			if(count > maxCount)
				maxCount = count;
			count = 0;
		 }
		else
			count++;
		text++;
	 }
	if(count > maxCount)
		maxCount = count;
	return(maxCount);
}

//==============================================================================
