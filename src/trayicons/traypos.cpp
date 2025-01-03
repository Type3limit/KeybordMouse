#include "traypos.h"

#ifdef WIN32
#include <process.h>
TrayPos::TrayPos()
{
	UINT	uThreadId;
	m_bTrackMouse = FALSE;
	m_hExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hThread = (HANDLE) _beginthreadex(NULL, 0, TrayPos::TrackMousePt, this, 0, &uThreadId);
	InitializeCriticalSection(&m_cs);
}

TrayPos::~TrayPos()
{
	if(m_hThread != NULL)
	{
		SetEvent(m_hExitEvent);
		if(WaitForSingleObject(m_hThread, 5000) == WAIT_TIMEOUT)
		{
			TerminateThread(m_hThread, 0);
		}

		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	if(m_hExitEvent != NULL)
	{
		CloseHandle(m_hExitEvent);
		m_hExitEvent = NULL;
	}

	DeleteCriticalSection(&m_cs);
}

UINT CALLBACK TrayPos::TrackMousePt(PVOID pvClass)
{
	POINT		ptMouse;
	TrayPos	*pTrayPos = (TrayPos *) pvClass;

	while(WaitForSingleObject(pTrayPos->m_hExitEvent, 100) == WAIT_TIMEOUT)
	{
		if(pTrayPos->m_bTrackMouse == TRUE)
		{
			GetCursorPos(&ptMouse);
			
			if(ptMouse.x != pTrayPos->m_ptMouse.x || ptMouse.y != pTrayPos->m_ptMouse.y)
			{
				pTrayPos->m_bTrackMouse = FALSE;
				pTrayPos->OnMouseLeave();
			}
		}
	}

	return 0;
}

VOID TrayPos::OnMouseMove()
{
	EnterCriticalSection(&m_cs);

	GetCursorPos(&m_ptMouse);
	if(m_bTrackMouse == FALSE)
	{
		OnMouseHover();
		m_bTrackMouse = TRUE;
	}

	LeaveCriticalSection(&m_cs);
}

BOOL TrayPos::IsMouseHover()
{
	return m_bTrackMouse;
}

//////////////////////////////////////////////////////////////////////////

QPoint TrayPos::MousePosition()
{
	return QPoint(m_ptMouse.x, m_ptMouse.y);
}

MsgTrayPos::MsgTrayPos(HWND hwnd, UINT uID, UINT uCallbackMsg)
	: TrayPos()
{
	SetNotifyIconInfo(hwnd, uID, uCallbackMsg);
}

MsgTrayPos::~MsgTrayPos()
{
}

VOID MsgTrayPos::SetNotifyIconInfo(HWND hwnd, UINT uID, UINT uCallbackMsg)
{
	m_hNotifyWnd = hwnd;
	m_uID = uID;
	m_uCallbackMsg = uCallbackMsg;
}

VOID MsgTrayPos::OnMouseHover()
{
	if(m_hNotifyWnd != NULL && IsWindow(m_hNotifyWnd))
		PostMessage(m_hNotifyWnd, m_uCallbackMsg, m_uID, WM_MOUSEHOVER);
}

VOID MsgTrayPos::OnMouseLeave()
{
	if(m_hNotifyWnd != NULL && IsWindow(m_hNotifyWnd))
		PostMessage(m_hNotifyWnd, m_uCallbackMsg, m_uID, WM_MOUSELEAVE);
}
#endif