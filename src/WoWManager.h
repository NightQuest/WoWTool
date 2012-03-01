#pragma once

class WoWManager
{
private:
	HANDLE hProcess;
	DWORD dwPID;
	PBYTE baseAddress;
	UINT gameVersion;

	Player *plr;
	Camera *cam;

	// Basic helper functions
	void Initialize();
	void Deinitialize();

	// WoW helper functions

public:
	WoWManager();
	~WoWManager();

	// Basic utility functions
	bool Attach(DWORD dwPID);
	bool IsAttached();
	bool Launch(TCHAR *path, TCHAR *commandline);
	TCHAR *GetProgramLocation();


	// Engine functions
	bool SetAnimationSpeed(double speed);
	bool SetGameSpeed(double speed);

	// Returns an instance to the Player class that contains Player specific functions
	Player *GetPlayer() { return plr; }
	Camera *GetCamera() { return cam; }
};