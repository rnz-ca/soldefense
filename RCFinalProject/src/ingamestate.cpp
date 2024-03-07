/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "ingamestate.h"

#include <assert.h>
#include "utils.h"
#include "entity.h"

void CIngameState::Init()
{
	InitValues();
#if SOUND_ENABLED
	InitSounds();
#endif	
	m_starfield.Init();
	InitPlayer();
	InitEnemies();
	InitProjectiles();
	InitText();

	m_isInitialized = true;

#if SOUND_ENABLED
	// start playing the music
	m_music.SetLoop(true);
	m_music.Play(SOUND_MUSIC_VOLUME);
#endif

	// spawn enemies
	m_enemyFormation.Spawn();

	RequestState(INITIAL_STATE);
	RequestMessageState(INITIAL_MESSAGE_STATE);
}

void CIngameState::CleanUp()
{
	if (IsInitialized())
	{
		DestroyText();
		DestroyProjectiles();
		DestroyEnemies();
		DestroyPlayer();
		m_starfield.Destroy();
#if SOUND_ENABLED
		DestroySounds();
#endif
	}
}

void CIngameState::RequestState(EState state)
{
	assert(m_currentState != state);
	m_requestedState = state;
}

void CIngameState::RequestMessageState(EMessageState state)
{
	assert(m_currentMessageState != state);
	m_requestedMessageState = state;
}

void CIngameState::HandleKeyDownInput(SDL_KeyboardEvent* kbEvent)
{
	if (kbEvent->repeat != 0)
	{
		return;
	}

	switch (kbEvent->keysym.scancode)
	{
	case SDL_SCANCODE_UP:
#if DEBUG_LOG_KEYDOWN
		LOG_SCR("Up key keydown");
#endif
		m_playerShip.SetInputUp(1);
		break;

	case SDL_SCANCODE_DOWN:
#if DEBUG_LOG_KEYDOWN
		LOG_SCR("Down key keydown");
#endif
		m_playerShip.SetInputDown(1);
		break;

	case SDL_SCANCODE_LEFT:
#if DEBUG_LOG_KEYDOWN
		LOG_SCR("Left key keydown");
#endif
		m_playerShip.SetInputLeft(1);
		break;

	case SDL_SCANCODE_RIGHT:
#if DEBUG_LOG_KEYDOWN
		LOG_SCR("Right key keydown");
#endif
		m_playerShip.SetInputRight(1);
		break;

	case SDL_SCANCODE_LCTRL:
#if DEBUG_LOG_KEYDOWN
		LOG_SCR("Left-Control key keydown");
#endif
		m_playerShip.SetInputFire(1);
		break;

	case SDL_SCANCODE_LALT:
#if DEBUG_LOG_KEYDOWN
		LOG_SCR("Left-Alt key keydown");
#endif
		m_playerShip.SetInputShield(1);
		break;

#if DEBUG_ENABLE_HOTKEYS
	case SDL_SCANCODE_B:
		SpawnBoss(CBoss::EBossType::SAUCER);
		break;

	case SDL_SCANCODE_N:
		SpawnBoss(CBoss::EBossType::SPIDER);
		break;

	case SDL_SCANCODE_M:
		SpawnBoss(CBoss::EBossType::WALKER);
		break;
#endif
	}
}

void CIngameState::HandleKeyUpInput(SDL_KeyboardEvent* kbEvent)
{
	if (kbEvent->repeat != 0)
	{
		return;
	}

	switch (kbEvent->keysym.scancode)
	{
	case SDL_SCANCODE_UP:
#if DEBUG_LOG_UP
		LOG_SCR("Up key keyup");
#endif
		m_playerShip.SetInputUp(0);
		break;

	case SDL_SCANCODE_DOWN:
#if DEBUG_LOG_UP
		LOG_SCR("Down key keyup");
#endif
		m_playerShip.SetInputDown(0);
		break;

	case SDL_SCANCODE_LEFT:
#if DEBUG_LOG_UP
		LOG_SCR("Left key keyup");
#endif
		m_playerShip.SetInputLeft(0);
		break;

	case SDL_SCANCODE_RIGHT:
#if DEBUG_LOG_UP
		LOG_SCR("Right key keyup");
#endif
		m_playerShip.SetInputRight(0);
		break;

	case SDL_SCANCODE_LCTRL:
#if DEBUG_LOG_UP
		LOG_SCR("Left-Control key keyup");
#endif
		m_playerShip.SetInputFire(0);
		break;

	case SDL_SCANCODE_LALT:
#if DEBUG_LOG_UP
		LOG_SCR("Left-Alt key keyup");
#endif
		m_playerShip.SetInputShield(0);
		break;

	case SDL_SCANCODE_ESCAPE:
		// request gamemanager to switch state
		ReturnToIntroState();
		break;
	}	
}

