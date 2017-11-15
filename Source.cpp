#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Commdlg.h>
#include <assert.h>
#include <tchar.h>
#include <string>
#include <time.h>  
#include <stdlib.h>

#include "kwindow.h"
#include "resource.h"
#include "mp3.h"
#include "kprogressbar.h"
#include "kplaybutton.h"
#include "kpumpkin.h"

#define MAINWIN_HEIGHT 422
#define MAINWIN_WIDTH  400

HBITMAP hPauseImage;
HBITMAP hPlayImage;
KPumpkin * main_win;
KPlayButton * play_pause_button;
KProgressBar * progress_bar;
Mp3 mp3;
bool playing;
bool paused;
WCHAR filename[MAX_PATH];

HDC ResizeBitmap(HWND hWnd, HBITMAP hBMP, int width, int height)
{
	HDC mainDC = GetDC(hWnd);
	HDC src = CreateCompatibleDC(NULL);
	HDC dest = CreateCompatibleDC(NULL);
	BITMAP bmpInfo;

	GetObject(hBMP, sizeof(BITMAP), (LPSTR)&bmpInfo);

	HBITMAP destBMP = CreateCompatibleBitmap(mainDC, width, height);

	SelectObject(dest, destBMP);
	SelectObject(src, hBMP);
	StretchBlt(dest, 0, 0, width, height, src, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, SRCCOPY);

	ReleaseDC(hWnd, mainDC);
	DeleteDC(src);

	return dest;
}

void OnMusicPlaying()
{
	play_pause_button->SetImage(hPauseImage);
}
void OnMusicNotPlaying()
{
	play_pause_button->SetImage(hPlayImage);
}

void OpenClick()
{
	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
	ofn.lpstrFilter = _T("MP3 files\0*.mp3\0\0");
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = _T("Select a File, yo!");
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn))
	{
		mp3.Load(filename);
		OnMusicPlaying();
		mp3.Play();
		paused = false;
		playing = true;
	}
}

void CloseClick()
{
	mp3.Cleanup();
	PostMessage(main_win->m_hWnd, WM_CLOSE, 0, 0);
}

void PauseClick()
{
	if (!playing)
	{
		if (mp3.Load(filename) && mp3.Play())
		{
			paused = false;
			playing = true;
			OnMusicPlaying();
		}
	}
	else if (paused)
	{
		paused = false;
		mp3.Play();
		OnMusicPlaying();
	}
	else
	{
		paused = true;
		mp3.Pause();
		OnMusicNotPlaying();
	}
}

void ProgressBarClick(double percent)
{
	auto duration = mp3.GetDuration();
	long long pos = (long long)(duration * percent);
	mp3.SetPositions(&pos, &duration, true);

	if (paused)
		mp3.Pause();
}

VOID CALLBACK TimerProc(
	_In_ HWND     hwnd,
	_In_ UINT     uMsg,
	_In_ UINT_PTR idEvent,
	_In_ DWORD    dwTime
)
{
	long evCode;
	// if music stopped playing
	if (!playing || mp3.WaitForCompletion(0, &evCode))
	{
		progress_bar->SetCurrStep(0);
		playing = false;
		OnMusicNotPlaying();
	}
	else
	{
		double percent = (double)mp3.GetCurrentPosition() / mp3.GetDuration();
		progress_bar->SetCurrStep((long long)(progress_bar->GetMaxStep() * percent));
	}
}

VOID CALLBACK ChangeBackgroundTimer(
	_In_ HWND     hwnd,
	_In_ UINT     uMsg,
	_In_ UINT_PTR idEvent,
	_In_ DWORD    dwTime
)
{
	static bool blackSet = true;
	if (playing && !paused)
	{
		auto r = (rand() % 77) + 50;
		auto g = (rand() % 77) + 50;
		auto b = (rand() % 77) + 50;
		progress_bar->SetColors(RGB(r, g, b), RGB(r << 1, g << 1, b << 1));
		blackSet = false;
	}
	else if (!blackSet)
	{
		progress_bar->SetColors(RGB(0, 0, 0), RGB(40, 40, 40));
		blackSet = true;
	}
}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCmd, int nShow)
{
	HBITMAP hMainBackground = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_MAINIMAGE), IMAGE_BITMAP, NULL, NULL, 0);
	HBITMAP hMask = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_MAINLAYER), IMAGE_BITMAP, NULL, NULL, LR_CREATEDIBSECTION | LR_MONOCHROME);
	main_win = new KPumpkin(hInst, WS_POPUP, NULL, hMainBackground, hMask, 100, 100);

	HBITMAP hPlayMask = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_BIGBTNMASK), IMAGE_BITMAP, NULL, NULL, LR_CREATEDIBSECTION | LR_MONOCHROME);
	HBITMAP hLoadImage = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_LOADBTNIMAGE), IMAGE_BITMAP, NULL, NULL, 0);
	KPlayButton playButton(hInst, main_win->m_hWnd, hLoadImage, hPlayMask, 74, 167, OpenClick);
	playButton.ShowWindow(nShow);

	HBITMAP hExitImage = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_EXITIMAGE), IMAGE_BITMAP, NULL, NULL, 0);
	KPlayButton exitButton(hInst, main_win->m_hWnd, hExitImage, hPlayMask, 247, 167, CloseClick);
	exitButton.ShowWindow(nShow);

	HBITMAP hSmallMask = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_SMALLBTNMASK), IMAGE_BITMAP, NULL, NULL, LR_CREATEDIBSECTION | LR_MONOCHROME);
	hPlayImage = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_PLAYIMAGE), IMAGE_BITMAP, NULL, NULL, 0);
	hPauseImage = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_PAUSEIMAGE), IMAGE_BITMAP, NULL, NULL, 0);
	play_pause_button = new KPlayButton(hInst, main_win->m_hWnd, hPlayImage, hSmallMask, 168, 217, PauseClick);
	play_pause_button->ShowWindow(nShow);

	HBITMAP hProgressMask = (HBITMAP)LoadImage(hInst, MAKEINTRESOURCE(IDB_PROGRESSMASK), IMAGE_BITMAP, NULL, NULL, LR_CREATEDIBSECTION | LR_MONOCHROME);
	progress_bar = new KProgressBar(hInst, main_win->m_hWnd, RGB(0, 0, 255), RGB(0, 0, 127), hProgressMask, 1000, 94, 294, ProgressBarClick);
	progress_bar->ShowWindow(nShow);

	main_win->ShowWindow(nShow);
	main_win->UpdateWindow();
	playButton.UpdateWindow();
	exitButton.UpdateWindow();
	play_pause_button->UpdateWindow();
	progress_bar->UpdateWindow();

	srand((unsigned int)time(NULL));

	main_win->SetTimer(0, 100, TimerProc);
	main_win->SetTimer(1, 50, ChangeBackgroundTimer);

	auto ret = main_win->MessageLoop();
	main_win->KillTimer(0);
	main_win->KillTimer(1);
	return ret;
	
}