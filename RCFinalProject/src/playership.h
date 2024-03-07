#pragma once

/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "entity.h"
#include "preproc.h"
#include "projectile.h"
#include "sound.h"

class CPlayerShip : public CEntity
{
public:
	// animation IDs
	enum class EAnimID : int
	{
		IDLE = 0
	};
	enum class EShieldAnimID : int
	{
		IDLE = 0
	};

	struct SInputData
	{
		Uint8 m_up;
		Uint8 m_down;
		Uint8 m_left;
		Uint8 m_right;
		Uint8 m_fire;
		Uint8 m_shield;

		SInputData()
		{
			Reset();
		}

		void Reset()
		{
			m_up = 0;
			m_down = 0;
			m_left = 0;
			m_right = 0;
			m_fire = 0;
			m_shield = 0;
		}
	};

	enum class EState : int
	{
		NORMAL,
		USING_SHIELD,
	};

	CPlayerShip();

	void Init(CTexture* spriteSheetTexture);
#if SOUND_ENABLED
	void InitSound(CSound* shootSound, CSound* shieldSound, CSound* shieldNullifiedSound);
#endif

	void Update(Uint32 elapsedTime);
	void Draw();

	void SetInputUp(Uint8 v) { m_inputData.m_up = v; }
	void SetInputDown(Uint8 v) { m_inputData.m_down = v; }
	void SetInputLeft(Uint8 v) { m_inputData.m_left = v; }
	void SetInputRight(Uint8 v) { m_inputData.m_right = v; }
	void SetInputFire(Uint8 v) { m_inputData.m_fire = v; }
	void SetInputShield(Uint8 v) { m_inputData.m_shield = v; }

	void ShootProjectile();

	void ActivateShield();
	void UpdateShield();

	void Respawn();

	EState GetState() { return m_state; }

	bool CanMove() { return m_state == EState::NORMAL && IsAlive(); }

	void OnCollision();

	bool IsShieldUp() { return m_state == EState::USING_SHIELD; }

	static const int SPRITE_WIDTH = 122;
	static const int SPRITE_HEIGHT = 98;
	static const int MOVE_SPEED_X = 72;
	static const int MOVE_SPEED_Y = 72;
	const Uint32 FIRE_COOLDOWN_MS = 500;
	const Uint32 SHIELD_DURATION = 1500;
	static const int SHIELD_SPRITE_WIDTH = 128;
	static const int SHIELD_SPRITE_HEIGHT = 128;
	static const int MOVE_LIMIT_Y = 700;
	const float COLLIDER_SCALE_FACTOR = 0.77f;
	const float SHIELD_NULLIFIED_SOUND_VOLUME = 0.4f;

private:
	// animation
	static const int ANIM_TABLE_COUNT = 1;
	static const int SHIELD_ANIM_TABLE_COUNT = 1;
	static SAnimationDef m_animTable[ANIM_TABLE_COUNT];
	static SAnimationDef m_shieldAnimTable[SHIELD_ANIM_TABLE_COUNT];
	CAnimationManager m_shieldAnimationMgr;

#if SOUND_ENABLED
	CSound* m_shootSound = nullptr;
	CSound* m_shieldSound = nullptr;
	CSound* m_shieldNullifiedSound = nullptr;
#endif

	float m_initialPosX = 0.0f;
	float m_initialPosY = 0.0f;
	SInputData m_inputData;

	EState m_state = EState::NORMAL;

	int m_lastShootTicks = 0;

	Uint32 m_shieldStartTicks = 0;
};