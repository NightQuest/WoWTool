#define ENGINE_SPEED_OF_ANIMATION	0x0048F750
#define ENGINE_GAME_SPEED			0x0048F608

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
	bool SetGameSpeed(double speed);

	// Returns an instance to the WoWPlayer class that contains Player specific functions
	WoWPlayer *GetPlayer() { return plr; }
};