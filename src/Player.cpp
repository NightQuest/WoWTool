#include "PreCompiled.h"

// Returns the Base address of the player block in memory - INTERNAL
PBYTE Player::GetPlayerBase()
{
	PBYTE plr = NULL;
	SIZE_T size = 0;

	// Retrieve the base address of the player class from the game's memory.
	if( !ReadProcessMemory(hProcess, (baseAddress + PLAYER_BASE_PTR_8606), &plr, sizeof(PBYTE), &size) )
		return NULL;

	return plr;
}

// Returns the base address of the player flags in memory - INTERNAL
PBYTE Player::GetPlayerFlagsBase()
{
	PBYTE Plr = GetPlayerBase();
	if( Plr == NULL )
		return false;

	PBYTE FlagsPtr = NULL;
	SIZE_T size = 0;

	// Read the Player Flags pointer
	// The pointer to the player flags is located 0x1190 bytes from the player base
	if( !ReadProcessMemory(hProcess, (Plr + PLAYER_FIELDS_OFFSET_8606), &FlagsPtr, sizeof(PBYTE), &size) || size != sizeof(PBYTE) )
		return NULL;

	// Return 8 bytes past the base to be able to retrieve the player flags bitmask
	return (FlagsPtr ? (FlagsPtr  + PLAYER_FIELDS_FLAGS_8606) : NULL);
}

// Returns the base address of the player movement info in memory - INTERNAL
PBYTE Player::GetPlayerMovementInfoBase()
{
	PBYTE Plr = GetPlayerBase();
	if( Plr == NULL )
		return false;

	PBYTE MovementInfo = NULL;
	SIZE_T size = 0;

	// Read the Player MovementInfo pointer
	if( !ReadProcessMemory(hProcess, (Plr + PLAYER_MOVEMENT_INFO_OFFSET_8606), &MovementInfo, sizeof(PBYTE), &size) || size != sizeof(PBYTE) )
		return NULL;

	return MovementInfo;
}

// Calculates the distance from the specified coordinates
// Returns distance on success, 0.0f on failure
float Player::DistanceFrom(float X, float Y, float Z)
{
	Vec4 position = GetPosition();
	if( position.X == NULL )
		return 0.0f;

	return sqrt( pow(position.X - X, 2) + pow(position.Y - Y, 2) + pow(position.Z - Z, 2) );
}

// Turns the player to face a certain set of coordinates
// Returns true on success
bool Player::SetFacing(float X, float Y, float Z)
{
	Vec4 newPos;
	Vec4 position = GetPosition();
	if( position.X == NULL )
		return false;

	newPos.X = position.X - X;
	newPos.Y = position.Y - Y;

	if( newPos.X < 0.0f )
		newPos.X *= -1.0f;
	if( newPos.Y < 0.0f )
		newPos.Y *= -1.0f;

	if( X > position.X && Y > position.Y )
		newPos.O = atan(newPos.Y/newPos.X) * 180.f / (float)M_PI;
	else if( X > position.X && Y < position.Y )
		newPos.O = (((atan(newPos.Y/newPos.X) * 180.0f / (float)M_PI) * -1.0f) + 90.0f) + 270.0f;
	else if( X < position.X && Y < position.Y )
		newPos.O = (atan(newPos.Y/newPos.X) * 180.0f / (float)M_PI) + 180.0f;
	else if( X > position.X && Y > position.Y )
		newPos.O = (((atan(newPos.Y/newPos.X) * 180.0f / (float)M_PI) * -1.0f) + 90.0f) + 90.0f;

	return SetPosO( ((float)M_PI*2)*newPos.O );
}

// Returns the player flags bitmask, or NULL on failure
DWORD Player::GetFlags()
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
bool Player::HasFlags(DWORD flags) { return ((GetFlags() & flags) == flags); }

// Does a bitwise OR operation on the player flags, adding whatever is passed.
// Returns true on success.
bool Player::SetFlags(DWORD flags)
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
bool Player::RemoveFlags(DWORD flags)
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
bool Player::IsInCommentatorMode()
{
	return HasFlags(PLAYER_FLAG_COMMENTATOR|PLAYER_FLAG_CAN_USE_COMMENTATOR_COMMANDS);
}

