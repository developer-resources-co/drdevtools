//==============================================================================
// send.hpp
//==============================================================================

extern "C"
	{
	void RecFast (char* buffer,long size);
	void SendByte (char a);
	unsigned char RecByte (void);
	void SendWord (unsigned int a);
	void SendLong (long a);
	void SendFast (char* buffer,long size);
	void ResumeSlave (void);
	int GetAttention (void);
	unsigned long SwapLongWords (unsigned long a);
	unsigned long SwapLong (unsigned long a);
	unsigned int SwapWord (unsigned int a);
//	void ResetPort (void);							CLASS
	unsigned int SlaveSetBkPt(long);
	void SlaveClrBkPt(long,unsigned int);
	void SendCmd (char a);
	void GetSlaveBus(void);
	void ReturnSlaveBus(void);
	};

//==============================================================================
