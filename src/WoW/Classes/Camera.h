#pragma once

class Camera
{
protected:
	HANDLE hProcess;
	PBYTE baseAddress;
	PBYTE GetCameraBase();

public:
	Camera(HANDLE hProc, PBYTE baseAddr) : hProcess(hProc), baseAddress(baseAddr) {}
	~Camera() {}

	float GetFieldOfView();
	bool SetFieldOfView(float newFov);
	float GetYaw();
	float GetPitch();
	float GetRoll();
	bool SetYaw(float newYaw);
	bool SetPitch(float newPitch);
	bool SetRoll(float newRoll);
};
