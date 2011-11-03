#include "main.h"

// Returns the Base address of the player block in memory - INTERNAL
PBYTE WoWPlayer::GetPlayerBase()
{
	PBYTE plr = NULL;
	SIZE_T size = 0;

	// Retrieve the base address of the player class from the game's memory.
	if( !ReadProcessMemory(hProcess, (baseAddress + PLAYER_BASE_PTR), &plr, sizeof(PBYTE), &size) )
		return NULL;

	return plr;
}

// Returns whether or not the player is in spectate mode
bool WoWPlayer::IsSpectating()
{
	PBYTE Plr = GetPlayerBase();
	if( Plr == NULL )
		return false;

	PBYTE SpectatePtr = NULL;
	DWORD SpectateMode = NULL;
	SIZE_T size = 0;

	// Read the Spectate Mode pointer
	// The pointer to the spectate mode class is located 0x1190 bytes from the player base
	if( !ReadProcessMemory(hProcess, (Plr + PLAYER_SPECTATE_MODE_OFFSET_ONE), &SpectatePtr, sizeof(PBYTE), &size) || size != sizeof(PBYTE) )
		return false;

	// Read 8 bytes past the spectate mode class base to retrieve the bitmask that allows us to enable/disable it
	if( !ReadProcessMemory(hProcess, (SpectatePtr + PLAYER_SPECTATE_MODE_OFFSET_TWO), &SpectateMode, sizeof(DWORD), &size) || size != sizeof(DWORD) )
		return false;

	if( SpectateMode & PLAYER_SPECTATE_MODE_VALUE )
		return true;
	else
		return false;
}

// Toggles spectate mode, returns true on success
bool WoWPlayer::SetSpectateMode(bool bEnable)
{
	PBYTE Plr = GetPlayerBase();
	if( Plr == NULL )
		return false;

	PBYTE SpectatePtr = NULL;
	DWORD SpectateMode = NULL;
	SIZE_T size = 0;

	// Read the Spectate Mode pointer
	// The pointer to the spectate mode class is located 0x1190 bytes from the player base
	if( !ReadProcessMemory(hProcess, (Plr + PLAYER_SPECTATE_MODE_OFFSET_ONE), &SpectatePtr, sizeof(PBYTE), &size) || size != sizeof(PBYTE) )
		return false;

	// Read 8 bytes past the spectate mode class base to retrieve the bitmask that allows us to enable/disable it
	if( !ReadProcessMemory(hProcess, (SpectatePtr + PLAYER_SPECTATE_MODE_OFFSET_TWO), &SpectateMode, sizeof(DWORD), &size) || size != sizeof(DWORD) )
		return false;

	if( bEnable )
	{
		// Read the player's position
		Vec4 pos = GetPosition();

		// Increase the Z axis so the camera doesn't appear at ground level
		pos.Z += 10.0f;

		// Copy the player's position to the spectate camera's position, with a slight offset to Z
		if( !WriteProcessMemory(hProcess, (baseAddress + PLAYER_SPECTATE_X), &pos.X, sizeof(float), &size) || size != sizeof(float) )
			return false;
		if( !WriteProcessMemory(hProcess, (baseAddress + PLAYER_SPECTATE_Y), &pos.Y, sizeof(float), &size) || size != sizeof(float) )
			return false;
		if( !WriteProcessMemory(hProcess, (baseAddress + PLAYER_SPECTATE_Z), &pos.Z, sizeof(float), &size) || size != sizeof(float) )
			return false;

		SpectateMode |= PLAYER_SPECTATE_MODE_VALUE;
	}
	else
	{
		SpectateMode &= ~PLAYER_SPECTATE_MODE_VALUE;
	}

	// Write SpectateMode back to memory, to either enable or disable it.
	if( !WriteProcessMemory(hProcess, (SpectatePtr + PLAYER_SPECTATE_MODE_OFFSET_TWO), &SpectateMode, sizeof(DWORD), &size) || size != sizeof(DWORD) )
		return false;

	return true;
}

// Sets whether or not the camera will collide with terrian, return true on success
bool WoWPlayer::SetSpectateCollision(bool bEnable)
{
	SIZE_T size = 0;
	if( !WriteProcessMemory(hProcess, (baseAddress + PLAYER_SPECTATE_COLLISION), &bEnable, sizeof(bool), &size) || size != sizeof(bool) )
		return false;
	return true;
}

// Return whether or not the spectate camera will collide with terrian
bool WoWPlayer::IsSpectateModeCollidable()
{
	SIZE_T size;
	bool bCollision = false;

	if( !ReadProcessMemory(hProcess, (baseAddress + PLAYER_SPECTATE_COLLISION), &bCollision, sizeof(bool), &size) || size != sizeof(bool) )
		return false;

	return bCollision;
}

