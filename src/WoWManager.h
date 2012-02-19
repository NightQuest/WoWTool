#pragma once

class WoWManager
{
private:
	HANDLE hProcess;
	DWORD dwPID;
	PBYTE baseAddress;

	WoWPlayer *plr;
	WoWCamera *cam;

	// Basic helper functions

	// WoW helper functions

public:
	WoWManager();
	~WoWManager();

	// Basic utility functions
	bool Attach(DWORD dwPID);
	bool Launch(TCHAR *path, TCHAR *commandline);
	TCHAR *GetProgramLocation();


	// Engine functions
	bool SetAnimationSpeed(double speed);
	bool SetGameSpeed(double speed);

	// Returns an instance to the WoWPlayer class that contains Player specific functions
	WoWPlayer *GetPlayer() { return plr; }
	WoWCamera *GetCamera() { return cam; }
};