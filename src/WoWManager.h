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

	// Engine functions
	bool SetAnimationSpeed(double speed);
	bool SetGameSpeed(double speed);

	// Returns an instance to the WoWPlayer class that contains Player specific functions
	WoWPlayer *GetPlayer() { return plr; }
};