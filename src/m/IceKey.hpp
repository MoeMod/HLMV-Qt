#pragma once

#include "stdint.h"

struct TIceSubKey;

class CIceKey
{
public:
	CIceKey(int n = 0);
	~CIceKey(void);

	void SetKey(const uint8_t *pKey);

	void Encrypt(const uint8_t pPlainText[8], uint8_t pCipherText[8]) const;
	void Decrypt(const uint8_t pCipherText[8], uint8_t pPlainText[8]) const;

	int GetKeySize() const { return m_iSize << 3; }
	static int GetBlockSize() { return 8; }

private:
	void BuildSchedule(unsigned short *usKeyBuilder, int n, const int *cpiKeyRotation);

	int m_iSize;
	int m_iRounds;
	TIceSubKey *m_pKeySchedule;
};
