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
	bool SetPosition(Vec4 pos);
	bool SetPosX(float newX);
	bool SetPosY(float newY);
	bool SetPosZ(float newZ);
	bool SetPosO(float newO);
};
