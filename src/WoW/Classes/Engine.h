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

	bool SetAnimationSpeed(double speed);
	bool SetGameSpeed(double speed);
	double GetAnimationSpeed();
	double GetGameSpeed();

};