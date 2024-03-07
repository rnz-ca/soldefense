/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "enemyformation.h"

#include <assert.h>
#include <cmath>
#include <stdio.h>
#include "ingamestate.h"
#include "playership.h"
#include "utils.h"

void CEnemyFormation::InitTexture(CTexture* spriteSheetTexture)
{
	m_spriteSheetTexture = spriteSheetTexture;
}

#if SOUND_ENABLED
void CEnemyFormation::InitSound(CSound* enemyAttackSound)
{
	m_attackSound = enemyAttackSound;
}
#endif

void CEnemyFormation::Destroy()
{
	CEnemy* enemyPtr = m_entitiesList.GetHeadElement();
	while (enemyPtr != nullptr)
	{
		CEnemy* nextElement = m_entitiesList.RemoveElement(enemyPtr);
		LOG_SCR_F("Deleting enemy %d\n", (int)(size_t)enemyPtr);
		delete enemyPtr;
		enemyPtr = nextElement;
	}
}

void CEnemyFormation::Spawn()
{
	// create formation of enemies
	float initialPosX = ENEMY_MOVE_LIMIT + 1.0f;
	float initialPosY = ENEMY_INITIAL_POS_Y;
	float posX = 0.0f;
	float posY = 0.0f;

	// reset the amount of total enemies
	m_totalEnemies = 0;

	for (Uint8 i = 0; i < ENEMY_NUM_LINES; i++) // rows
	{
		posY = initialPosY + CEnemy::SPRITE_HEIGHT * i;
		for (Uint8 j = 0; j < ENEMY_NUM_ENEMIES_PER_LINE; j++) // columns
		{
			posX = initialPosX + CEnemy::SPRITE_WIDTH * j;

			// instantiate a new enemy object
			CEnemy* newEnemy = new CEnemy;

			// initialize the enemy
			newEnemy->Init(this, m_spriteSheetTexture, i, j, posX, posY);

			// add enemy entity to linked list
			m_entitiesList.AddElement(newEnemy);

			// increase the number of total enemies variable
			m_totalEnemies++;

			// initialize the front enemies map
			if (i == ENEMY_NUM_LINES - 1)
			{
				m_frontEnemiesTable[j] = newEnemy;
			}

			LOG_SCR_F("Enemy created: %d\n", (int)(size_t)newEnemy);
		}
	}

	// set enemy count variable to the total since this is the start of the round
	m_enemyCount = m_totalEnemies;

	// set formation direction
	m_directionX = 1;
	m_directionY = 0;

	SetSpeedMultiplier();

	// set the formation state
	m_state = EState::NORMAL;
}