void CIngameState::InitValues()
{
	SetIsInitialized(false);

	m_score = 0;
	m_previousScore = -1;
	m_level = GAMEPLAY_INITIAL_LEVEL;
	m_previousLevel = -1;
	m_lives = GAMEPLAY_STARTING_LIVES;
	m_previousLives = -1;
}

#if SOUND_ENABLED
void CIngameState::InitSounds()
{
	m_music.CreateFromFile(SOUND_MUSIC_FILENAME);
	m_playerShootSound.CreateFromFile(SOUND_PLAYER_PROJECTILE_FILENAME);
	m_playerShieldSound.CreateFromFile(SOUND_PLAYER_SHIELD_FILENAME);
	m_playerExplosionSound.CreateFromFile(SOUND_PLAYER_EXPLOSION_FILENAME);
	m_playerShieldNullifiedSound.CreateFromFile(SOUND_PLAYER_SHIELD_NULLIFIED_FILENAME);
	m_enemyExplosionSound.CreateFromFile(SOUND_ENEMY_EXPLOSION_FILENAME);
	m_enemyAttackSound.CreateFromFile(SOUND_ENEMY_PROJECTILE_FILENAME);
	m_bossSpawnSound.CreateFromFile(SOUND_BOSS_SPAWN_FILENAME);
	m_bossNullifySound.CreateFromFile(SOUND_BOSS_NULLIFY_FILENAME);
	m_bossEnhanceSound.CreateFromFile(SOUND_BOSS_ENHANCE_FILENAME);
}

void CIngameState::DestroySounds()
{
	m_bossEnhanceSound.Destroy();
	m_bossNullifySound.Destroy();
	m_bossSpawnSound.Destroy();
	m_enemyAttackSound.Destroy();
	m_enemyExplosionSound.Destroy();
	m_playerShieldNullifiedSound.Destroy();
	m_playerExplosionSound.Destroy();
	m_playerShieldSound.Destroy();
	m_playerShootSound.Destroy();
	m_music.Destroy();
}
#endif

void CIngameState::Update(Uint32 elapsedTime)
{
	// handle state requests
	if (m_requestedState != EState::UNASSIGNED)
	{
		m_currentState = m_requestedState;
		m_requestedState = EState::UNASSIGNED;
	}
	if (m_requestedMessageState != EMessageState::UNASSIGNED)
	{
		m_currentMessageState = m_requestedMessageState;
		m_requestedMessageState = EMessageState::UNASSIGNED;
		m_lastMessageDisplayTicks = Utils::GetTicks();
	}

	// update game loop
	if (m_currentState == EState::PLAYING || m_currentState == EState::PLAYER_DEATH_COOLDOWN)
	{
		m_starfield.Update(elapsedTime);
		m_playerShip.Update(elapsedTime);
		UpdateEnemies(elapsedTime);
		UpdateProjectiles(elapsedTime);
		UpdateExplosions(elapsedTime);
		UpdateText(elapsedTime);

		if (m_currentState == EState::PLAYER_DEATH_COOLDOWN && !m_starfield.IsPlayingAnimation())
		{
			// respawn the player
			m_playerShip.Respawn();
			
			// request the playing state
			RequestState(EState::PLAYING);
		}
	}

	// check for level completion
	if (IsLevelComplete())
	{
		HandleLevelCompletion();
		return;
	}
}

