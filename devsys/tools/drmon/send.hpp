//==============================================================================
// send.hpp
//==============================================================================

extern "C"
	{
	unsigned long _cdecl SwapLong (unsigned long a);	// kts moveed 10-14-93 00:52am

	void RecFast (char far *buffer,long size);
	void SendByte (char a);
	unsigned char RecByte (void);
	void SendWord (unsigned int a);
	void SendLong (long a);
	void SendFast (char far *buffer,long size);
	void ResumeSlave (void);
	int GetAttention (void);
	unsigned long SwapLongWords (unsigned long a);
	unsigned int SwapWord (unsigned int a);
//	void ResetPort (void);							CLASS
	uword SlaveSetBkPt( ulong );
	void SlaveClrBkPt(long,unsigned int);
	void SendCmd ( char a);
	void GetSlaveBus(void);
	void ReturnSlaveBus(void);
	};

//==============================================================================