void CEnemyFormation::Update(Uint32 elapsedTime)
{
	CIngameState* ingameState = dynamic_cast<CIngameState*>(CApp::GetInstance()->GetGameManager()->GetState());

	bool isInPlayingState = ingameState->GetState() == CIngameState::EState::PLAYING;
	if (isInPlayingState)
	{
		// build front enemies table
		UpdateFrontEnemiesTable();
	}	

	CEnemy* enemyPtr = m_entitiesList.GetHeadElement();
	while (enemyPtr != nullptr)
	{
		// determine which enemies can attack
		enemyPtr->SetCanAttack(enemyPtr == m_frontEnemiesTable[enemyPtr->GetSpot().m_column]);

		// update enemies
		enemyPtr->Update(elapsedTime);

#if COLLISIONS_ENABLED
		// check if enemy has collided against the player's ship
		CPlayerShip* playerShip = ingameState->GetPlayerShip();
		if (playerShip->IsAlive() && playerShip->CollidesWith(enemyPtr))
		{
			// collision has occurred, is the player using the shield? if yes, destroy the enemy
			if (playerShip->GetState() == CPlayerShip::EState::USING_SHIELD)
			{
				// get the enemy position before destroying it
				float explosionPosX = enemyPtr->GetPosX() + static_cast<float>(CEnemy::SPRITE_WIDTH / 2.0) - static_cast<float>(CExplosion::SPRITE_WIDTH / 2.0);
				float explosionPosY = enemyPtr->GetPosY() + static_cast<float>(CEnemy::SPRITE_HEIGHT / 2.0) - static_cast<float>(CExplosion::SPRITE_HEIGHT / 2.0);

				CEnemy* nextElement = m_entitiesList.RemoveElement(enemyPtr);
				LOG_SCR_F("Deleting enemy %d\n", (int)(size_t)enemyPtr);
				delete enemyPtr;
				enemyPtr = nextElement;
				
				// call this function to handle what happens when an enemy dies
				OnEnemyDeath();

				// spawn explosion
				ingameState->SpawnExplosion(CEntity::EEntityType::ENEMY, explosionPosX, explosionPosY);
			}
			else
			{
				// get the player position before destroying it
				float explosionPosX = playerShip->GetPosX() + static_cast<float>(CPlayerShip::SPRITE_WIDTH / 2.0) - static_cast<float>(CExplosion::SPRITE_WIDTH / 2.0);
				float explosionPosY = playerShip->GetPosY() + static_cast<float>(CPlayerShip::SPRITE_HEIGHT / 2.0) - static_cast<float>(CExplosion::SPRITE_HEIGHT / 2.0);

				// spawn explosion
				ingameState->SpawnExplosion(CEntity::EEntityType::PLAYERSHIP, explosionPosX, explosionPosY);

				// notify the playership object that it has died
				playerShip->OnCollision();

				// notify ingamestate that player died
				ingameState->OnPlayerDeath();
			}
		}
		else
		{
			enemyPtr = m_entitiesList.GetNextElement(enemyPtr);
		}
#endif
	}
		
	if (ingameState->GetState() == CIngameState::EState::PLAYER_DEATH_COOLDOWN) // if formation is returning to the initial Y position, verify that all enemies have reached their initial positions
	{
		bool isFormationAtInitialYPos = true;
		CEnemy* enemyPtr = m_entitiesList.GetHeadElement();
		while (enemyPtr != nullptr)
		{
			if (!enemyPtr->IsAtInitialPosY())
			{
				isFormationAtInitialYPos = false;
				break;
			}
			enemyPtr = m_entitiesList.GetNextElement(enemyPtr);
		}

		// formation is at initial Y position
		if (isFormationAtInitialYPos)
		{
			m_state = EState::NORMAL;

			CIngameState* ingameState = dynamic_cast<CIngameState*>(CApp::GetInstance()->GetGameManager()->GetState());
			assert(ingameState != nullptr);
			ingameState->OnEnemyFormationPositionRestarted();
		}
	}
	else
	{
		if (m_enemyCount > 0)
		{
			// check if the leftmost entity or the rightmost entity has reached any of the edges, and if they did change direction of the whole squad
			CEnemy* leftmostEnemy = nullptr;
			CEnemy* rightmostEnemy = nullptr;
			CEnemy* topmostEnemy = nullptr;
			CEnemy* bottommostEnemy = nullptr;
			GetEnemiesInEdges(&topmostEnemy, &bottommostEnemy, &leftmostEnemy, &rightmostEnemy);

			if (m_directionX != 0) // moving horizontally
			{
				if ((m_directionX == -1 && leftmostEnemy->GetPosX() <= ENEMY_MOVE_LIMIT) || 
					(m_directionX == 1 && rightmostEnemy->GetPosX() + CEnemy::SPRITE_WIDTH >= CApp::GetInstance()->GetScreenWidth() - ENEMY_MOVE_LIMIT))
				{
					m_previousDirectionX = m_directionX;
					// if any enemy has reached an Y-Pos (limit), just move the formation Left to Right and don't let them go down further
					if (bottommostEnemy->GetPosY() >= CEnemyFormation::FORMATION_MOVE_LIMIT_Y)
					{
						m_directionX *= -1;
						m_directionY = 0;
					}
					else
					{
						m_directionX = 0;
						m_directionY = 1;
						m_formationYMovePos = FORMATION_VERTICAL_SPEED; // move FORMATION_VERTICAL_SPEED pixels vertically
					}
				}
			}
			else if (m_directionY != 0) // moving vertically
			{
				m_formationYMovePos += Utils::ScaleSpeed(elapsedTime, ENEMY_MOVE_SPEED_Y) * m_directionY;
				if (m_formationYMovePos > 0.0f)
				{
					m_directionX = m_previousDirectionX * -1;
					m_directionY = 0;
				}
			}
		}
	}
}

void CEnemyFormation::Draw()
{
	CEnemy* enemyPtr = m_entitiesList.GetHeadElement();
	while (enemyPtr != nullptr)
	{
		enemyPtr->Draw();
		enemyPtr = m_entitiesList.GetNextElement(enemyPtr);
	}
}

