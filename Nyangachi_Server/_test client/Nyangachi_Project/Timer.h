#pragma once

const ULONG MAX_SAMPLE_COUNT = 50;

class CGameTimer
{
public:
	CGameTimer();
	~CGameTimer();

	void Tick(float fLockFPS = 0.f);	//Ÿ�̸� �ð��� ����
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);	//�������� ��� ��� �ð� ��ȯ
	float GetTimeElapsed();		//������ ����Ʈ ��ȯ

private:
	bool m_bHardwareHalsPerformanceCounter;
	float m_fTimeScale;
	float m_fTimeElapsed;
	__int64 m_nCurrentTime;
	__int64 m_nLastTime;
	__int64 m_nPerformanceFrequency;

	float m_fFrameTime[MAX_SAMPLE_COUNT];
	ULONG m_nSampleCount;

	unsigned long m_nCurrentFrameRate;
	unsigned long m_nFramesPerSecond;
	float m_fFPSTimeElapsed;
};

