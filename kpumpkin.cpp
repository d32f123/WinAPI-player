#include "kpumpkin.h"
#include <tchar.h>

KPumpkin::KPumpkin(HINSTANCE hInst, DWORD dwStyle, HWND hParent, HBITMAP hImage, HBITMAP hMask, int x, int y)
{
	BITMAP bm;
	GetObject(hMask, sizeof(bm), &bm);

	this->hMainBackground = hImage;

	this->KWindow::CreateEx(0, _T("MAIN"), _T("MAIN"), dwStyle, x, y, bm.bmWidth, bm.bmHeight, hParent, NULL, hInst);

	HRGN hRgn = GetRegionFromMask(hMask, &(this->hMainMaskBMP));

	SetWindowRgn(this->m_hWnd, hRgn, TRUE);
}

void KPumpkin::OnDraw(PAINTSTRUCT ps)
{
	HDC hDC = ps.hdc;
	BITMAP mainBackground;
	GetObject(hMainBackground, sizeof(BITMAP), &mainBackground);
	HDC hdcBuffer = CreateCompatibleDC(hDC);
	HBITMAP hBmpBuffer = CreateCompatibleBitmap(hDC, mainBackground.bmWidth, mainBackground.bmHeight);

	SelectObject(hdcBuffer, hBmpBuffer);

	HDC hdcComp = CreateCompatibleDC(hDC);
	SelectObject(hdcComp, hMainBackground);
	MaskBlt(hdcBuffer, 0, 0, mainBackground.bmWidth, mainBackground.bmHeight,
		hdcComp, 0, 0, hMainMaskBMP, 0, 0, SRCCOPY);
	DeleteDC(hdcComp);

	auto width = ps.rcPaint.right - ps.rcPaint.left;
	auto height = ps.rcPaint.bottom - ps.rcPaint.top;
	BitBlt(hDC, ps.rcPaint.left, ps.rcPaint.top, width, height, hdcBuffer, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

	DeleteDC(hdcBuffer);
	DeleteObject(hBmpBuffer);
}

LRESULT KPumpkin::DoChitTest(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT hit = DefWindowProc(hWnd, uMsg, wParam, lParam);
	if (hit == HTCLIENT) hit = HTCAPTION;
	return hit;
}