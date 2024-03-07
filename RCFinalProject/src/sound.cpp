/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

#include "preproc.h"

#if SOUND_ENABLED
#include "sound.h"

#include "app.h"
#include <assert.h>
#include <stdio.h>
#include "utils.h"

bool CSound::CreateFromFile(const std::string& filename)
{
	std::string path = SOUND_DIRECTORY;
	path.append(filename);

	// if the sound is already initialized, destroy it first
	if (m_sound != nullptr)
	{
		Destroy();
	}

	FMOD_RESULT result = CApp::GetInstance()->GetFmodSystem()->createSound(path.c_str(), FMOD_DEFAULT, nullptr, &m_sound);
	if (m_sound == nullptr)
	{
		LOG_SCR_F("Unable to load sound: %s (%s)\n", path.c_str(), FMOD_ErrorString(result));
		return false;
	}

	LOG_SCR_F("Sound loaded successfully: %s\n", path.c_str());
	
	return true;

}

void CSound::SetLoop(bool loop)
{
	assert(m_sound != nullptr);

	m_sound->setMode(loop ? FMOD_LOOP_NORMAL : FMOD_DEFAULT);
}

// main play function
void CSound::Play(float volume)
{
	assert(m_sound != nullptr);

	FMOD::Channel* channel = nullptr;
	FMOD_RESULT result = CApp::GetInstance()->GetFmodSystem()->playSound(m_sound, nullptr, false, &channel);
	if (result != FMOD_OK)
	{
		LOG_SCR_F("Failed to play FMOD sound: %s\n", FMOD_ErrorString(result));
		return;
	}

	if (!CApp::GetInstance()->PrepareSoundChannel(channel))
	{
		return;
	}

	CApp::GetInstance()->SetChannelVolume(channel, volume);
}

void CSound::Destroy()
{
	if (m_sound != nullptr)
	{
		m_sound->release();
		LOG_SCR_F("Sound destroyed successfully: %ld\n", (int)(size_t)m_sound);

		m_channel = nullptr;
		m_sound = nullptr;		
	}
}
#endif