void CIngameState::UpdateEnemies(Uint32 elapsedTime)
{
	m_enemyFormation.Update(elapsedTime);

	if (m_currentState == EState::PLAYING)
	{
		// see if it is time to spawn a boss
		CIngameState* ingameState = dynamic_cast<CIngameState*>(CApp::GetInstance()->GetGameManager()->GetState());
		if (ingameState->CanSpawnBoss())
		{
			m_boss.Spawn(CBoss::EBossType::RANDOM);
		}
	}

	m_boss.Update(elapsedTime);	
}

bool CIngameState::CanSpawnBoss()
{
	return m_currentState == EState::PLAYING && !m_boss.IsAlive() && static_cast<Uint32>(m_enemyFormation.GetEnemyCount()) >= BOSS_SPAWN_MINIMUM_ENEMIES && Utils::GetTicks() - m_lastBossSpawnTicks > BOSS_SPAWN_INTERVAL_MS;
}

void CIngameState::UpdateProjectiles(Uint32 elapsedTime)
{
	CProjectile* projectilePtr = m_projectilesList.GetHeadElement();
	while (projectilePtr != nullptr)
	{
		projectilePtr->Update(elapsedTime);
				
#if COLLISIONS_ENABLED
		// check if a player projectile has collided against an ENEMY
		if (projectilePtr->GetOwner() == CProjectile::EProjectileOwner::PLAYER)
		{
			CDoubleLinkedList<CEnemy*>& enemiesList = m_enemyFormation.GetEntities();

			CEnemy* enemyPtr = enemiesList.GetHeadElement();
			while (enemyPtr != nullptr)
			{
				if (enemyPtr->CollidesWith(projectilePtr))
				{
					// get the enemy position before destroying it
					float explosionPosX = enemyPtr->GetPosX() + static_cast<float>(CEnemy::SPRITE_WIDTH / 2.0) - static_cast<float>(CExplosion::SPRITE_WIDTH / 2.0);
					float explosionPosY = enemyPtr->GetPosY() + static_cast<float>(CEnemy::SPRITE_HEIGHT / 2.0) - static_cast<float>(CExplosion::SPRITE_HEIGHT / 2.0);

					CEnemy* nextElement = enemiesList.RemoveElement(enemyPtr);
					LOG_SCR_F("Deleting enemy %d\n", (int)(size_t)enemyPtr);
					delete enemyPtr;
					enemyPtr = nextElement;

					m_enemyFormation.OnEnemyDeath();

					// spawn explosion
					SpawnExplosion(CEntity::EEntityType::ENEMY, explosionPosX, explosionPosY);

					// award the player score
					m_score += CEnemyFormation::ENEMY_POINTS_WORTH;

					projectilePtr->OnCollision();
					break; // projectile expires after first hit
				}
				else
				{
					enemyPtr = enemiesList.GetNextElement(enemyPtr);
				}
			}

			// check if a player projectile has collided against a BOSS
			if (m_boss.IsAlive() && m_boss.CollidesWith(projectilePtr))
			{
				// despawn the boss object
				m_boss.Despawn();

				// spawn explosion
				float explosionPosX = m_boss.GetPosX() + static_cast<float>(m_boss.GetSpriteWidth() / 2.0) - static_cast<float>(CExplosion::SPRITE_WIDTH / 2.0);
				float explosionPosY = m_boss.GetPosY() + static_cast<float>(m_boss.GetSpriteHeight() / 2.0) - static_cast<float>(CExplosion::SPRITE_HEIGHT / 2.0);
				SpawnExplosion(CEntity::EEntityType::BOSS, explosionPosX, explosionPosY);

				// award the player score
				m_score += m_boss.GetPointsWorth();

				// projectile expires after first hit
				projectilePtr->OnCollision();

				// update the boss spawn timer for next boss
				m_lastBossSpawnTicks = Utils::GetTicks();
			}
		}
		else if (m_playerShip.IsAlive() && projectilePtr->GetOwner() == CProjectile::EProjectileOwner::ENEMY) // check if an enemy projectile has collided against the player
		{
			if (m_playerShip.CollidesWith(projectilePtr))
			{
				if (!m_playerShip.IsShieldUp())
				{
					// get the player position before destroying it
					float explosionPosX = m_playerShip.GetPosX() + static_cast<float>(CPlayerShip::SPRITE_WIDTH / 2.0) - static_cast<float>(CExplosion::SPRITE_WIDTH / 2.0);
					float explosionPosY = m_playerShip.GetPosY() + static_cast<float>(CPlayerShip::SPRITE_HEIGHT / 2.0) - static_cast<float>(CExplosion::SPRITE_HEIGHT / 2.0);

					// spawn explosion
					SpawnExplosion(CEntity::EEntityType::PLAYERSHIP, explosionPosX, explosionPosY);

					// notify the playership object that it has died
					m_playerShip.OnCollision();

					OnPlayerDeath();
				}
								
				// projectile expires after first hit
				projectilePtr->OnCollision();
			}
		}
#endif

		// remove any of the dead projectiles from the list
		if (projectilePtr->IsAlive())
		{
			projectilePtr = m_projectilesList.GetNextElement(projectilePtr);
		}
		else
		{
			CProjectile* nextElement = m_projectilesList.RemoveElement(projectilePtr);
			LOG_SCR_F("Deleting projectile %d\n", (int)(size_t)projectilePtr);
			delete projectilePtr;
			projectilePtr = nextElement;
		}
	}
}

