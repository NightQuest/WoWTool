#include "main.h"

// Returns the Base address of the camera block in memory - INTERNAL
PBYTE WoWCamera::GetCameraBase()
{
	PBYTE cam = NULL;
	SIZE_T size = 0;

	// Retrieve the base address of the camera class from the game's memory.
	if( !ReadProcessMemory(hProcess, (baseAddress + CAMERA_BASE_PTR), &cam, sizeof(PBYTE), &size) )
		return NULL;
	if( !ReadProcessMemory(hProcess, (cam + CAMERA_CLASS_OFFSET), &cam, sizeof(PBYTE), &size) )
		return NULL;

	return cam;
}

// Returns the Cameras field of view in degrees
float WoWCamera::GetFieldOfView()
{
	PBYTE Cam = GetCameraBase();
	SIZE_T size = 0;
	float fov = 0.0f;

	if( !Cam )
		return 0.0f;

	// Read 0x40 bytes past the camera base to retrieve the field of view.
	ReadProcessMemory(hProcess, (Cam + CAMERA_FOV_OFFSET), &fov, sizeof(float), &size);

	// return the result in degrees, not radians.
	return (fov * (180.0f/(float)M_PI));
}

bool WoWCamera::SetFieldOfView(float newFov)
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
	if( !WriteProcessMemory(hProcess, (Cam + CAMERA_FOV_OFFSET), &newFov, sizeof(float), &size) || size != sizeof(float) )
		return false;

	return true;
}