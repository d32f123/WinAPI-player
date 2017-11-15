#include "kplaybutton.h"
#include <tchar.h>

KPlayButton::KPlayButton(HINSTANCE hInst, HWND hParent, HBITMAP hImage, HBITMAP hMask, int x, int y, void(*OnClick)(void))
{
	BITMAP bm;
	GetObject(hMask, sizeof(bm), &bm);

	this->hImage = hImage;

	this->OnClick = OnClick;
	this->KWindow::CreateEx(0, _T("play_button"), _T("play_button"), WS_CHILD, x, y, bm.bmWidth, bm.bmHeight, hParent, NULL, hInst);

	HRGN hRgn = GetRegionFromMask(hMask, &(this->hMask));

	SetWindowRgn(this->m_hWnd, hRgn, TRUE);
}

void KPlayButton::SetTrack()
{
	TRACKMOUSEEVENT trackMouseEvent;
	trackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
	trackMouseEvent.dwFlags = TME_LEAVE;
	trackMouseEvent.hwndTrack = this->m_hWnd;
	trackMouseEvent.dwHoverTime = 0;
	TrackMouseEvent(&trackMouseEvent);
	tracking = true;
}

void KPlayButton::OnMouseDown(WPARAM wParam, LPARAM lParam)
{
	wasPressed = true;
	if (!tracking)
	{
		SetTrack();
	}
}

void KPlayButton::OnMouseUp(WPARAM wParam, LPARAM lParam)
{
	if (wasPressed && OnClick != NULL)
		OnClick();
	wasPressed = false;
}

void KPlayButton::OnDraw(PAINTSTRUCT ps)
{
	HDC hDC = ps.hdc;
	BITMAP image;
	GetObject(hImage, sizeof(BITMAP), &image);
	HDC hdcBuffer = CreateCompatibleDC(hDC);
	HBITMAP hBmpBuffer = CreateCompatibleBitmap(hDC, image.bmWidth, image.bmHeight);

	SelectObject(hdcBuffer, hBmpBuffer);

	HDC hdcComp = CreateCompatibleDC(hDC);
	SelectObject(hdcComp, hImage);
	MaskBlt(hdcBuffer, 0, 0, image.bmWidth, image.bmHeight, hdcComp, 0, 0, hMask, 0, 0, SRCCOPY);
	DeleteDC(hdcComp);

	BitBlt(hDC, 0, 0, image.bmWidth, image.bmHeight, hdcBuffer, 0, 0, SRCCOPY);

	DeleteDC(hdcBuffer);
	DeleteObject(hBmpBuffer);
}

BOOL KPlayButton::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
	SetCursor(LoadCursor(NULL, IDC_HAND));
	return TRUE;
}

void KPlayButton::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if (!tracking)
	{
		SetTrack();
	}
}

void KPlayButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	wasPressed = false;
	tracking = false;
}