// Returns a XYZO vector of the player's coordinates
Vec4 WoWPlayer::GetPosition()
{
	Vec4 pos = { NULL };
	PBYTE Plr = GetPlayerBase();
	SIZE_T size = 0;
	if( Plr == NULL )
		return pos;

	// Read the current position of the player
	ReadProcessMemory(hProcess, (Plr + PLAYER_POSITION_X_OFFSET), &pos, sizeof(Vec4), &size);
	return pos;
}

// Returns the player'S X coordinate
float WoWPlayer::GetPosX()
{
	PBYTE Plr = GetPlayerBase();
	if( Plr == NULL )
		return false;

	float pX = 0.0f;
	SIZE_T size = 0;

	// Read the current position of the player's X coordinate
	ReadProcessMemory(hProcess, (Plr + PLAYER_POSITION_X_OFFSET), &pX, sizeof(float), &size);

	return pX;
}

// Returns the player's Y coordinate
float WoWPlayer::GetPosY()
{
	PBYTE Plr = GetPlayerBase();
	if( Plr == NULL )
		return false;

	float pY = 0.0f;
	SIZE_T size = 0;

	// Read the current position of the player's Y coordinate
	ReadProcessMemory(hProcess, (Plr + PLAYER_POSITION_Y_OFFSET), &pY, sizeof(float), &size);

	return pY;
}

// Returns the player's Z coordinate
float WoWPlayer::GetPosZ()
{
	PBYTE Plr = GetPlayerBase();
	if( Plr == NULL )
		return false;

	float pZ = 0.0f;
	SIZE_T size = 0;

	// Read the current position of the player's Z coordinate
	ReadProcessMemory(hProcess, (Plr + PLAYER_POSITION_Z_OFFSET), &pZ, sizeof(float), &size);

	return pZ;
}

// Returns the player's orientation
float WoWPlayer::GetPosO()
{
	PBYTE Plr = GetPlayerBase();
	if( Plr == NULL )
		return false;

	float pO = 0.0f;
	SIZE_T size = 0;

	// Read the current position of the player's orientation
	ReadProcessMemory(hProcess, (Plr + PLAYER_POSITION_O_OFFSET), &pO, sizeof(float), &size);

	return pO;
}

// Returns a XYZO vector of the player's coordinates
bool WoWPlayer::SetPosition(Vec4 pos)
{
	SIZE_T size = 0;
	PBYTE Plr = GetPlayerBase();
	if( Plr == NULL )
		return false;

	// Set the current position of the player
	if( !WriteProcessMemory(hProcess, (Plr + PLAYER_POSITION_X_OFFSET), &pos, sizeof(Vec4), &size) || size != sizeof(Vec4) )
		return false;

	return true;
}

// Set the Player's X coordinate, returns true on success
bool WoWPlayer::SetPosX(float newX)
{
	SIZE_T size = 0;
	PBYTE Plr = GetPlayerBase();
	if( Plr == NULL )
		return false;

	// Set the current position of the player's X coordinate
	if( !WriteProcessMemory(hProcess, (Plr + PLAYER_POSITION_X_OFFSET), &newX, sizeof(float), &size) || size != sizeof(float) )
		return false;

	return true;
}

// Set the Player's Y coordinate, returns true on success
bool WoWPlayer::SetPosY(float newY)
{
	SIZE_T size = 0;
	PBYTE Plr = GetPlayerBase();
	if( Plr == NULL )
		return false;

	// Set the current position of the player's Y coordinate
	if( !WriteProcessMemory(hProcess, (Plr + PLAYER_POSITION_Y_OFFSET), &newY, sizeof(float), &size) || size != sizeof(float) )
		return false;

	return true;
}

// Set the Player's X coordinate, returns true on success
bool WoWPlayer::SetPosZ(float newZ)
{
	SIZE_T size = 0;
	PBYTE Plr = GetPlayerBase();
	if( Plr == NULL )
		return false;

	// Set the current position of the player's Z coordinate
	if( !WriteProcessMemory(hProcess, (Plr + PLAYER_POSITION_Z_OFFSET), &newZ, sizeof(float), &size) || size != sizeof(float) )
		return false;

	return true;
}

// Set the Player's orientation, returns true on success
bool WoWPlayer::SetPosO(float newO)
{
	SIZE_T size = 0;
	PBYTE Plr = GetPlayerBase();
	if( Plr == NULL )
		return false;

	// Set the current position of the player's orientation
	if( !WriteProcessMemory(hProcess, (Plr + PLAYER_POSITION_O_OFFSET), &newO, sizeof(float), &size) || size != sizeof(float) )
		return false;

	return true;
}
