#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <dwmapi.h>
#include <vector>

using namespace std;
using namespace cv;

class AimApp
{
public:
	AimApp();
	static void Init();
	static void RecognitionLoop();
	~AimApp();

private:
	static void Release();
	static void GetPid();
	static BOOL CALLBACK EnumWindows_GetHWND(HWND hwnd, LPARAM lParam);
	static void WaitStart();
	static void WaitPlay();
	static void CaptureWindow();
	static Point FindBall(Mat inputImg, int CenterX, int CenterY);
	static void shoot(int AimX, int AimY, int CenterX, int CenterY, int x, int y);
	static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

private:
	static DWORD AimLabPid;
	static HWND AimLabHWND;
	static HANDLE hConsole;
	static RECT AimLabWindowRect;
	static bool IsRun;
	static bool IsStart;
	static int PressCount;
	static HHOOK hHook;
};
