#pragma once

class WoWPlayer
{
protected:
	HANDLE hProcess;
	PBYTE baseAddress;
	PBYTE GetPlayerBase();
	PBYTE GetPlayerFlagsBase();

public:
	WoWPlayer(HANDLE hProc, PBYTE baseAddr) : hProcess(hProc), baseAddress(baseAddr) {}
	~WoWPlayer() {}

	bool HasFlags(DWORD flags);
	bool SetFlags(DWORD flags);
	bool RemoveFlags(DWORD flags);
	DWORD GetFlags();
	bool SetCommentatorMode(bool bEnable);
	bool IsInCommentatorMode();
	bool SetCommentatorCameraSpeed(float speed);
	float GetCommentatorCameraSpeed();
	bool SetCommentatorCameraCollision(bool bEnable);
	bool IsCommentatorCameraCollidable();
	Vec4 GetPosition();
	float GetPosX();
	float GetPosY();
	float GetPosZ();
	float GetPosO();
	bool SetPosition(Vec3 pos);
	bool SetPosition(Vec4 pos);
	bool SetPosition(float X, float Y, float Z, float O = 0.0f);
	bool SetPosX(float newX);
	bool SetPosY(float newY);
	bool SetPosZ(float newZ);
	bool SetPosO(float newO);
};
