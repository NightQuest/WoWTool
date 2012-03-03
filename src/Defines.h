#pragma once

struct Vec3
{
	float X;
	float Y;
	float Z;
};
struct Vec4
{
	float X;
	float Y;
	float Z;
	float O;
};

// Game versions
#define FINAL_TBC										8606
#define FINAL_WOTLK										12340

// Engine
#define ENGINE_SPEED_OF_ANIMATION_8606					0x0048F750
#define ENGINE_GAME_SPEED_8606							0x0048F608
#define ENGINE_RENDERING_FLAGS_8606						0x009A3510

// Player offsets
#define PLAYER_BASE_PTR_8606							0x00A28D28
#define PLAYER_FIELDS_OFFSET_8606						0x00001190
#define PLAYER_DUEL_ARBITER_8606						0x00000000
#define PLAYER_FLAG_8606								0x00000008

#define PLAYER_POSITION_X_OFFSET_8606					0x00000BF0
#define PLAYER_POSITION_Y_OFFSET_8606					0x00000BF4
#define PLAYER_POSITION_Z_OFFSET_8606					0x00000BF8
#define PLAYER_POSITION_O_OFFSET_8606					0x00000BFC

#define PLAYER_COMMENTATOR_X_8606						0x008EE474
#define PLAYER_COMMENTATOR_Y_8606						0x008EE478
#define PLAYER_COMMENTATOR_Z_8606						0x008EE47C
#define PLAYER_COMMENTATOR_AUTOPAN_X_8606				0x008EE480
#define PLAYER_COMMENTATOR_AUTOPAN_Y_8606				0x008EE484
#define PLAYER_COMMENTATOR_AUTOPAN_Z_8606				0x008EE488
#define PLAYER_COMMENTATOR_GUID_CHASECAM_STEADY_8606	0x008EE48C
#define PLAYER_COMMENTATOR_GUID_CHASECAM_RELATIVE_8606	0x008EE494
#define PLAYER_COMMENTATOR_CAMERA_ZOOM_8606				0x008EE4A4
#define PLAYER_COMMENTATOR_CAMERA_ZOOM2_8606			0x008EE4A8
#define PLAYER_COMMENTATOR_SPEED_8606					0x008EE4AC
#define PLAYER_COMMENTATOR_COLLISION_8606				0x008EE4B0

// Camera offsets - 8606
#define CAMERA_BASE_PTR_8606							0x0086DCCC
#define CAMERA_CLASS_OFFSET_8606						0x0000732C
#define CAMERA_POSITION_X_OFFSET_8606					0x00000008
#define CAMERA_POSITION_Y_OFFSET_8606					0x0000000C
#define CAMERA_POSITION_Z_OFFSET_8606					0x00000010
#define CAMERA_FOV_OFFSET_8606							0x00000040
#define CAMERA_ZOOM_OFFSET_8606							0x00000100
#define CAMERA_YAW_OFFSET_8606							0x00000104
#define CAMERA_PITCH_OFFSET_8606						0x00000108
#define CAMERA_ROLL_OFFSET_8606							0x0000010C

// Camera offsets - 12340
#define CAMERA_BASE_PTR_12340							0x0077336C
#define CAMERA_CLASS_OFFSET_12340						0x00007E20
#define CAMERA_POSITION_X_OFFSET_12340					0x00000008
#define CAMERA_POSITION_Y_OFFSET_12340					0x0000000C
#define CAMERA_POSITION_Z_OFFSET_12340					0x00000010
#define CAMERA_FOV_OFFSET_12340							0x00000040
#define CAMERA_ZOOM_OFFSET_12340						0x00000118
#define CAMERA_ROLL_OFFSET_12340						0x00000124

// Rendering flags
enum RenderingFlags
{
	RENDER_FLAG_M2										= 0x00000001, // 00
	RENDER_FLAG_TERRAIN									= 0x00000002, // 01
	RENDER_FLAG_UNK01									= 0x00000004, // 02
	RENDER_FLAG_UNK02									= 0x00000008, // 03
	RENDER_FLAG_UNK03									= 0x00000010, // 04
	RENDER_FLAG_UNK04									= 0x00000020, // 05
	RENDER_FLAG_UNK05									= 0x00000040, // 06
	RENDER_FLAG_UNK06									= 0x00000080, // 07
	RENDER_FLAG_WMO										= 0x00000100, // 08
	RENDER_FLAG_WMO_BAKED_SHADOWS						= 0x00000200, // 09
	RENDER_FLAG_UNK07									= 0x00000400, // 10
	RENDER_FLAG_WMO_INTERIOR_TEXTURES					= 0x00000800, // 11
	RENDER_FLAG_WMO_OPENING								= 0x00001000, // 12
	RENDER_FLAG_UNK08									= 0x00002000, // 13
	RENDER_FLAG_UNK09									= 0x00004000, // 14
	RENDER_FLAG_UNK10									= 0x00008000, // 15
	RENDER_FLAG_UNK11									= 0x00010000, // 16
	RENDER_FLAG_UNK12									= 0x00020000, // 17
	RENDER_FLAG_UNK13									= 0x00040000, // 18
	RENDER_FLAG_UNK14									= 0x00080000, // 19
	RENDER_FLAG_GROUND_DETAIL							= 0x00100000, // 20
	RENDER_FLAG_WMO_AND_TERRAIN_HIT_FACES				= 0x00200000, // 21
	RENDER_FLAG_UNK15									= 0x00400000, // 22
	RENDER_FLAG_UNK16									= 0x00800000, // 23
	RENDER_FLAG_UNK17									= 0x01000000, // 24
	RENDER_FLAG_UNK18									= 0x02000000, // 25
	RENDER_FLAG_UNK19									= 0x04000000, // 26
	RENDER_FLAG_UNK20									= 0x08000000, // 27
	RENDER_FLAG_UNK21									= 0x10000000, // 28
	RENDER_FLAG_WIREFRAME								= 0x20000000, // 29
	RENDER_FLAG_WMO_NORMALS								= 0x40000000, // 30
	RENDER_FLAG_UNK22									= 0x80000000, // 31
};

