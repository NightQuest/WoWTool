class WoWManager
{
private:
	HANDLE hProcess;
	PBYTE baseAddress;

	WoWPlayer *plr;

	// Basic helper functions

	// WoW helper functions

public:
	WoWManager();
	~WoWManager();

	// Basic utility functions
	bool Attach(DWORD dwPID);
	bool Launch(TCHAR *path, TCHAR *commandline);

	// These are basic enable/disable/etc functions
	bool SetAnimationSpeed(double speed);
	bool SetGameSpeed(float speed);

	// Misc.
	WoWPlayer *GetPlayer() { return plr; }
};