#define PLAYER_BASE_PTR					0x00A28D28
#define PLAYER_SPECTATE_MODE_VALUE		0x43780000
#define PLAYER_SPECTATE_MODE_OFFSET_ONE 0x00001190
#define PLAYER_SPECTATE_MODE_OFFSET_TWO 0x00000008
#define PLAYER_SPECTATE_X				0x008EE474
#define PLAYER_SPECTATE_Y				0x008EE478
#define PLAYER_SPECTATE_Z				0x008EE47C
#define PLAYER_SPECTATE_COLLISION		0x008EE4B0
#define PLAYER_POSITION_X_OFFSET		0x00000BF0
#define PLAYER_POSITION_Y_OFFSET		0x00000BF4
#define PLAYER_POSITION_Z_OFFSET		0x00000BF8
#define PLAYER_POSITION_O_OFFSET		0x00000BFC

struct Vec4
{
	float X;
	float Y;
	float Z;
	float O;
};

class WoWPlayer
{
private:
	HANDLE hProcess;
	PBYTE baseAddress;
	PBYTE GetPlayerBase();

public:
	WoWPlayer(HANDLE hProc, PBYTE baseAddr) : hProcess(hProc), baseAddress(baseAddr) {}
	~WoWPlayer() {}

	bool SetSpectateMode(bool bEnable);
	bool IsSpectating();
	bool SetSpectateCollision(bool bEnable);
	bool IsSpectateModeCollidable();
	Vec4 GetPosition();
	float GetPosX();
	float GetPosY();
	float GetPosZ();
	float GetPosO();
	bool SetPosX(float newX);
	bool SetPosY(float newY);
	bool SetPosZ(float newZ);
	bool SetPosO(float newO);
};
