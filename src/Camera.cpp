#include "PreCompiled.h"

// Returns the Base address of the camera block in memory - INTERNAL
PBYTE Camera::GetCameraBase()
{
	PBYTE cam = NULL;
	SIZE_T size = 0;

	// Retrieve the base address of the camera class from the game's memory.
	if( !ReadProcessMemory(hProcess, (baseAddress + CAMERA_BASE_PTR_8606), &cam, sizeof(PBYTE), &size) )
		return NULL;
	if( !ReadProcessMemory(hProcess, (cam + CAMERA_CLASS_OFFSET_8606), &cam, sizeof(PBYTE), &size) )
		return NULL;

	return cam;
}

// Returns the Cameras field of view in degrees
float Camera::GetFieldOfView()
{
	PBYTE Cam = GetCameraBase();
	SIZE_T size = 0;
	float fov = 0.0f;

	if( !Cam )
		return 0.0f;

	// Read 0x40 bytes past the camera base to retrieve the field of view.
	ReadProcessMemory(hProcess, (Cam + CAMERA_FOV_OFFSET_8606), &fov, sizeof(float), &size);

	// return the result in degrees, not radians.
	return (fov * (180.0f/(float)M_PI));
}

// Sets the new field of view (passed float is degrees)
// Return true on success
bool Camera::SetFieldOfView(float newFov)
{
	PBYTE Cam = GetCameraBase();
	SIZE_T size = 0;

	if( !Cam )
		return false;

	// Convert the field of view from degrees to radians
	if( newFov < 0.1f )
		newFov = 0.1f;
	else if( newFov > 180.0f )
		newFov = 180.0f;
	newFov = newFov * ((float)M_PI/180.0f);

	// Set the new field of view
	if( !WriteProcessMemory(hProcess, (Cam + CAMERA_FOV_OFFSET_8606), &newFov, sizeof(float), &size) || size != sizeof(float) )
		return false;

	return true;
}

// Returns the camera roll in degrees
float Camera::GetRoll()
{
	PBYTE Cam = GetCameraBase();
	SIZE_T size = 0;
	float roll = 0.0f;

	if( !Cam )
		return 0.0f;

	// Read 10C bytes past the camera base to retrieve the camera roll.
	if( !ReadProcessMemory(hProcess, (Cam + CAMERA_ROLL_OFFSET_8606), &roll, sizeof(float), &size) )
		return 0.0f;

	// Return the result in degrees, not radians
	return (roll * (360.0f/(float)(M_PI*2)));
}

// Sets the camera roll (passed float is in degrees)
// Returns true on success
bool Camera::SetRoll(float newRoll)
{
	PBYTE Cam = GetCameraBase();
	SIZE_T size = 0;

	if( !Cam )
		return false;

	while( newRoll > 360.0f )
		newRoll -= 360.0f;

	// Convert the roll from degrees to radians
	newRoll = newRoll * ((float)(M_PI*2)/360.0f);

	// Set the new field of view
	if( !WriteProcessMemory(hProcess, (Cam + CAMERA_ROLL_OFFSET_8606), &newRoll, sizeof(float), &size) || size != sizeof(float) )
		return false;

	return true;
}