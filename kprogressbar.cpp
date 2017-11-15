#include <tchar.h>
#include <Windowsx.h>

#include "kprogressbar.h"

void KProgressBar::SetMaxStep(long long maxStep)
{
	this->maxStep = maxStep;
	InvalidateRect(this->m_hWnd, NULL, FALSE);
}

void KProgressBar::SetCurrStep(long long currStep)
{
	this->currStep = currStep;
	InvalidateRect(this->m_hWnd, NULL, FALSE);
}

void KProgressBar::SetColors(COLORREF backgroundColor, COLORREF foregroundColor)
{
	if (background)
		DeleteObject(background);
	this->background = CreateSolidBrush(backgroundColor);
	if (foreground)
		DeleteObject(foreground);
	this->foreground = CreateSolidBrush(foregroundColor);
	InvalidateRect(this->m_hWnd, NULL, FALSE);
}

void KProgressBar::SetBackgroundColor(COLORREF color)
{
	if (background)
		DeleteObject(background);
	this->background = CreateSolidBrush(color);
	InvalidateRect(this->m_hWnd, NULL, FALSE);
}

void KProgressBar::SetForegroundColor(COLORREF color)
{
	if (foreground)
		DeleteObject(foreground);
	this->foreground = CreateSolidBrush(color);
	InvalidateRect(this->m_hWnd, NULL, FALSE);
}

KProgressBar::KProgressBar(HINSTANCE hInst, HWND hParent, COLORREF foreground, 
	COLORREF background, HBITMAP hMask, long long maxStep, int x, int y, void(*OnUserClick)(double))
{
	BITMAP bm;
	GetObject(hMask, sizeof(bm), &bm);

	this->KWindow::CreateEx(0, _T("progress_bar"), _T("progress_bar"), WS_CHILD, x, y, bm.bmWidth, bm.bmHeight, hParent, NULL, hInst);

	HRGN hRgn = GetRegionFromMask(hMask, &(this->hMask));

	SetWindowRgn(this->m_hWnd, hRgn, TRUE);

	this->width = bm.bmWidth;
	this->height = bm.bmHeight;
	this->foreground = CreateSolidBrush(foreground);
	this->background = CreateSolidBrush(background);
	this->maxStep = maxStep;
	this->currStep = 0;
	this->OnUserClick = OnUserClick;
}

void KProgressBar::SetTrack()
{
	TRACKMOUSEEVENT trackMouseEvent;
	trackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
	trackMouseEvent.dwFlags = TME_LEAVE;
	trackMouseEvent.hwndTrack = this->m_hWnd;
	trackMouseEvent.dwHoverTime = 0;
	TrackMouseEvent(&trackMouseEvent);
	tracking = true;
}

void KProgressBar::OnMouseDown(WPARAM wParam, LPARAM lParam)
{
	wasPressed = true;
	if (!tracking)
	{
		SetTrack();
	}
}

void KProgressBar::OnMouseUp(WPARAM wParam, LPARAM lParam)
{
	if (wasPressed)
		OnClick(wParam, lParam);;
	wasPressed = false;
}

void KProgressBar::OnClick(WPARAM wParam, LPARAM lParam)
{
	auto xPos = GET_X_LPARAM(lParam);
	currStep = maxStep * xPos / width;
	InvalidateRect(this->m_hWnd, NULL, FALSE);
	if (OnUserClick)
		OnUserClick((double)currStep / (double)maxStep);
}

void KProgressBar::OnDraw(PAINTSTRUCT ps)
{

	HDC hDC = ps.hdc;
	HDC srcDC = CreateCompatibleDC(hDC);
	HDC destDC = CreateCompatibleDC(hDC);
	HBITMAP destBMP = CreateCompatibleBitmap(hDC, width, height);
	HBITMAP srcBMP = CreateCompatibleBitmap(hDC, width, height);

	SelectObject(srcDC, srcBMP);
	SelectObject(destDC, destBMP);

	BitBlt(srcDC, 0, 0, width, height, destDC, 0, 0, BLACKNESS);
	SelectObject(destDC, background);
	// paint the background
	BitBlt(destDC, 0, 0, width, height, srcDC, 0, 0, 0x00FC008A);

	// paint the foreground
	long long currWidth = currStep * width / maxStep;
	SelectObject(destDC, foreground);
	BitBlt(destDC, 0, 0, (int)currWidth, width, srcDC, 0, 0, 0x00FC008A);

	// copy to paint dc
	MaskBlt(hDC, 0, 0, width, height, destDC, 0, 0, hMask, 0, 0, SRCCOPY);

	// clean-up
	DeleteObject(srcBMP);
	DeleteObject(srcBMP);
	DeleteDC(srcDC);
	DeleteDC(destDC);
}

BOOL KProgressBar::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
	SetCursor(LoadCursor(NULL, IDC_HAND));
	return TRUE;
}