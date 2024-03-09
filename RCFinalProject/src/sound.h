#pragma once

/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "preproc.h"

#if SOUND_ENABLED

#include <fmod.hpp>
#include <fmod_errors.h>
#include <string>

// wrapper class for an FMOD Sound object
class CSound
{
public:
	bool CreateFromFile(const std::string& filename);
	
	void SetLoop(bool loop);

	void Play(float volume = SOUND_DEFAULT_VOLUME);

	void Destroy();

	bool IsCreated() { return m_sound != nullptr; }

private:
	FMOD::Sound* m_sound = nullptr;
	FMOD::Channel* m_channel = nullptr;
};

#endif