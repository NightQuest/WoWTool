#pragma once

class Engine
{
private:
	HANDLE hProcess;
	PBYTE baseAddress;

public:
	Engine(HANDLE hProc, PBYTE baseAddr) : hProcess(hProc), baseAddress(baseAddr) {}
	~Engine() {}

	DWORD GetRenderingFlags();
	bool HasRenderingFlags(DWORD flags);
	bool SetRenderingFlags(DWORD flags);
	bool RemoveRenderingFlags(DWORD flags);

	float GetSkyPosition();
	bool SetSkyPosition(float position);
	bool DrawSky(bool bDrawSky);

	bool SetAnimationSpeed(float speed);
	bool SetGameSpeed(double speed);
	float GetAnimationSpeed();
	double GetGameSpeed();

};