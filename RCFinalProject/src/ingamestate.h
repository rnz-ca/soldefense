#pragma once

/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "boss.h"
#include "doublelinkedlist.h"
#include "enemyformation.h"
#include "explosion.h"
#include "gamestate.h"
#include "projectile.h"
#include "playership.h"
#include "preproc.h"
#if SOUND_ENABLED
#include "sound.h"
#endif
#include "starfield.h"
#include <string>

class CIngameState : public CGameState
{
public:
	enum class EState : int
	{
		UNASSIGNED,
		PLAYING,
		PLAYER_DEATH_COOLDOWN
	};

	enum class EMessageState : int
	{
		UNASSIGNED,
		NONE,
		GET_READY,
		GAME_OVER,
		MISSION_SUCCESSFUL
	};

	void Init();
	void CleanUp();

	void HandleKeyDownInput(SDL_KeyboardEvent* kbEvent);
	void HandleKeyUpInput(SDL_KeyboardEvent* kbEvent);

	void InitValues();
#if SOUND_ENABLED
	void InitSounds();
#endif
	void InitPlayer();
	void InitEnemies();
	void InitProjectiles();
	void InitExplosions();
	void InitText();

#if SOUND_ENABLED
	void DestroySounds();
#endif
	void DestroyPlayer();
	void DestroyEnemies();
	void DestroyProjectiles();
	void DestroyText();

	void Update(Uint32 elapsedTime);
	void UpdateEnemies(Uint32 elapsedTime);
	void UpdateProjectiles(Uint32 elapsedTime);
	void UpdateExplosions(Uint32 elapsedTime);
	void UpdateText(Uint32 elapsedTime);

	void Draw();
	void DrawEnemies();
	void DrawProjectiles();
	void DrawExplosions();
	void DrawText();
	
	void SpawnProjectile(CProjectile::EProjectileOwner, CProjectile::EProjectileType projectileType, float x, float y);
	void SpawnExplosion(CEntity::EEntityType entityType, float x, float y);

	CPlayerShip* GetPlayerShip() { return &m_playerShip; }
	CBoss* GetBoss() { return &m_boss; }
	CStarfield* GetStarfield() { return &m_starfield; }
	EState GetState() { return m_currentState; }

	void OnPlayerDeath();
	void OnEnemyFormationPositionRestarted();
	void OnBossLeave();
	void OnAllEnemiesDead();

	void SpawnBoss(CBoss::EBossType bossType);

	bool IsBossNullifyingPlayerShield() { return m_boss.IsNullifyingPlayerShield(); }
	bool IsBossMakingEnemiesShootDiagonally() { return m_boss.IsMakingEnemiesShootDiagonally(); }
	bool CanSpawnBoss();

	float GetPlayerShipPosX() { return m_playerShip.GetPosX(); }
	float GetPlayerShipPosY() { return m_playerShip.GetPosY(); }

	float GetCurrentDifficultyMultiplier();

#if DEBUG_DRAW
	void DrawDebug() override;
#endif

private:
	const EState INITIAL_STATE = EState::PLAYING;
	const EMessageState INITIAL_MESSAGE_STATE = EMessageState::GET_READY;

	// gameplay
	const int GAMEPLAY_STARTING_LIVES = 3;
	const int GAMEPLAY_INITIAL_LEVEL = 1;

	const int LABEL_BOSS_EFFECT_POSX_OFFSET = 30;
	const int LABELS_POSY = 20;
	const SDL_Color LABELS_COLOR{ 255, 255, 0, 255 };

	// score
	const std::string LABEL_SCORE_TEXT = "SCORE:";
	const int LABEL_SCORE_VALUE_POSX = LABEL_BOSS_EFFECT_POSX_OFFSET + 150;

	// level
	const int LABEL_LEVEL_POSX = LABEL_BOSS_EFFECT_POSX_OFFSET + 350;
	const std::string LABEL_LEVEL_TEXT = "LEVEL:";
	const int LABEL_LEVEL_VALUE_POSX = LABEL_BOSS_EFFECT_POSX_OFFSET + 500;

	// lives
	const int LABEL_LIVES_POSX = LABEL_BOSS_EFFECT_POSX_OFFSET + 650;
	const std::string LABEL_LIVES_TEXT = "SHIPS:";
	const int LABEL_LIVES_VALUE_POSX = LABEL_BOSS_EFFECT_POSX_OFFSET + 800;

	// boss effect
	const std::string LABEL_SHIELD_NULLIFIED_TEXT = "SHIELD NULLIFIED!";
	const std::string LABEL_ENEMIES_ENHANCED_TEXT = "ENEMIES ENHANCED!";
	const SDL_Color LABEL_SHIELD_NULLIFIED_COLOR{ 255, 0, 0, 255 };
	const SDL_Color LABEL_ENEMIES_ENHANCED_COLOR{ 255, 0, 0, 255 };

	// boss spawn rules
	const Uint32 BOSS_SPAWN_INTERVAL_MS = 12000; // when does the next boss spawn after game start or last boss' death
	const Uint32 BOSS_SPAWN_MINIMUM_ENEMIES = 8; // boss can spawn if there are this amount of enemies or more