// Toggles commentator mode
// Returns true on success
bool Player::SetCommentatorMode(bool bEnable)
{
	SIZE_T size = 0;

	if( bEnable )
	{
		// Read the players position
		Vec4 pos = GetPosition();

		// Increase the Z axis so the camera doesn't appear at ground level
		pos.Z += 10.0f;

		// Copy the players position to the commentator camera's position, with a slight offset to Z
		if( !SetCommentatorCameraPosX(pos.X) )
			return false;
		if( !SetCommentatorCameraPosY(pos.Y) )
			return false;
		if( !SetCommentatorCameraPosZ(pos.Z) )
			return false;

		// Write the player flags back to memory, to enable commentator mode
		return SetFlags(PLAYER_FLAG_COMMENTATOR|PLAYER_FLAG_CAN_USE_COMMENTATOR_COMMANDS);
	}

	// Write the player flags back to memory, to disable commentator mode
	return RemoveFlags(PLAYER_FLAG_COMMENTATOR|PLAYER_FLAG_CAN_USE_COMMENTATOR_COMMANDS);
}

// Retrieves the commentator camera Yaw (in degrees)
float Player::GetCommentatorCameraYaw()
{
	SIZE_T size;
	float yaw = 0.0f;

	// Read the Commentator's Camera's Yaw
	if( !ReadProcessMemory(hProcess, (baseAddress + COMMENTATOR_YAW_8606), &yaw, sizeof(float), &size) || size != sizeof(float) )
		return 0.0f;

	// Return the result in degrees, not radians
	return (yaw * (360.0f/(float)(M_PI*2)));
}

// Sets the commentator camera Yaw (passed in degrees)
// Returns true on success
bool Player::SetCommentatorCameraYaw(float newYaw)
{
	SIZE_T size = 0;

	while( newYaw > 360.0f )
		newYaw -= 360.0f;

	// Convert the yaw from degrees to radians
	newYaw *= ((float)(M_PI*2)/360.0f);

	// Set the new yaw
	if( !WriteProcessMemory(hProcess, (baseAddress + COMMENTATOR_YAW_8606), &newYaw, sizeof(float), &size) || size != sizeof(float) )
		return false;

	return true;
}

// Retrieves the commentator camera Pitch (in degrees)
float Player::GetCommentatorCameraPitch()
{
	SIZE_T size;
	float pitch = 0.0f;

	if( !ReadProcessMemory(hProcess, (baseAddress + COMMENTATOR_PITCH_8606), &pitch, sizeof(float), &size) || size != sizeof(float) )
		return 0.0f;

	// Pitch is stored from negative PI/2 to positive PI/2, so we add PI/2 before converting to degrees
	return ((pitch + (float)M_PI/2) * (180.0f/(float)M_PI));
}

// Sets the commentator camera Pitch (passed in degrees)
// Returns true on success
bool Player::SetCommentatorCameraPitch(float newPitch)
{
	SIZE_T size;

	// Must be at or below 180 degrees
	while( newPitch > 180.0f )
		newPitch -= 180.0f;

	// Convert to radians, then remove MP/2 since pitch is stored from negative PI/2 to positive PI/2
	newPitch = (newPitch * ((float)M_PI/180.0f)) + ((float)M_PI/2);

	// Write the new pitch to memory
	if( !WriteProcessMemory(hProcess, (baseAddress + COMMENTATOR_PITCH_8606), &newPitch, sizeof(float), &size) || size != sizeof(float) )
		return false;

	return true;
}

// Retrieves the commentator camera's X position
float Player::GetCommentatorCameraPosX()
{
	SIZE_T size;
	float X = 0.0f;

	if( !ReadProcessMemory(hProcess, (baseAddress + COMMENTATOR_POSITION_X_8606), &X, sizeof(float), &size) || size != sizeof(float))
		return 0.0f;

	return X;
}

// Sets the commentator camera's X position
// Returns true on success
bool Player::SetCommentatorCameraPosX(float X)
{
	SIZE_T size = 0;

	if( !WriteProcessMemory(hProcess, (baseAddress + COMMENTATOR_POSITION_X_8606), &X, sizeof(float), &size) || size != sizeof(float) )
		return false;

	return true;
}

// Retrieves the commentator camera's Y position
float Player::GetCommentatorCameraPosY()
{
	SIZE_T size;
	float Y = 0.0f;

	if( !ReadProcessMemory(hProcess, (baseAddress + COMMENTATOR_POSITION_Y_8606), &Y, sizeof(float), &size) || size != sizeof(float) )
		return 0.0f;

	return Y;
}

