/***************************************************
*
* Module name: Memory Mapped File wrapper class
*
* Copyright 2005 Tanisys Technologies as an unpublished work.
* All Rights Reserved.
* The information contained herein is the confidential property of Tanisys Technologies and may not be
* reproduced or distributed without the prior written permission of Tanisys Technologies.
*
* This work (and included software, documentation such as READMEs, or other related items)
* is being provided by the copyright holders under the following license. By obtaining, using and/or
* copying this work, you (the licensee) agree that you have read, understood, and will comply with the
* following terms and conditions.
*
* THIS SOFTWARE AND DOCUMENTATION IS PROVIDED "AS IS," AND COPYRIGHT HOLDERS
* MAKE NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT
* LIMITED TO, WARRANTIES OF MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR
* PURPOSE OR THAT THE USE OF THE SOFTWARE OR DOCUMENTATION WILL NOT INFRINGE
* ANY THIRD PARTY PATENTS, COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS.
* COPYRIGHT HOLDERS WILL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL OR
* CONSEQUENTIAL DAMAGES ARISING OUT OF ANY USE OF THE SOFTWARE OR
* DOCUMENTATION.
*
* The name and trademarks of copyright holders may NOT be used in advertising
* or publicity pertaining to the software without specific, written prior
* permission. Title to copyright in this software and any associated
* documentation will at all times remain with copyright holders.
*
* Module Description:
*
* file�� virtual memory�� mapping �ؼ� access�ϱ� ������
* �ӵ��� ������, �Ϲ����� memory ���� ��İ� ���� ������
* �ٷ�� ����. ���� IPC���� shared memory�ε� ����.
*
***************************************************/

#include "stdafx.h"
#include "mmf.h"

CMMF::CMMF()
{
	m_hFile = INVALID_HANDLE_VALUE;
	m_hMapping = 0;
	m_pData = NULL;
	m_nLen = 0;
}

CMMF::~CMMF()
{
	close();
}

bool CMMF::flush()
{
	if (m_pData == NULL)
		return false;
	return FlushViewOfFile(m_pData, 0) == TRUE;
}

void CMMF::close()
{
	unmap();
	if (m_hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

	m_sName = "";
	m_nLen = 0;
}

bool CMMF::unmap()
{
	if (m_pData != NULL) {
		if (!FlushViewOfFile(m_pData, 0)) {
			TRACE("%s: flush failed\n", __FUNCTION__);
			return false;
		}
		if (!UnmapViewOfFile(m_pData)) {
			TRACE("%s: unmap view failed\n", __FUNCTION__);
			return false;
		}
		m_pData = NULL;
	}

	if (m_hMapping != NULL) {
		if (!CloseHandle(m_hMapping)) {
			TRACE("%s: close failed\n", __FUNCTION__);
			return false;
		}

		m_hMapping = NULL;
	}
	return true;
}

bool CMMF::map(size_t sz)
{
	DWORD msz = m_nLen;
	if (sz != -1)
		msz = (DWORD)sz;

	m_sName.Replace('\\', '_');
	m_sName.MakeUpper();

	m_hMapping = ::CreateFileMapping(m_hFile, NULL, PAGE_READWRITE, 0, msz, m_sName);
	if (m_hMapping == NULL) {
		TRACE("%s: create map error: %d\n", __FUNCTION__, GetLastError());
		close();
		return false;
	}

	m_pData = MapViewOfFile(m_hMapping, FILE_MAP_WRITE, 0, 0, 0);
	if (m_pData == NULL) {
		TRACE("%s: create view error: %d\n", __FUNCTION__, GetLastError());
		return false;
	}

	return true;
}

// ���Ͽ� ���ϱ�. 
// create() �Լ����� size=0���� �������� ���� ����� �� �ִ�.
// �ӵ��� ������ ����� ������ ����.

bool CMMF::add(const void* buf, DWORD sz)
{
	DWORD wsz = 0;
	if (!m_hFile)
		return false;

	bool bOk = (::WriteFile(m_hFile, buf, sz, &wsz, NULL) != 0);
	if (wsz != sz) {
		TRACE("%s: write count mismatch. %d != %d\n", __FUNCTION__, sz, wsz);
		return false;
	}
	
	m_nLen += sz;

//	TRACE("%s: size=%d\n", __FUNCTION__, m_nLen);
	return bOk;
}

bool CMMF::create(const char* fn, DWORD sz)
{
	m_hFile = CreateFile(
		fn, 
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);

	if (m_hFile == INVALID_HANDLE_VALUE) {
		TRACE("%s: file '%s' create error\n", __FUNCTION__, fn);
		return false;
	}

	// size�� 0�̸� create�� �ϰ� map�� ���� ����.
	if (sz == 0)
		return true;

	m_nLen = sz;
	m_sName = fn;
	return map();
}

bool CMMF::open(const char* fn)
{
	m_hFile = CreateFile(
		fn, 
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);

	if (m_hFile == INVALID_HANDLE_VALUE) {
		TRACE("%s: file '%s' open error\n", __FUNCTION__, fn);
		return false;
	}

	DWORD szh = 0;
	m_nLen = GetFileSize(m_hFile, &szh);
	if (m_nLen == 0xFFFFFFFF) {
		close();
		return false;
	}

	// ����� 0�̰ų� 4GB �� �Ѵ� ũ��

	if (szh || (szh == 0 && m_nLen == 0)) {
		close();
		return false;
	}
	m_sName = fn;
	return map();
}
