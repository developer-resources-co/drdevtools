
class Ini
	{
public:
	Ini( char* progName );
//	Ini( char* progName, char*  );

	boolean Boolean( char* szItem, boolean bDefault );
	int Int( char* szItem, int nDefault );
	

private:
	char szToolIni[ 80 ];
	char szToolName[ 50 ];

	char szDrive[10];
	char szDir[60];
	char szFile[10];
	char szExt[4];
	


	};