void CIngameState::UpdateExplosions(Uint32 elapsedTime)
{
	CExplosion* explosionPtr = m_explosionsList.GetHeadElement();
	while (explosionPtr != nullptr)
	{
		explosionPtr->Update(elapsedTime);

		// remove any of the dead explosions from the list
		if (explosionPtr->IsAlive())
		{
			explosionPtr = m_explosionsList.GetNextElement(explosionPtr);
		}
		else
		{
			CExplosion* nextElement = m_explosionsList.RemoveElement(explosionPtr);
			LOG_SCR_F("Deleting explosion %d\n", (int)(size_t)explosionPtr);
			delete explosionPtr;
			explosionPtr = nextElement;
		}
	}
}

void CIngameState::UpdateText(Uint32 elapsedTime)
{
	// detect if any of the values has changed... if yes, update the text texture(s) - this is good to avoid re-creating the texture every frame

	const size_t MAX_BUFFER_SIZE = 64; // 64 characters is enough for all these strings
	char buffer[MAX_BUFFER_SIZE];

	if (m_score != m_previousScore)
	{
		sprintf_s(buffer, "%d\0", m_score);
		m_scoreValueTexture.CreateFromText(buffer, SDL_Color{ 255, 255, 0, 255 });
		m_previousScore = m_score;
		LOG_SCR_F("Updated score value texture: %d\n", m_score);
	}

	if (m_level != m_previousLevel)
	{
		sprintf_s(buffer, "%d\0", m_level);
		m_levelValueTexture.CreateFromText(buffer, SDL_Color{ 255, 255, 0, 255 });
		m_previousLevel = m_level;
		LOG_SCR_F("Updated level value texture: %d\n", m_level);
	}
	
	if (m_lives != m_previousLives)
	{
		sprintf_s(buffer, "%d\0", m_lives);
		m_livesValueTexture.CreateFromText(buffer, SDL_Color{ 255, 255, 0, 255 });
		m_previousLives = m_lives;
		LOG_SCR_F("Updated lives value texture: %d\n", m_lives);
	}

	if (m_currentMessageState != EMessageState::NONE && Utils::GetTicks() - m_lastMessageDisplayTicks > GetCurrentMessageDuration())
	{
		if (m_currentMessageState == EMessageState::GAME_OVER)
		{
			ReturnToIntroState();
		}
		RequestMessageState(EMessageState::NONE);
	}
}

void CIngameState::Draw()
{
	if (m_currentState == EState::PLAYING || m_currentState == EState::PLAYER_DEATH_COOLDOWN)
	{
		m_starfield.Draw();
		m_playerShip.Draw();
		DrawEnemies();
		DrawProjectiles();
		DrawExplosions();		
		DrawText();
	}		
}

void CIngameState::DrawEnemies()
{
	m_enemyFormation.Draw();
	m_boss.Draw();
}

void CIngameState::DrawProjectiles()
{
	CProjectile* projectilePtr = m_projectilesList.GetHeadElement();
	while (projectilePtr != nullptr)
	{
		projectilePtr->Draw();
		projectilePtr = m_projectilesList.GetNextElement(projectilePtr);
	}
}

