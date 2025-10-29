#include "Sound_Manager.h"

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize()
{
	// 사운드를 담당하는 대표객체를 생성하는 함수
	FMOD_RESULT result = FMOD::System_Create(&m_pSystem);
	 
	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 

	// 최대 64개의 사운드 동시 재생 가능. 기존에 재생 중이던 사운드가 있다면
	// 해당 사운드는 자동으로 다른 채널로 들어가서 재생되게 된다.
	// 볼륨 세팅 걱정 안해도 될 듯?
	m_pSystem->init(64, FMOD_INIT_NORMAL, NULL);

	LoadSoundFile();

	return S_OK;
}

CSound_Manager* CSound_Manager::Create()
{
	CSound_Manager* pInstance = new CSound_Manager();
	pInstance->Initialize();
	return pInstance;
}

void CSound_Manager::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		Mypair.second->release();
	}
	m_mapSound.clear();

	m_pSystem->release();
	m_pSystem->close();
}

void CSound_Manager::PlaySoundOnce(const _wstring& pSoundKey, CHANNELID eID, float fVolume)
{
	//auto iter = find_if(m_mapSound.begin(), m_mapSound.end(),
	//	[&](auto& iter)->_bool
	//	{
	//		return !lstrcmp(pSoundKey, iter.first);
	//	});

	auto iter = m_mapSound.find(pSoundKey);

	if (iter == m_mapSound.end())
		return;

	_bool bPlay = FALSE;

	m_pSystem->playSound(iter->second, nullptr, FALSE, &m_pChannelArr[ENUM_CLASS(eID)]);
	m_pChannelArr[ENUM_CLASS(eID)]->setVolume(fVolume);
	m_pSystem->update();
}

/*
	map<TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	bool bPlay = FALSE;

	m_pSystem->playSound(iter->second, nullptr, FALSE, &m_pChannelArr[ENUM_CLASS(eID)]);
	m_pChannelArr[ENUM_CLASS(eID)]->setVolume(fVolume);
	m_pSystem->update();
*/
void CSound_Manager::PlayBGM(const _wstring& pSoundKey, float fVolume)
{
	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	
	//auto iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
	//	{
	//		return !lstrcmp(pSoundKey, iter.first);
	//	});

	auto iter = m_mapSound.find(pSoundKey);

	if (iter == m_mapSound.end())
		return;

	m_pSystem->playSound(iter->second, nullptr, FALSE, &m_pChannelArr[ENUM_CLASS(CHANNELID::BGM)]);
	m_pChannelArr[ENUM_CLASS(CHANNELID::BGM)]->setMode(FMOD_LOOP_NORMAL);
	m_pChannelArr[ENUM_CLASS(CHANNELID::BGM)]->setVolume(fVolume);
	m_pSystem->update();
}

void CSound_Manager::PauseBGM(_bool bFlag)
{
	m_pChannelArr[ENUM_CLASS(CHANNELID::BGM)]->setPaused(bFlag);
	m_pSystem->update();
}

void CSound_Manager::PlaySoundLoop(const _wstring& pSoundKey, CHANNELID eID, float fVolume)
{
	auto iter = m_mapSound.find(pSoundKey);

	if (iter == m_mapSound.end())
		return;

	m_pSystem->playSound(iter->second, nullptr, FALSE, &m_pChannelArr[ENUM_CLASS(eID)]);
	m_pChannelArr[ENUM_CLASS(eID)]->setMode(FMOD_LOOP_NORMAL);
	m_pChannelArr[ENUM_CLASS(eID)]->setVolume(fVolume);
	m_pSystem->update();
}

void CSound_Manager::StopSound(CHANNELID eID)
{
	m_pChannelArr[ENUM_CLASS(eID)]->stop();
	m_pSystem->update();
}

void CSound_Manager::StopAll()
{
	for (int i = 0; i < ENUM_CLASS(CHANNELID::SOUND_END); ++i)
		m_pChannelArr[i]->stop();
	m_pSystem->update();
}

void CSound_Manager::SetChannelVolume(CHANNELID eID, float fVolume)
{
	m_pChannelArr[ENUM_CLASS(eID)]->setVolume(fVolume);
	m_pSystem->update();
}

void CSound_Manager::LoadSoundFile()
{
	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddatai64_t  fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst64("../Bin/Resources/Sounds/*.*", &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	char szCurPath[128] = "../Bin/Resources/Sounds/";	 // 상대 경로
	char szFullPath[128] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);

		// "../Sound/" + "Success.wav"
		strcat_s(szFullPath, fd.name);
		// "../Sound/Success.wav"

		FMOD::Sound* pSound = nullptr;
		FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_DEFAULT, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			wchar_t wszSoundKey[128] = {};
			MultiByteToWideChar(CP_ACP, 0, fd.name, -1, wszSoundKey, 128);

			std::wstring wstrKey(wszSoundKey);
			m_mapSound.emplace(wstrKey, pSound);
		}
		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext64(handle, &fd);
	}

	m_pSystem->update();
	_findclose(handle);
}
