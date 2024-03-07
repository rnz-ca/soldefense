#pragma once

/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

//-------------------------------------------------------------------------------------------------
// APP INFO
//-------------------------------------------------------------------------------------------------

#define APP_NAME										"Sol Defense"
#define APP_VERSION										0.01
#define APP_AUTHOR										"RENZO CALDERON"
#define APP_ADDITIONAL_TEXT								"CM3037 FINAL PROJECT"

//-------------------------------------------------------------------------------------------------
// GRAPHICS SETTINGS
//-------------------------------------------------------------------------------------------------

#define GFX_SCREEN_WIDTH								1920
#define GFX_SCREEN_HEIGHT								1080
#define GFX_SCREEN_FULLSCREEN							1
#define GFX_DIRECTORY									"assets/gfx/"

//-------------------------------------------------------------------------------------------------
// SOUND SETTINGS
//-------------------------------------------------------------------------------------------------

#define SOUND_ENABLED									1
#if SOUND_ENABLED
#define SOUND_DEFAULT_VOLUME							1.0f
#define SOUND_DIRECTORY									"assets/sfx/"
#endif

//-------------------------------------------------------------------------------------------------
// MAIN GAMEPLAY
//-------------------------------------------------------------------------------------------------

#define COLLISIONS_ENABLED								1
#define SCALE_FACTOR									256.0f

// ENEMIES

#define ENEMY_MOVE_SPEED_X								8.0f
#define ENEMY_MOVE_SPEED_Y								8.0f
#define ENEMY_MOVE_LIMIT								60

//-------------------------------------------------------------------------------------------------
// DEBUG SETTINGS
//-------------------------------------------------------------------------------------------------

#define DEBUG_LOG_KEYDOWN								0
#define DEBUG_LOG_KEYUP									0
#define DEBUG_DRAW										0
#define DEBUG_DRAW_ENEMY_FORMATION_EDGES				0
#define DEBUG_ENABLE_HOTKEYS							0