void CIngameState::DrawExplosions()
{
	CExplosion* explosionPtr = m_explosionsList.GetHeadElement();
	while (explosionPtr != nullptr)
	{
		explosionPtr->Draw();
		explosionPtr = m_explosionsList.GetNextElement(explosionPtr);
	}
}

void CIngameState::DrawMessage(CTexture* texture)
{
	texture->Draw((CApp::GetInstance()->GetScreenWidth() - texture->GetWidth()) / 2, (CApp::GetInstance()->GetScreenHeight() - texture->GetHeight()) / 2);
}

void CIngameState::DrawText()
{
	m_scoreLabelTexture.Draw(LABEL_BOSS_EFFECT_POSX_OFFSET, LABELS_POSY);
	m_scoreValueTexture.Draw(LABEL_SCORE_VALUE_POSX, LABELS_POSY);
	m_levelLabelTexture.Draw(LABEL_LEVEL_POSX, LABELS_POSY);
	m_levelValueTexture.Draw(LABEL_LEVEL_VALUE_POSX, LABELS_POSY);
	m_livesLabelTexture.Draw(LABEL_LIVES_POSX, LABELS_POSY);
	m_livesValueTexture.Draw(LABEL_LIVES_VALUE_POSX, LABELS_POSY);
	if (IsBossNullifyingPlayerShield())
	{
		m_shieldNullifiedLabelTexture.Draw(CApp::GetInstance()->GetScreenWidth() - LABEL_BOSS_EFFECT_POSX_OFFSET - m_shieldNullifiedLabelTexture.GetWidth(), LABELS_POSY);
	}
	else if (IsBossMakingEnemiesShootDiagonally())
	{
		m_enemiesEnhancedLabelTexture.Draw(CApp::GetInstance()->GetScreenWidth() - LABEL_BOSS_EFFECT_POSX_OFFSET - m_enemiesEnhancedLabelTexture.GetWidth(), LABELS_POSY);
	}

	if (m_currentMessageState == EMessageState::GET_READY)
	{
		DrawMessage(&m_getReadyMessageTexture);
	}
	else if (m_currentMessageState == EMessageState::MISSION_SUCCESSFUL)
	{
		DrawMessage(&m_successMessageTexture);
	}
	else if (m_currentMessageState == EMessageState::GAME_OVER)
	{
		DrawMessage(&m_gameOverMessageTexture);
	}
}

void CIngameState::InitPlayer()
{
	// load gfx
	if (m_playerShipSheetTexture.CreateFromFile(TEXTURE_PLAYERSHIP_SPRITESHEET_FILENAME))
	{	
		LOG_SCR("Playership texture created");

		// create ship
		m_playerShip.Init(&m_playerShipSheetTexture);

#if SOUND_ENABLED
		m_playerShip.InitSound(&m_playerShootSound, &m_playerShieldSound, &m_playerShieldNullifiedSound);
#endif
	}	
}

void CIngameState::InitEnemies()
{
	// load gfx
	if (m_enemySpriteSheetTexture.CreateFromFile(TEXTURE_ENEMY_SPRITESHEET_FILENAME))
	{
		LOG_SCR("Enemies texture created");

		// regular enemies
		m_enemyFormation.InitTexture(&m_enemySpriteSheetTexture);
#if SOUND_ENABLED
		m_enemyFormation.InitSound(&m_enemyAttackSound);
#endif

		// boss
		m_boss.Init(&m_enemySpriteSheetTexture);
#if SOUND_ENABLED
		m_boss.InitSound(&m_bossSpawnSound, &m_bossNullifySound, &m_bossEnhanceSound);
#endif
		m_lastBossSpawnTicks = Utils::GetTicks();
	}
}

void CIngameState::InitProjectiles()
{
	// load gfx
	if (m_projectilesSheetTexture.CreateFromFile(TEXTURE_PROJECTILES_SPRITESHEET_FILENAME))
	{
		LOG_SCR("Projectiles texture created");
	}
}

void CIngameState::InitExplosions()
{
	// explosions use the projectiles spritesheet
}

