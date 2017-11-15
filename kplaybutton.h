#pragma once
#include <windows.h>

#include "kwindow.h"

class KPlayButton : public KWindow
{
public:
	void(*OnClick)(void);

	void SetImage(HBITMAP hImage)
	{
		this->hImage = hImage;
		InvalidateRect(this->m_hWnd, NULL, FALSE);
	}

	KPlayButton(HINSTANCE hInst, HWND hParent, HBITMAP hImage, HBITMAP hMask, int x, int y, void(*OnClick)(void));
private:
	HBITMAP hImage = NULL;
	HBITMAP hMask = NULL;
	bool wasPressed = false;
	bool tracking = false;

	void SetTrack();
	void OnMouseDown(WPARAM wParam, LPARAM lParam) override;
	void OnMouseUp(WPARAM wParam, LPARAM lParam) override;
	void OnDraw(PAINTSTRUCT ps) override;
	BOOL OnSetCursor(WPARAM wParam, LPARAM lParam) override;
	void OnMouseMove(WPARAM wParam, LPARAM lParam) override;
	void OnMouseLeave(WPARAM wParam, LPARAM lParam) override;
};
