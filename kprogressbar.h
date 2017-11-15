#pragma once
#include <windows.h>

#include "kwindow.h"

class KProgressBar : public KWindow
{
public:
	void(*OnUserClick)(double) = NULL;

	void SetMaxStep(long long maxStep);
	long long GetMaxStep()
	{
		return this->maxStep;
	}

	void SetCurrStep(long long currStep);
	long long GetCurrStep()
	{
		return this->currStep;
	}

	void SetColors(COLORREF backgroundColor, COLORREF foregroundColor);
	void SetBackgroundColor(COLORREF color);
	void SetForegroundColor(COLORREF color);
	KProgressBar(HINSTANCE hInst, HWND hParent, COLORREF foreground, COLORREF background, 
		HBITMAP hMask, long long maxStep, int x, int y, void(*OnUserClick)(double));
private:
	long long maxStep;
	long long currStep;
	HBITMAP hMask;
	HBRUSH background = NULL;
	HBRUSH foreground = NULL;
	bool wasPressed = false;
	bool tracking = false;
	int width;
	int height;

	~KProgressBar()
	{
		if (background)
			DeleteObject(background);
		if (foreground)
			DeleteObject(foreground);
	}

	void SetTrack();
	void OnMouseDown(WPARAM wParam, LPARAM lParam) override;
	void OnMouseUp(WPARAM wParam, LPARAM lParam) override;
	void OnClick(WPARAM wParam, LPARAM lParam);
	void OnDraw(PAINTSTRUCT ps) override;
	BOOL OnSetCursor(WPARAM wParam, LPARAM lParam) override;
};
