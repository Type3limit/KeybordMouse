#ifndef TRAYPOS_H
#define TRAYPOS_H
#include <QPoint>

#ifdef WIN32
#ifndef NOMINMAX
# define NOMINMAX
#endif
#include "windows.h"
#define WM_TRAYNOTIFY WM_USER + 100
class TrayPos
{
private:
	POINT				m_ptMouse;
	HANDLE				m_hThread;
	HANDLE				m_hExitEvent;
	BOOL				m_bTrackMouse;
	CRITICAL_SECTION	m_cs;
public:
	TrayPos();
	virtual ~TrayPos();
	
	static UINT CALLBACK TrackMousePt(PVOID pvClass);
	VOID OnMouseMove();
	BOOL IsMouseHover();
	QPoint MousePosition();
protected:
	virtual VOID OnMouseHover() = 0;
	virtual VOID OnMouseLeave() = 0;
};

class MsgTrayPos : public TrayPos
{
private:
	HWND	m_hNotifyWnd;
	UINT	m_uID;
	UINT	m_uCallbackMsg;
public:
	MsgTrayPos(HWND hwnd=NULL, UINT uID=0, UINT uCallbackMsg=0);
	~MsgTrayPos();

	VOID SetNotifyIconInfo(HWND hwnd, UINT uID, UINT uCallbackMsg);

protected:
	VOID OnMouseHover();
	VOID OnMouseLeave();
};
#endif
#endif