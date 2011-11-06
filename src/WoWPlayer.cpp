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

// Returns the base address of the player flags in memory - INTERNAL
PBYTE WoWPlayer::GetPlayerFlagsBase()
{
	PBYTE Plr = GetPlayerBase();
	if( Plr == NULL )
		return false;

	PBYTE FlagsPtr = NULL;
	SIZE_T size = 0;

	// Read the Player Flags pointer
	// The pointer to the player flags is located 0x1190 bytes from the player base
	if( !ReadProcessMemory(hProcess, (Plr + PLAYER_FLAGS_OFFSET_ONE), &FlagsPtr, sizeof(PBYTE), &size) || size != sizeof(PBYTE) )
		return NULL;

	// Return 8 bytes past the base to be able to retrieve the player flags bitmask
	return (FlagsPtr ? (FlagsPtr  + PLAYER_FLAGS_OFFSET_TWO) : NULL);
}

// Returns the player flags bitmask, or NULL on failure
DWORD WoWPlayer::GetFlags()
{
	PBYTE FlagsPtr = GetPlayerFlagsBase();
	DWORD PlrFlags = NULL;
	SIZE_T size = 0;

	if( !FlagsPtr )
		return NULL;

	// Read 8 bytes past the base to retrieve the player flags bitmask
	if( !ReadProcessMemory(hProcess, FlagsPtr, &PlrFlags, sizeof(DWORD), &size) || size != sizeof(DWORD) )
		return NULL;

	return PlrFlags;
}

// Checks to see if the passed bitmask exists in the player flags bitmask.
// Returns true on success.
bool WoWPlayer::HasFlags(DWORD flags) { return (GetFlags() & flags) > 0; }

// Does a bitwise OR operation on the player flags, adding whatever is passed.
// Returns true on success.
bool WoWPlayer::SetFlags(DWORD flags)
{
	PBYTE FlagsPtr = GetPlayerFlagsBase();
	DWORD PlrFlags = NULL;
	SIZE_T size = 0;

	if( !FlagsPtr )
		return false;

	// Read the player flags and do a bitwise OR on them, adding the passed flags to them
	PlrFlags = GetFlags() | flags;

	// Write the new flags back to the player flags in memory
	if( !WriteProcessMemory(hProcess, FlagsPtr, &PlrFlags, sizeof(DWORD), &size) || size != sizeof(DWORD) )
		return false;

	return true;
}

// Does a bitwise inverse OR on the player flags, removing whatever is passed.
// Returns true on success.
bool WoWPlayer::RemoveFlags(DWORD flags)
{
	PBYTE FlagsPtr = GetPlayerFlagsBase();
	DWORD PlrFlags = NULL;
	SIZE_T size = 0;

	if( !FlagsPtr )
		return false;

	// Read the player flags and do a bitwise inverse AND on them, removing the passed flags from them
	PlrFlags = GetFlags() & ~flags;

	// Write the new flags back to the player flags in memory
	if( !WriteProcessMemory(hProcess, FlagsPtr, &PlrFlags, sizeof(DWORD), &size) || size != sizeof(DWORD) )
		return false;

	return true;
}

// Returns whether or not the player is in commentator mode
bool WoWPlayer::IsInCommentatorMode()
{
	return HasFlags(PLAYER_FLAGS_COMMENTATOR|PLAYER_FLAGS_COMMENTATOR_CAN_USE_COMMANDS);
}

// Toggles commentator mode
// Returns true on success
bool WoWPlayer::SetCommentatorMode(bool bEnable)
{
	PBYTE Plr = GetPlayerBase();
	if( Plr == NULL )
		return false;

	// Retrieve the player flags
	DWORD PlrFlags = GetFlags();
	SIZE_T size = 0;

	if( bEnable )
	{
		// Read the player's position
		Vec4 pos = GetPosition();

		// Increase the Z axis so the camera doesn't appear at ground level
		pos.Z += 10.0f;

		// Copy the player's position to the commentator camera's position, with a slight offset to Z
		if( !WriteProcessMemory(hProcess, (baseAddress + PLAYER_COMMENTATOR_X), &pos.X, sizeof(float), &size) || size != sizeof(float) )
			return false;
		if( !WriteProcessMemory(hProcess, (baseAddress + PLAYER_COMMENTATOR_Y), &pos.Y, sizeof(float), &size) || size != sizeof(float) )
			return false;
		if( !WriteProcessMemory(hProcess, (baseAddress + PLAYER_COMMENTATOR_Z), &pos.Z, sizeof(float), &size) || size != sizeof(float) )
			return false;

		// Write the player flags back to memory, to either enable or disable commentator mode
		if( !SetFlags(PLAYER_FLAGS_COMMENTATOR|PLAYER_FLAGS_COMMENTATOR_CAN_USE_COMMANDS) )
			return false;
	}
	else
	{
		// Write the player flags back to memory, to either enable or disable commentator mode
		if( !RemoveFlags(PLAYER_FLAGS_COMMENTATOR|PLAYER_FLAGS_COMMENTATOR_CAN_USE_COMMANDS) )
			return false;
	}

	return true;
}

// Sets whether or not the commentator camera will collide with terrian
// Returns true on success
bool WoWPlayer::SetCommentatorCameraCollision(bool bEnable)
{
	SIZE_T size = 0;
	if( !WriteProcessMemory(hProcess, (baseAddress + PLAYER_COMMENTATOR_COLLISION), &bEnable, sizeof(bool), &size) || size != sizeof(bool) )
		return false;
	return true;
}

// Returns whether or not the commentator camera will collide with terrian
bool WoWPlayer::IsCommentatorCameraCollidable()
{
	SIZE_T size;
	bool bCollision = false;

	if( !ReadProcessMemory(hProcess, (baseAddress + PLAYER_COMMENTATOR_COLLISION), &bCollision, sizeof(bool), &size) || size != sizeof(bool) )
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

// Returns the player's orientation (Tau)
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

// Sets the XYZO coordinates of the player using a vector
// Returns true on success
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

// Sets the XYZO coordinates of the player
// Returns true on success
bool WoWPlayer::SetPosition(float X, float Y, float Z, float O)
{
	Vec4 pos = { X, Y, Z, O };
	return SetPosition(pos);
}

// Set the Player's X coordinate
// Returns true on success
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

// Set the Player's Y coordinate
// Returns true on success
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

// Set the Player's X coordinate
// Returns true on success
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

// Set the Player's orientation (Tau)
// Returns true on success
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
