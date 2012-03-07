#pragma once

class Player
{
protected:
	HANDLE hProcess;
	PBYTE baseAddress;
	PBYTE GetPlayerBase();
	PBYTE GetPlayerFlagsBase();
	PBYTE GetPlayerMovementInfoBase();

public:
	Player(HANDLE hProc, PBYTE baseAddr) : hProcess(hProc), baseAddress(baseAddr) {}
	~Player() {}

	// Utility functions
	float DistanceFrom(float X, float Y, float Z);
	float DistanceFrom(Vec3 position) { return DistanceFrom(position.X, position.Y, position.Z); }
	bool SetFacing(float X, float Y, float Z);
	bool SetFacing(Vec3 position) { return SetFacing(position.X, position.Y, position.Z); }

	// Memory functions
	bool HasFlags(DWORD flags);
	bool SetFlags(DWORD flags);
	bool RemoveFlags(DWORD flags);
	DWORD GetFlags();
	bool SetCommentatorMode(bool bEnable);
	bool IsInCommentatorMode();
	float GetCommentatorCameraYaw();
	bool SetCommentatorCameraYaw(float newYaw);
	float GetCommentatorCameraPitch();
	bool SetCommentatorCameraPitch(float newPitch);
	float GetCommentatorCameraPosX();
	bool SetCommentatorCameraPosX(float X);
	float GetCommentatorCameraPosY();
	bool SetCommentatorCameraPosY(float Y);
	float GetCommentatorCameraPosZ();
	bool SetCommentatorCameraPosZ(float Z);
	Vec3 GetCommentatorCameraPosition();
	bool SetCommentatorCameraPosition(float X, float Y, float Z);
	bool SetCommentatorCameraPosition(Vec3 pos);
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
