#pragma once

class WoWCamera
{
protected:
	HANDLE hProcess;
	PBYTE baseAddress;
	PBYTE GetCameraBase();

public:
	WoWCamera(HANDLE hProc, PBYTE baseAddr) : hProcess(hProc), baseAddress(baseAddr) {}
	~WoWCamera() {}

	float GetFieldOfView();
	bool SetFieldOfView(float newFov);
};
