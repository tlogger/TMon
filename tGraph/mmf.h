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
* file을 virtual memory에 mapping 해서 access하기 때문에
* 속도가 빠르고, 일반적인 memory 접근 방식과 같기 때문에
* 다루기 편함. 또한 IPC에서 shared memory로도 사용됨.
*
***************************************************/

#pragma once

class CMMF 
{
public:
	CMMF();
	virtual ~CMMF();

public:
	void*			m_pData;
	DWORD			m_nLen;
	HANDLE			m_hMapping;
	HANDLE			m_hFile;
	CString			m_sName;

public:	
	inline void*	getPtr(int idx = 0)	{ return ((char*)m_pData) + idx; }
	inline bool		isOpen()			{ return m_pData != NULL; }
	inline bool		isFileOpen()		{ return (m_hFile != INVALID_HANDLE_VALUE); }
	inline DWORD	getLength()			{ return m_nLen; }

	bool			create(const char* fn, DWORD sz);
	bool			open(const char* fn);
	bool			flush();
	void			close();
	bool			add(const void* buf, DWORD sz);
	bool			map(size_t sz = -1);
	bool			unmap();
};