void CIngameState::InitText()
{
	m_scoreLabelTexture.CreateFromText(LABEL_SCORE_TEXT.c_str(), LABELS_COLOR);
	m_levelLabelTexture.CreateFromText(LABEL_LEVEL_TEXT.c_str(), LABELS_COLOR);
	m_livesLabelTexture.CreateFromText(LABEL_LIVES_TEXT.c_str(), LABELS_COLOR);
	m_shieldNullifiedLabelTexture.CreateFromText(LABEL_SHIELD_NULLIFIED_TEXT.c_str(), LABEL_SHIELD_NULLIFIED_COLOR);
	m_enemiesEnhancedLabelTexture.CreateFromText(LABEL_ENEMIES_ENHANCED_TEXT.c_str(), LABEL_ENEMIES_ENHANCED_COLOR);	
	m_getReadyMessageTexture.CreateFromText(MESSAGE_GET_READY_TEXT, MESSAGE_GET_READY_COLOR, CTexture::EFont::BIG);
	m_successMessageTexture.CreateFromText(MESSAGE_SUCCESS_TEXT, MESSAGE_SUCCESS_COLOR, CTexture::EFont::BIG);
	m_gameOverMessageTexture.CreateFromText(MESSAGE_GAMEOVER_TEXT, MESSAGE_GAMEOVER_COLOR, CTexture::EFont::BIG);
}

void CIngameState::DestroyPlayer()
{
	m_playerShipSheetTexture.Destroy();
	LOG_SCR("Playership texture destroyed");
}

void CIngameState::DestroyEnemies()
{
	m_enemyFormation.Destroy();
	m_boss.Destroy();

	// destroy texture
	m_enemySpriteSheetTexture.Destroy();
	LOG_SCR("Enemy texture destroyed");
}

void CIngameState::DestroyProjectiles()
{
	CProjectile* projectilePtr = m_projectilesList.GetHeadElement();
	while (projectilePtr != nullptr)
	{
		CProjectile* nextElement = m_projectilesList.RemoveElement(projectilePtr);
		LOG_SCR_F("Deleting projectile %d\n", (int)(size_t)projectilePtr);
		delete projectilePtr;
		projectilePtr = nextElement;
	}

	m_projectilesSheetTexture.Destroy();
	LOG_SCR("Projectiles texture destroyed");
}

void CIngameState::DestroyText()
{
	m_gameOverMessageTexture.Destroy();
	m_successMessageTexture.Destroy();
	m_getReadyMessageTexture.Destroy();
	m_livesValueTexture.Destroy();
	m_livesLabelTexture.Destroy();
	m_levelValueTexture.Destroy();
	m_levelLabelTexture.Destroy();
	m_scoreValueTexture.Destroy();
	m_scoreLabelTexture.Destroy();
}

void CIngameState::SpawnProjectile(CProjectile::EProjectileOwner owner, CProjectile::EProjectileType projectileType, float x, float y)
{
	// create projectile
	CProjectile* newProjectile = new CProjectile();
	newProjectile->Init(owner, projectileType, &m_projectilesSheetTexture, x, y);

	// add projectile to linked list	
	m_projectilesList.AddElement(newProjectile);

	LOG_SCR_F("Owner: %d fired a projectile: %d\n", (int)(size_t)owner, (int)(size_t)newProjectile);
}

void CIngameState::SpawnExplosion(CEntity::EEntityType entityType, float x, float y)
{
	// create explosion
	CExplosion* newExplosion = new CExplosion;

	// determine what sound, lifetime and animId to play
	bool isBigExplosion = entityType == CEntity::EEntityType::PLAYERSHIP || entityType == CEntity::EEntityType::BOSS;
#if SOUND_ENABLED
	CSound* sound = isBigExplosion ? &m_playerExplosionSound : &m_enemyExplosionSound;
#endif

	Uint32 explosionLifetimeMs = isBigExplosion ? PLAYER_EXPLOSION_LIFETIME_MS : ENEMY_EXPLOSION_LIFETIME_MS;
	CExplosion::EAnimID animId = isBigExplosion ? CExplosion::EAnimID::PLAYER_EXPLOSION : CExplosion::EAnimID::ENEMY_EXPLOSION;

	newExplosion->Init(&m_projectilesSheetTexture, animId, explosionLifetimeMs);
#if SOUND_ENABLED
	newExplosion->InitSound(sound);
#endif
	newExplosion->SetPosition(x, y);

	// add explosion to the linked list	
	m_explosionsList.AddElement(newExplosion);

	LOG_SCR_F("Spawned an explosion: %d\n", (int)(size_t)newExplosion);
}

