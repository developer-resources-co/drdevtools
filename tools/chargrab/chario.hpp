
#include <pclib/grphport.hpp>

class CharacterData
	{
public:
	CharacterData()			{ }
	~CharacterData()		{ }
	virtual boolean MakeChar( rastGraphPort* rp, int x, int y ) = 0;
	virtual void CreateXFlip() = 0;
	virtual void CreateYFlip() = 0;
	virtual void CreateXYFlip() = 0;
	virtual int xFlip() = 0;
	virtual int yFlip() = 0;

	int nMaxPalette;
	};


class GenesisCharacterData : public CharacterData
	{
public:
	GenesisCharacterData();
	~GenesisCharacterData()		{ }
	virtual boolean MakeChar( rastGraphPort* rp, int x, int y );
	virtual void CreateXFlip();
	virtual void CreateYFlip();
	virtual void CreateXYFlip();
	virtual int xFlip()		{ return 0x800; }
	virtual int yFlip()		{ return 0x1000; }
	};


class SnesCharacterData : public CharacterData
	{
	int charPlanes;
	int planeOffset[8];
	int maskArray[1+8];

public:
	SnesCharacterData();
	~SnesCharacterData()		{ }
	virtual boolean MakeChar( rastGraphPort* rp, int x, int y );
	virtual void CreateXFlip();
	virtual void CreateYFlip();
	virtual void CreateXYFlip();
	virtual int xFlip()		{ return 0x4000; }
	virtual int yFlip()		{ return 0x8000; }
	};

