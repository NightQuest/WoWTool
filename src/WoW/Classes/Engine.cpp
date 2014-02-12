#include "PreCompiled.h"

// Sets the speed of animations
// Default: 1000
// Returns true on success
bool Engine::SetAnimationSpeed(float speed)
{
	SIZE_T size;
	DWORD oldProtect;
	bool ret = false;

	if( VirtualProtectEx(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_NEW_12340), sizeof(float), PAGE_READWRITE, &oldProtect) != 0 )
	{
		ret = (WriteProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_NEW_12340), &speed, sizeof(float), &size) && size == sizeof(float));
		VirtualProtectEx(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_NEW_12340), sizeof(float), oldProtect, &oldProtect);
	}
	return ret;
}

// Sets the game speed of the game (speed of everything from time, animations, etc)
// Default: 0.00100000004749
// Returns true on success
bool Engine::SetGameSpeed(double speed)
{
	SIZE_T size;
	return (WriteProcessMemory(hProcess, (baseAddress + ENGINE_GAME_SPEED_8606), &speed, sizeof(double), &size) && size == sizeof(double));
}

// Returns the speed of animations
// Default: 1000
float Engine::GetAnimationSpeed()
{
	SIZE_T size = 0;
	float speed = 0.0f;

	if( !ReadProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_NEW_12340), &speed, sizeof(float), &size) || size != sizeof(float) )
		return 0.0f;

	return speed;
}

// Returns the speed of the game (speed of everything from time, animations, etc)
// Default: 0.00100000004749
double Engine::GetGameSpeed()
{
	SIZE_T size;
	double speed = 0.0f;

	if( !ReadProcessMemory(hProcess, (baseAddress + ENGINE_GAME_SPEED_8606), &speed, sizeof(double), &size) || size != sizeof(double) )
		return 0.0f;

	return speed;
}

// Checks to see if client has fully loaded into the game environment
// Returns true if it has, false if not.
bool Engine::IsFullyLoadedInGame()
{
	bool inGame = false;
	SIZE_T size = 0;

	if( !ReadProcessMemory(hProcess, (baseAddress + ENGINE_IS_FULLY_LOADED_INGAME), &inGame, sizeof(bool), &size) || size != sizeof(bool) )
		return false;

	return inGame;
}

// Returns the bitmask that the game uses to decide what and how to render the scene
// Default: 0x1F104F73
DWORD Engine::GetRenderingFlags()
{
	DWORD bitmask = 0;
	SIZE_T size = 0;

	if( !ReadProcessMemory(hProcess, (baseAddress + ENGINE_RENDERING_FLAGS_12340), &bitmask, sizeof(DWORD), &size) || size != sizeof(DWORD) )
		return NULL;

	return bitmask;
}

// Checks to see if the passed bitmask exists in the rendering flags bitmask.
// Returns true on success.
bool Engine::HasRenderingFlags(DWORD flags) { return ((GetRenderingFlags() & flags) == flags); }

// Sets the bitmask that the game uses to decide what and how to render the scene
// Returns true on success
bool Engine::SetRenderingFlags(DWORD flags)
{
	DWORD RenderingFlags = NULL;
	SIZE_T size = 0;

	// Read the rendering flags and do a bitwise OR on them, adding the passed flags to them
	RenderingFlags = GetRenderingFlags() | flags;

	// Write the new flags back to the rendering flags in memory
	if( !WriteProcessMemory(hProcess, (baseAddress + ENGINE_RENDERING_FLAGS_12340), &RenderingFlags, sizeof(DWORD), &size) || size != sizeof(DWORD) )
		return false;

	return true;
}


// Does a bitwise inverse OR on the rendering flags, removing whatever is passed.
// Returns true on success.
bool Engine::RemoveRenderingFlags(DWORD flags)
{
	DWORD RenderingFlags = NULL;
	SIZE_T size = 0;

	// Read the rendering flags and do a bitwise inverse AND on them, removing the passed flags from them
	RenderingFlags = GetRenderingFlags() & ~flags;

	// Write the new flags back to the rendering flags in memory
	if( !WriteProcessMemory(hProcess, (baseAddress + ENGINE_RENDERING_FLAGS_12340), &RenderingFlags, sizeof(DWORD), &size) || size != sizeof(DWORD) )
		return false;

	return true;
}

// Returns the current rotation of the sky.
float Engine::GetSkyPosition()
{
	float position = 0.0f;
	SIZE_T size = 0;

	if( !ReadProcessMemory(hProcess, (baseAddress + ENGINE_SKY_POSITION_12340), &position, sizeof(float), &size) || size != sizeof(float) )
		return 0.0f;

	return position;
}

// Sets the current rotation of the sky.
// Returns true on success
bool Engine::SetSkyPosition(float position)
{
	SIZE_T size = 0;

	// Write the new position into memory
	if( !WriteProcessMemory(hProcess, (baseAddress + ENGINE_SKY_POSITION_12340), &position, sizeof(float), &size) || size != sizeof(float) )
		return false;

	return true;
}

// Returns the opacity of the night sky.
BYTE Engine::GetNightSkyOpacity()
{
	BYTE opacity = 0;
	SIZE_T size = 0;

	if( !ReadProcessMemory(hProcess, (baseAddress + ENGINE_NIGHT_SKY_OPACITY_12340), &opacity, sizeof(BYTE), &size) || size != sizeof(BYTE) )
		return 0;

	return opacity;
}

// Sets the opacity of the night sky.
// Returns true on success
bool Engine::SetNightSkyOpacity(BYTE opacity)
{
	SIZE_T size = 0;

	// Minimum value is 1
	if( opacity == 0 )
		opacity = 1;

	// Write the new position into memory
	if( !WriteProcessMemory(hProcess, (baseAddress + ENGINE_NIGHT_SKY_OPACITY_12340), &opacity, sizeof(BYTE), &size) || size != sizeof(BYTE) )
		return false;

	return true;
}

// Sets whether or not to draw the sky.
// Return true on success.
bool Engine::DrawSky(bool bDrawSky)
{
	SIZE_T size = 0;
	BYTE drawSky = bDrawSky ? 1 : 0;

	// Write whether or not to draw the sky into memory
	if( !WriteProcessMemory(hProcess, (baseAddress + ENGINE_DRAW_SKY), &drawSky, sizeof(BYTE), &size) || size != sizeof(BYTE) )
		return false;

	return true;
}

// Sets whether or not to draw clouds in the sky.
// Return true on success
bool Engine::DrawClouds(bool bDrawClouds)
{
	SIZE_T size = 0;
	BYTE drawClouds = bDrawClouds ? 1 : 0;

	// Write whether or not to draw the clouds into memory
	if( !WriteProcessMemory(hProcess, (baseAddress + ENGINE_DRAW_CLOUDS), &drawClouds, sizeof(BYTE), &size) || size != sizeof(BYTE) )
		return false;

	return true;
}