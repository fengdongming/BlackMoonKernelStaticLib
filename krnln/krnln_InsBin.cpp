#include "stdafx.h"
#include "MyMemFile.h"
#include "Myfunctions.h"
//�ļ���д - �����ֽڼ�
/*
    ���ø�ʽ�� ���߼��͡� �����ֽڼ� �������� ��д�����ݵ��ļ��ţ��ֽڼ� ��д�����ֽڼ����ݣ�... �� - ϵͳ����֧�ֿ�->�ļ���д
    Ӣ�����ƣ�InsBin
    ��������������һ�λ������ֽڼ����ݵ��ļ��е�ǰ��дλ�ô������ļ�����ʱ�������"#��д"��"#�Ķ�"Ȩ�ޡ��ɹ������棬ʧ�ܷ��ؼ١�������Ϊ���������������������һ���������Ա��ظ����ӡ�
    ����<1>������Ϊ����д�����ݵ��ļ��š�������Ϊ�������ͣ�int���������ļ����ɡ����ļ������������ء�
    ����<2>������Ϊ����д�����ֽڼ����ݡ�������Ϊ���ֽڼ���bin������
*/
LIBAPI(BOOL, krnln_InsBin)
{
	PFILEELEMENT pFile = (PFILEELEMENT)ArgInf.m_pCompoundData;
	if(pFile==NULL)
		return NULL;
	if(IsInFileMangerList(pFile)==FALSE)//������Ϸ���
		return NULL;
	PMDATA_INF pArgInf = &ArgInf;

	BOOL bRet = FALSE;

	if(pFile->nType ==1)//�����ļ�
	{
		HANDLE hFile = (HANDLE)pFile->FileHandle;
		INT orgLoc = SetFilePointer(hFile,0,NULL,FILE_CURRENT);
		if(orgLoc == HFILE_ERROR)
			return FALSE;

		DWORD dwNumOfByteRead;
		INT nBkLen = GetFileSize(hFile,NULL) - orgLoc;
		LPBYTE pBkData = NULL;
		if(nBkLen >0)
		{
			pBkData = new BYTE[nBkLen];
			if(!ReadFile(hFile, pBkData, nBkLen, &dwNumOfByteRead, 0))
			{
				delete[] pBkData;
				return FALSE;
			}
			SetFilePointer(hFile,orgLoc,NULL,FILE_BEGIN);//�ָ�ԭλ��
		}

		bRet = TRUE;
		for(INT i=1;i < nArgCount;i++)
		{
			LPBYTE pData = pArgInf[i].m_pBin + 2*sizeof(INT);
			INT nLen = pArgInf[i].m_pInt[1];

			if(WriteFile(hFile,pData,nLen,&dwNumOfByteRead,NULL)==FALSE)
			{
				bRet = FALSE;
				break;
			}
/*			if(FlushFileBuffers(hFile)==FALSE)
			{
				bRet = FALSE;
				break;
			}*/
		}
		if(bRet && pBkData)
		{
			orgLoc = SetFilePointer(hFile,0,NULL,FILE_CURRENT);//���µ�ǰλ��,�Ա�ָ�
			if(WriteFile(hFile,pBkData, nBkLen,&dwNumOfByteRead,NULL))
			{
				SetFilePointer(hFile,orgLoc,NULL,FILE_BEGIN);//�ָ�ԭλ��
//				if(FlushFileBuffers(hFile)==FALSE)
//					bRet = FALSE;
			}
			else
				bRet = FALSE;
		}
		if(pBkData)
			delete[] pBkData;

	}else if(pFile->nType ==2)//�ڴ��ļ�
	{
		CMyMemFile* pMemFile = (CMyMemFile*) pFile->FileHandle;

		INT orgLoc = pMemFile->GetPosition();

		LPBYTE pBufStart, pBufMax;

		INT nBkLen = pMemFile->GetBufferPtr (CMyMemFile::BufferCommand::bufferRead,-1,(void **)&pBufStart,(void **)&pBufMax);
		LPBYTE pBkData = NULL;
		if(nBkLen >0)
		{
			pBkData = new BYTE[nBkLen];
			memcpy(pBkData,pBufStart,nBkLen);
		}

		bRet = TRUE;
		for(INT i=1;i < nArgCount;i++)
		{
			LPBYTE pData = pArgInf[i].m_pBin + 2*sizeof(INT);
			INT nLen = pArgInf[i].m_pInt[1];
			pMemFile->Write(pData,nLen);
		}
		if(pBkData)
		{
			orgLoc = pMemFile->GetPosition();
			pMemFile->Write(pBkData, nBkLen);
			delete[] pBkData;
			pMemFile->Seek (orgLoc,CMyMemFile::SeekPosition::begin);
		}

	}else if(pFile->nType == 3)//�����ļ�
	{
		HANDLE hFile = (HANDLE)pFile->FileHandle;
		INT orgLoc = SetFilePointer(hFile,0,NULL,FILE_CURRENT);
		if(orgLoc == HFILE_ERROR)
			return FALSE;

		DWORD dwNumOfByteRead;
		INT nBkLen = GetFileSize(hFile,NULL) - orgLoc;
		LPBYTE pBkData = NULL;
		if(nBkLen >0)
		{
			pBkData = new BYTE[nBkLen];
			if(!ReadFile(hFile, pBkData, nBkLen, &dwNumOfByteRead, 0))
			{
				delete[] pBkData;
				return FALSE;
			}
			nBkLen = dwNumOfByteRead;
			E_RC4_Calc(orgLoc, (unsigned char*)pBkData, nBkLen, pFile->strTable, pFile->nCryptStart, pFile->strMD5);
			SetFilePointer(hFile,orgLoc,NULL,FILE_BEGIN);//�ָ�ԭλ��
		}

		bRet = TRUE;
		for(INT i=1;i < nArgCount;i++)
		{
			LPBYTE pData = pArgInf[i].m_pBin + 2*sizeof(INT);
			INT nLen = pArgInf[i].m_pInt[1];
			INT nPos = SetFilePointer(hFile,0,NULL,FILE_CURRENT);

			LPBYTE pTMP = (LPBYTE)malloc(nLen);
			memcpy(pTMP, pData, nLen);
			pData = pTMP;

			E_RC4_Calc(orgLoc, (unsigned char*)pData, nLen, pFile->strTable, pFile->nCryptStart, pFile->strMD5);
			if(WriteFile(hFile,pData,nLen,&dwNumOfByteRead,NULL)==FALSE)
			{
				free(pData);
				bRet = FALSE;
				break;
			}
			free(pData);
		}
		if(bRet && pBkData)
		{
			orgLoc = SetFilePointer(hFile,0,NULL,FILE_CURRENT);//���µ�ǰλ��,�Ա�ָ�
			E_RC4_Calc(orgLoc, (unsigned char*)pBkData, nBkLen, pFile->strTable, pFile->nCryptStart, pFile->strMD5);
			if(WriteFile(hFile,pBkData, nBkLen,&dwNumOfByteRead,NULL))
			{
				SetFilePointer(hFile,orgLoc,NULL,FILE_BEGIN);//�ָ�ԭλ��
			}
			else
				bRet = FALSE;
		}
		if(pBkData)
			delete[] pBkData;
	}
	return bRet;
}