#pragma once

class WoWManager
{
private:
	HANDLE hProcess;
	DWORD dwPID;
	PBYTE baseAddress;
	UINT gameVersion;
	TCHAR *gameLocation;

	Engine *eng;
	Player *plr;
	Camera *cam;

	// Basic helper functions
	void Initialize();
	void Deinitialize();
	void Patch();
	void Depatch();

	// WoW helper functions

public:
	WoWManager();
	~WoWManager();

	// Basic utility functions
	bool Attach(DWORD dwPID);
	bool IsAttached();
	bool Launch(TCHAR *path, TCHAR *commandline);
	TCHAR *GetGameLocation();
	UINT GetGameVersion();

	// Basic helper functions
	bool HasPatchedSkyPosition();
	bool PatchSkyPosition();
	bool DepatchSkyPosition();

	// Returns an instance to the Player class that contains Player specific functions
	Engine *GetEngine() { return eng; }
	Player *GetPlayer() { return plr; }
	Camera *GetCamera() { return cam; }
};