// Sets the commentator camera's Y position
// Returns true on success
bool Player::SetCommentatorCameraPosY(float Y)
{
	SIZE_T size = 0;

	if( !WriteProcessMemory(hProcess, (baseAddress + COMMENTATOR_POSITION_Y_8606), &Y, sizeof(float), &size) || size != sizeof(float) )
		return false;

	return true;
}

// Retrieves the commentator camera's Z position
float Player::GetCommentatorCameraPosZ()
{
	SIZE_T size;
	float Z = 0.0f;

	if( !ReadProcessMemory(hProcess, (baseAddress + COMMENTATOR_POSITION_Z_8606), &Z, sizeof(float), &size) || size != sizeof(float) )
		return 0.0f;

	return Z;
}

// Sets the commentator camera's Z position
// Returns true on success
bool Player::SetCommentatorCameraPosZ(float Z)
{
	SIZE_T size = 0;

	if( !WriteProcessMemory(hProcess, (baseAddress + COMMENTATOR_POSITION_Z_8606), &Z, sizeof(float), &size) || size != sizeof(float) )
		return false;

	return true;
}

// Retrieves the commentator camera's XYZ position
Vec3 Player::GetCommentatorCameraPosition()
{
	Vec3 pos;
	pos.X = GetCommentatorCameraPosX();
	pos.Y = GetCommentatorCameraPosY();
	pos.Z = GetCommentatorCameraPosZ();

	return pos;
}

// Sets the commentator camera XYZ position
// Returns true on success
bool Player::SetCommentatorCameraPosition(float X, float Y, float Z)
{
	if( !SetCommentatorCameraPosX(X) || !SetCommentatorCameraPosY(Y) || !SetCommentatorCameraPosZ(Z) )
		return false;
	return true;
}

// Sets the commentator camera XYZ position
// Returns true on success
bool Player::SetCommentatorCameraPosition(Vec3 pos)
{
	if( !SetCommentatorCameraPosX(pos.X) || !SetCommentatorCameraPosY(pos.Y) || !SetCommentatorCameraPosZ(pos.Z) )
		return false;
	return true;
}

// Sets the commentator camera speed multiplier
// Returns true on success
bool Player::SetCommentatorCameraSpeed(float speed)
{
	SIZE_T size = 0;
	if( !WriteProcessMemory(hProcess, (baseAddress + COMMENTATOR_SPEED_8606), &speed, sizeof(float), &size) || size != sizeof(float) )
		return false;
	return true;
}

// Returns the speed of the camera in commentator mode
float Player::GetCommentatorCameraSpeed()
{
	SIZE_T size;
	float speed = 0.0f;

	if( !ReadProcessMemory(hProcess, (baseAddress + COMMENTATOR_SPEED_8606), &speed, sizeof(float), &size) )
		return 0.0f;

	return speed;
}

// Sets whether or not the commentator camera will collide with terrian
// Returns true on success
bool Player::SetCommentatorCameraCollision(bool bEnable)
{
	SIZE_T size = 0;
	if( !WriteProcessMemory(hProcess, (baseAddress + COMMENTATOR_COLLISION_8606), &bEnable, sizeof(bool), &size) || size != sizeof(bool) )
		return false;
	return true;
}

// Returns whether or not the commentator camera will collide with terrian
bool Player::IsCommentatorCameraCollidable()
{
	SIZE_T size;
	bool bCollision = false;

	if( !ReadProcessMemory(hProcess, (baseAddress + COMMENTATOR_COLLISION_8606), &bCollision, sizeof(bool), &size) || size != sizeof(bool) )
		return false;

	return bCollision;
}

// Returns a XYZO vector of the players coordinates
Vec4 Player::GetPosition()
{
	// Grab the player X,Y,Z,O coordinates
	Vec4 pos = { NULL };
	pos.X = GetPosX();
	pos.Y = GetPosY();
	pos.Z = GetPosZ();
	pos.O = GetPosO();

	return pos;
}

// Returns the players X coordinate
float Player::GetPosX()
{
	PBYTE MovementInfo = GetPlayerMovementInfoBase();
	if( MovementInfo == NULL )
		return false;

	float pX = 0.0f;
	SIZE_T size = 0;

	// Read the current position of the players X coordinate
	ReadProcessMemory(hProcess, (MovementInfo + PLAYER_POSITION_X_OFFSET_8606), &pX, sizeof(float), &size);

	return pX;
}