	// explosion
	const Uint32 PLAYER_EXPLOSION_LIFETIME_MS = 1654;
	const Uint32 ENEMY_EXPLOSION_LIFETIME_MS = 580;

	// textures
	const std::string TEXTURE_PLAYERSHIP_SPRITESHEET_FILENAME = "sip_ship_spritesheet.png";
	const std::string TEXTURE_ENEMY_SPRITESHEET_FILENAME = "sip_enemy_spritesheet.png";
	const std::string TEXTURE_PROJECTILES_SPRITESHEET_FILENAME = "sip_projectiles_spritesheet.png";

	// sounds
	const std::string SOUND_MUSIC_FILENAME = "mus-ingame.mod";
	const float SOUND_MUSIC_VOLUME = 0.2f;

	const std::string SOUND_PLAYER_PROJECTILE_FILENAME = "player-attack3.wav";
	const std::string SOUND_PLAYER_SHIELD_FILENAME = "player-shield.wav";
	const std::string SOUND_PLAYER_DEATH_FILENAME = "player-death.wav";
	const std::string SOUND_PLAYER_EXPLOSION_FILENAME = "player-explosion.wav";
	const std::string SOUND_PLAYER_SHIELD_NULLIFIED_FILENAME = "player-shield-nullified.wav";
	
	const std::string SOUND_ENEMY_PROJECTILE_FILENAME = "enemy-attack.wav";
	const std::string SOUND_ENEMY_EXPLOSION_FILENAME = "enemy-explosion.wav";
	const std::string SOUND_BOSS_SPAWN_FILENAME = "boss-spawn.wav";
	const std::string SOUND_BOSS_NULLIFY_FILENAME = "boss-nullify.wav";
	const std::string SOUND_BOSS_ENHANCE_FILENAME = "boss-enhance.wav";

	// messages
	const std::string MESSAGE_GET_READY_TEXT = "GET READY!";
	const SDL_Color MESSAGE_GET_READY_COLOR{ 255, 255, 0, 255 };
	const Uint32 MESSAGE_GET_READY_DURATION_MS = 1000;

	const std::string MESSAGE_SUCCESS_TEXT = "MISSION SUCCESFUL, GET READY!";
	const SDL_Color MESSAGE_SUCCESS_COLOR{ 0, 255, 0, 255 };
	const Uint32 MESSAGE_SUCCESS_DURATION_MS = 1000;

	const std::string MESSAGE_GAMEOVER_TEXT = "GAME OVER";
	const SDL_Color MESSAGE_GAMEOVER_COLOR{ 255, 0, 0, 255 };
	const Uint32 MESSAGE_GAMEOVER_DURATION_MS = 5000;

	void RequestState(EState state);
	void RequestMessageState(EMessageState state);

	void DrawMessage(CTexture* texture);
	Uint32 GetCurrentMessageDuration();

	void ReturnToIntroState();

	bool IsLevelComplete();
	void HandleLevelCompletion();

	CStarfield m_starfield;
	CPlayerShip m_playerShip;
	CEnemyFormation m_enemyFormation;
	CDoubleLinkedList<CProjectile*> m_projectilesList;
	CDoubleLinkedList<CExplosion*> m_explosionsList;
	CBoss m_boss;

	CTexture m_playerShipSheetTexture;
	CTexture m_enemySpriteSheetTexture;
	CTexture m_projectilesSheetTexture;
	CTexture m_scoreLabelTexture;
	CTexture m_scoreValueTexture;
	CTexture m_levelLabelTexture;
	CTexture m_levelValueTexture;
	CTexture m_livesLabelTexture;
	CTexture m_livesValueTexture;
	CTexture m_shieldNullifiedLabelTexture;
	CTexture m_enemiesEnhancedLabelTexture;
	CTexture m_getReadyMessageTexture;
	CTexture m_successMessageTexture;
	CTexture m_gameOverMessageTexture;

#if SOUND_ENABLED
	CSound m_music;
	CSound m_playerShootSound;
	CSound m_playerShieldSound;
	CSound m_playerExplosionSound;
	CSound m_playerShieldNullifiedSound;
	CSound m_enemyExplosionSound;
	CSound m_enemyAttackSound;
	CSound m_bossSpawnSound;
	CSound m_bossNullifySound;
	CSound m_bossEnhanceSound;
#endif

	EState m_currentState = EState::UNASSIGNED;
	EState m_requestedState = EState::UNASSIGNED;
	EMessageState m_currentMessageState = EMessageState::UNASSIGNED;
	EMessageState m_requestedMessageState = EMessageState::UNASSIGNED;

	Uint32 m_lastBossSpawnTicks = 0;
	Uint32 m_lastMessageDisplayTicks = 0;

	int m_score = 0;
	int m_previousScore = -1;
	int m_level = 0;
	int m_previousLevel = -1;
	int m_lives = 0;
	int m_previousLives = -1;

	float m_difficultyMultiplier = 1.0f;
};