// Player flags
enum PlayerFlags
{
	PLAYER_FLAG_GROUP_LEADER							= 0x00000001, // 00 
	PLAYER_FLAG_AFK										= 0x00000002, // 01 
	PLAYER_FLAG_DND										= 0x00000004, // 02 
	PLAYER_FLAG_GM										= 0x00000008, // 03 
	PLAYER_FLAG_GHOST									= 0x00000010, // 04 
	PLAYER_FLAG_RESTING									= 0x00000020, // 05 
	PLAYER_FLAG_HIDE_HELM								= 0x00000400, // 10 
	PLAYER_FLAG_HIDE_CLOAK								= 0x00000800, // 11 
	PLAYER_FLAG_SANCTUARY								= 0x00010000, // 16 
	PLAYER_FLAG_TAXI_TIME_TEST							= 0x00020000, // 17 
	PLAYER_FLAG_PVP_TIMER								= 0x00040000, // 18 
	PLAYER_FLAG_COMMENTATOR								= 0x00080000, // 19 
	PLAYER_FLAG_CAN_USE_COMMENTATOR_COMMANDS			= 0x00400000, // 22 
};

// Player State
enum PlayerState
{
	PLAYER_STATE_TAXI_TIME_TEST							= 0x00100000, // 20
	PLAYER_STATE_MOUNTED								= 0x08000000, // 27
};

// Movement State
enum MovementState
{
	MOVEMENT_STATE_MOVE_FORWARD							= 0x00000100, // 08
	MOVEMENT_STATE_MOVE_BACKWARD						= 0x00000200, // 09
	MOVEMENT_STATE_MOVE_LEFT							= 0x00000400, // 10
	MOVEMENT_STATE_MOVE_RIGHT							= 0x00000800, // 11
	MOVEMENT_STATE_YAW_LEFT								= 0x00001000, // 12
	MOVEMENT_STATE_YAW_RIGHT							= 0x00002000, // 13
	MOVEMENT_STATE_PITCH_UP								= 0x00004000, // 14
	MOVEMENT_STATE_PITCH_DOWN							= 0x00008000, // 15
	MOVEMENT_STATE_WALK									= 0x00010000, // 16
	MOVEMENT_STATE_UNK									= 0x00020000, // 17
	MOVEMENT_STATE_FLYING								= 0x00040000, // 18
	MOVEMENT_STATE_DEAD									= 0x00080000, // 19
	MOVEMENT_STATE_IN_THE_AIR							= 0x00100000, // 20
	MOVEMENT_STATE_FALLING								= 0x00200000, // 21
	MOVEMENT_STATE_REBOUND_IN_AIR						= 0x00400000, // 22
	MOVEMENT_STATE_REBOUND_LANDING						= 0x00800000, // 23
	MOVEMENT_STATE_AUTO_MOVE_FORWARD					= 0x01000000, // 24
	MOVEMENT_STATE_AUTO_MOVE_BACKWARD					= 0x02000000, // 25
	MOVEMENT_STATE_AUTO_MOVE_LEFT						= 0x04000000, // 26
	MOVEMENT_STATE_AUTO_MOVE_RIGHT						= 0x08000000, // 27
	MOVEMENT_STATE_CANNOT_MOVE							= 0x10000000, // 28
	MOVEMENT_STATE_SWIMMING								= 0x20000000, // 29
	MOVEMENT_STATE_FLYING_UP							= 0x40000000, // 30
	MOVEMENT_STATE_SWIMMING_DOWN						= 0x80000000, // 31
};

// Movement Type
enum MovementType
{
	MOVEMENT_TYPE_ABLE_TO_FLY							= 0x00000001, // 00
	MOVEMENT_TYPE_FLYING								= 0x00000002, // 01
	MOVEMENT_TYPE_SKIMMING_OBJECT						= 0x00000004, // 02
	MOVEMENT_TYPE_QUEUE_MOVEMENT						= 0x00000008, // 03
	MOVEMENT_TYPE_WATERWALK								= 0x00000010, // 04
	MOVEMENT_TYPE_HOVER									= 0x00000040, // 05
	MOVEMENT_TYPE_NORMAL								= 0x00000080, // 06
};