// Returns the players Y coordinate
float Player::GetPosY()
{
	PBYTE MovementInfo = GetPlayerMovementInfoBase();
	if( MovementInfo == NULL )
		return false;

	float pY = 0.0f;
	SIZE_T size = 0;

	// Read the current position of the players Y coordinate
	ReadProcessMemory(hProcess, (MovementInfo + PLAYER_POSITION_Y_OFFSET_8606), &pY, sizeof(float), &size);

	return pY;
}

// Returns the players Z coordinate
float Player::GetPosZ()
{
	PBYTE MovementInfo = GetPlayerMovementInfoBase();
	if( MovementInfo == NULL )
		return false;

	float pZ = 0.0f;
	SIZE_T size = 0;

	// Read the current position of the players Z coordinate
	ReadProcessMemory(hProcess, (MovementInfo + PLAYER_POSITION_Z_OFFSET_8606), &pZ, sizeof(float), &size);

	return pZ;
}

// Returns the players orientation (Tau)
float Player::GetPosO()
{
	PBYTE MovementInfo = GetPlayerMovementInfoBase();
	if( MovementInfo == NULL )
		return false;

	float pO = 0.0f;
	SIZE_T size = 0;

	// Read the current position of the players orientation
	ReadProcessMemory(hProcess, (MovementInfo + PLAYER_POSITION_O_OFFSET_8606), &pO, sizeof(float), &size);

	return pO;
}

// Sets the XYZO coordinates of the player using a vector
// Returns true on success
bool Player::SetPosition(Vec4 pos)
{
	// Set the current position of the player
	return (SetPosX(pos.X) && SetPosY(pos.Y) && SetPosZ(pos.Z) && SetPosO(pos.O));
}

// Sets the XYZ coordinates of the player using a vector
// Returns true on success
bool Player::SetPosition(Vec3 pos)
{
	// Set the current position of the player
	return (SetPosX(pos.X) && SetPosY(pos.Y) && SetPosZ(pos.Z));
}

// Sets the XYZO coordinates of the player
// Returns true on success
bool Player::SetPosition(float X, float Y, float Z, float O)
{
	Vec4 pos = { X, Y, Z, O };
	return SetPosition(pos);
}

// Set the Player's X coordinate
// Returns true on success
bool Player::SetPosX(float newX)
{
	SIZE_T size = 0;
	PBYTE MovementInfo = GetPlayerMovementInfoBase();
	if( MovementInfo == NULL )
		return false;

	// Set the current position of the players X coordinate
	if( !WriteProcessMemory(hProcess, (MovementInfo + PLAYER_POSITION_X_OFFSET_8606), &newX, sizeof(float), &size) || size != sizeof(float) )
		return false;

	return true;
}

// Set the Player's Y coordinate
// Returns true on success
bool Player::SetPosY(float newY)
{
	SIZE_T size = 0;
	PBYTE MovementInfo = GetPlayerMovementInfoBase();
	if( MovementInfo == NULL )
		return false;

	// Set the current position of the players Y coordinate
	if( !WriteProcessMemory(hProcess, (MovementInfo + PLAYER_POSITION_Y_OFFSET_8606), &newY, sizeof(float), &size) || size != sizeof(float) )
		return false;

	return true;
}

// Set the Player's X coordinate
// Returns true on success
bool Player::SetPosZ(float newZ)
{
	SIZE_T size = 0;
	PBYTE MovementInfo = GetPlayerMovementInfoBase();
	if( MovementInfo == NULL )
		return false;

	// Set the current position of the players Z coordinate
	if( !WriteProcessMemory(hProcess, (MovementInfo + PLAYER_POSITION_Z_OFFSET_8606), &newZ, sizeof(float), &size) || size != sizeof(float) )
		return false;

	return true;
}

// Set the Player's orientation (Tau)
// Returns true on success
bool Player::SetPosO(float newO)
{
	SIZE_T size = 0;
	PBYTE MovementInfo = GetPlayerMovementInfoBase();
	if( MovementInfo == NULL )
		return false;

	// Set the current position of the players orientation
	if( !WriteProcessMemory(hProcess, (MovementInfo + PLAYER_POSITION_O_OFFSET_8606), &newO, sizeof(float), &size) || size != sizeof(float) )
		return false;

	return true;
}