void CIngameState::OnPlayerDeath()
{
	m_lives--;
	if (m_lives == 0)
	{
		RequestMessageState(EMessageState::GAME_OVER);
	}
	else
	{
		// speed up starfield speed
		m_starfield.StartAnimation();

		// request state
		RequestState(EState::PLAYER_DEATH_COOLDOWN);
	}
}

void CIngameState::OnEnemyFormationPositionRestarted()
{
}

void CIngameState::OnBossLeave()
{
	// update the timer to wait for the next boss
	m_lastBossSpawnTicks = Utils::GetTicks();
}

void CIngameState::OnAllEnemiesDead()
{
	
}

void CIngameState::SpawnBoss(CBoss::EBossType bossType)
{
	// there is only one boss in the screen at a time, no need to use a linked list
	m_boss.Spawn(bossType);

	LOG_SCR_F("Spawned boss, bossType: %d\n", bossType);
}

Uint32 CIngameState::GetCurrentMessageDuration()
{
	if (m_currentMessageState == EMessageState::GET_READY)
	{
		return MESSAGE_GET_READY_DURATION_MS;
	}
	else if (m_currentMessageState == EMessageState::MISSION_SUCCESSFUL)
	{
		return MESSAGE_SUCCESS_DURATION_MS;
	}
	else if (m_currentMessageState == EMessageState::GAME_OVER)
	{
		return MESSAGE_GAMEOVER_DURATION_MS;
	}

	return 0;
}

bool CIngameState::IsLevelComplete()
{
	return m_enemyFormation.GetEnemyCount() == 0 && !m_boss.IsAlive();
}

void CIngameState::HandleLevelCompletion()
{
	// show a message
	RequestMessageState(EMessageState::MISSION_SUCCESSFUL);

	// increase level
	m_level++;

	// restart enemies for next level
	m_enemyFormation.Spawn();

	// reset the boss spawn timer
	m_lastBossSpawnTicks = Utils::GetTicks();
}

float CIngameState::GetCurrentDifficultyMultiplier()
{
	return 1.0f + (m_level * 0.25f);
}

void CIngameState::ReturnToIntroState()
{
	CApp::GetInstance()->GetGameManager()->RequestState(CGameManager::EGameState::INTRO);
}

#if DEBUG_DRAW
void CIngameState::DrawDebug()
{
#if DEBUG_DRAW_ENEMY_FORMATION_EDGES
	
	// draw edges where enemies can move
	SDL_Color enemyEdgesColor{ 255, 255, 255, 255 };
	CApp::GetInstance()->DrawLine(ENEMY_MOVE_LIMIT, 0, ENEMY_MOVE_LIMIT, CApp::GetInstance()->GetScreenHeight(), enemyEdgesColor);
	int x = CApp::GetInstance()->GetScreenWidth() - ENEMY_MOVE_LIMIT;
	CApp::GetInstance()->DrawLine(x, 0, x, CApp::GetInstance()->GetScreenHeight(), enemyEdgesColor);

	// draw Y limit to where player can move
	SDL_Color playerLimitColor{ 0, 128, 0, 255 };
	CApp::GetInstance()->DrawLine(0, CPlayerShip::MOVE_LIMIT_Y, CApp::GetInstance()->GetScreenWidth(), CPlayerShip::MOVE_LIMIT_Y, playerLimitColor);

	// draw boss spawn y-position bounds
	SDL_Color bossSpawnBoundsColor{ 128, 0, 0, 255 };
	CApp::GetInstance()->DrawLine(0, CBoss::BOSS_SPAWN_Y_LBOUND, CApp::GetInstance()->GetScreenWidth(), CBoss::BOSS_SPAWN_Y_LBOUND, bossSpawnBoundsColor);
	CApp::GetInstance()->DrawLine(0, CBoss::BOSS_SPAWN_Y_UBOUND, CApp::GetInstance()->GetScreenWidth(), CBoss::BOSS_SPAWN_Y_UBOUND, bossSpawnBoundsColor);
	
#endif
}
#endif