void CEnemyFormation::GetEnemiesInEdges(CEnemy** topmostEnemy, CEnemy** bottommostEnemy, CEnemy** leftmostEnemy, CEnemy** rightmostEnemy)
{
	CEnemy* topmostEnemyPtr = nullptr;
	CEnemy* bottommostEnemyPtr = nullptr;
	CEnemy* leftmostEnemyPtr = nullptr;
	CEnemy* rightmostEnemyPtr = nullptr;
	
	CEnemy* enemyPtr = m_entitiesList.GetHeadElement();
	while (enemyPtr != nullptr)
	{
		if (topmostEnemyPtr == nullptr || enemyPtr->GetSpot().m_row < topmostEnemyPtr->GetSpot().m_row)
		{
			topmostEnemyPtr = enemyPtr;
		}
		if (bottommostEnemyPtr == nullptr || enemyPtr->GetSpot().m_row > bottommostEnemyPtr->GetSpot().m_row)
		{
			bottommostEnemyPtr = enemyPtr;
		}

		if (leftmostEnemyPtr == nullptr || enemyPtr->GetSpot().m_column < leftmostEnemyPtr->GetSpot().m_column)
		{
			leftmostEnemyPtr = enemyPtr;
		}
		if (rightmostEnemyPtr == nullptr || enemyPtr->GetSpot().m_column > rightmostEnemyPtr->GetSpot().m_column)
		{
			rightmostEnemyPtr = enemyPtr;
		}
		enemyPtr = m_entitiesList.GetNextElement(enemyPtr);
	}

	*leftmostEnemy = leftmostEnemyPtr;
	*rightmostEnemy = rightmostEnemyPtr;
	*topmostEnemy = topmostEnemyPtr;
	*bottommostEnemy = bottommostEnemyPtr;
}

void CEnemyFormation::SetSpeedMultiplier()
{
	// handle ceil speed multiplier
	float altitude = ceil(static_cast<float>(m_enemyCount) / m_totalEnemies / ENEMY_FORMATION_SPEED_INCREASE_THRESHOLD);
	float maxAltitude = 1.0f / ENEMY_FORMATION_SPEED_INCREASE_THRESHOLD;
	m_speedMultiplier = altitude == maxAltitude ? 1.0f : ENEMY_FORMATION_SPEED_INCREASE_MULTIPLIER * abs(maxAltitude - altitude);

	// scale the speed multiplier by the difficulty
	CIngameState* ingameState = dynamic_cast<CIngameState*>(CApp::GetInstance()->GetGameManager()->GetState());
	float difficultyMultiplier = ingameState->GetCurrentDifficultyMultiplier();
	m_speedMultiplier *= difficultyMultiplier;

	LOG_SCR_F("Speed Multiplier = %2.2f, enemyCount = %d, difficulty multiplier = %2.2f\n", m_speedMultiplier, m_enemyCount, difficultyMultiplier);
}

void CEnemyFormation::UpdateFrontEnemiesTable()
{
	CIngameState* ingameState = dynamic_cast<CIngameState*>(CApp::GetInstance()->GetGameManager()->GetState());
	assert(ingameState != nullptr);

	struct SShortestEnemyDistance
	{
		CEnemy* m_enemyPtr = nullptr;
		float m_distance = -1.0f;
	};

	SShortestEnemyDistance shortestEnemyDistancePerColumn[ENEMY_NUM_ENEMIES_PER_LINE];

	CEnemy* enemyPtr = m_entitiesList.GetHeadElement();
	while (enemyPtr != nullptr)
	{
		const Utils::SGridLocation8& enemySpot = enemyPtr->GetSpot();

		// calculate distance to ship on the y-axis
		float distance = ingameState->GetPlayerShip()->GetCenterPointY() - enemyPtr->GetCenterPointY();

		if (shortestEnemyDistancePerColumn[enemySpot.m_column].m_distance == -1.0f || distance < shortestEnemyDistancePerColumn[enemySpot.m_column].m_distance)
		{
			shortestEnemyDistancePerColumn[enemySpot.m_column].m_distance = distance;
			shortestEnemyDistancePerColumn[enemySpot.m_column].m_enemyPtr = enemyPtr;
		}

		enemyPtr = m_entitiesList.GetNextElement(enemyPtr);
	}

	// fill the local table
	for (Uint8 i = 0; i < ENEMY_NUM_ENEMIES_PER_LINE; i++)
	{
		m_frontEnemiesTable[i] = shortestEnemyDistancePerColumn[i].m_enemyPtr;
	}
}

void CEnemyFormation::OnEnemyDeath()
{
	// reduce the local variable containing enemy count
	m_enemyCount--;

	// check if all enemies are dead
	if (m_enemyCount == 0)
	{
		CIngameState* ingameState = dynamic_cast<CIngameState*>(CApp::GetInstance()->GetGameManager()->GetState());
		ingameState->OnAllEnemiesDead();
	}

	// update the multiplier if needed
	SetSpeedMultiplier();
}