#include "AimApp.h"

DWORD AimApp::AimLabPid = 0;
HWND AimApp::AimLabHWND = nullptr;
HANDLE AimApp::hConsole = nullptr;
bool AimApp::IsRun = false;
bool AimApp::IsStart = false;
int AimApp::PressCount = 0;
HHOOK AimApp::hHook = nullptr;

AimApp::AimApp() {}

void AimApp::Init() {
	GetPid();
	EnumWindows(EnumWindows_GetHWND, AimLabPid);
	//	枚举顶层窗口，对每个顶层窗口执行回调函数，回调函数返回值为真则继续枚举，为否停止枚举
	//	FindWindow()可以获取特定名称的顶层窗口

	hConsole = GetStdHandle(STD_INPUT_HANDLE);
	//	获得控制台句柄

	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, NULL);
	
	WaitStart();

}

void AimApp::WaitStart() {
	const int maxEvent = 1;
	//	捕获事件缓存的大小

	INPUT_RECORD irInBuf;
	//	捕获事件的缓存

	DWORD EventRead;
	//	写会读取到的事件数量

	cout << "Press 'Enter' to Ready" << endl;

	while (1) {
		ReadConsoleInput(hConsole, &irInBuf, maxEvent, &EventRead);
		//	https://blog.csdn.net/RedStone114514/article/details/142301892	控制台捕获事件

		if (irInBuf.EventType == KEY_EVENT) {
			WORD vkcode = irInBuf.Event.KeyEvent.wVirtualKeyCode;
			if (vkcode == VK_RETURN && irInBuf.Event.KeyEvent.bKeyDown) {
				if (IsIconic(AimLabHWND))
					ShowWindow(AimLabHWND, SW_RESTORE);
				RECT rect;
				DwmGetWindowAttribute(AimLabHWND, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
				SetCursorPos(rect.left + (rect.right - rect.left) / 2, rect.top + (rect.bottom - rect.top) / 2);
				
				IsRun = true;

				RecognitionLoop();
				return;
			}
		}
	}
}

void AimApp::WaitPlay() {

}

void AimApp::CaptureWindow() {
	RECT rect;
	DwmGetWindowAttribute(AimLabHWND, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
	//	获取不含投影的窗口矩形

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	int x = rect.left;
	int y = rect.top;

	HDC screenDC = GetDC(NULL);
	//	获取窗口上下文

	HDC memDC = CreateCompatibleDC(screenDC);
	//	创建窗口类型的内存上下文

	HBITMAP windowBitmap = CreateCompatibleBitmap(screenDC, width, height);
	//	创建窗口位图

	SelectObject(memDC, windowBitmap);
	//	将窗口位图选入到内存上下文中

	BitBlt(memDC, 0, 0, width, height, screenDC, x, y, SRCCOPY);
	//	将窗口上下文的内容复制到内存上下文中

	LPVOID shotData = new char[width * height * 4];
	GetBitmapBits(windowBitmap, width * height * 4, shotData);
	Mat res(height, width, CV_8UC4, shotData);

	//imshow("test", res);

	Point clickCoor = FindBall(res, width / 2, height / 2);
	//	计算要点击的图片

	shoot(clickCoor.x, clickCoor.y, width / 2, height / 2, x, y);

	ReleaseDC(AimLabHWND, screenDC);
	DeleteDC(memDC);
	DeleteObject(windowBitmap);
	delete[] shotData;
	//	释放对象
}

void AimApp::RecognitionLoop() {
	while (IsRun) {
		if (IsStart) {
			CaptureWindow();
		}
		waitKey(3);
	}
}

void AimApp::shoot(int AimX, int AimY, int CenterX, int CenterY, int x, int y) {
	int dX = AimX - CenterX;
	int dY = AimY - CenterY - 10;
	MOUSEINPUT drag;
	drag.dx = dX * 1.67;
	drag.dy = dY * 1.5;
	drag.dwFlags = MOUSEEVENTF_MOVE;

	INPUT input0[1] = {0};
	input0[0].type = INPUT_MOUSE;
	input0[0].mi = drag;
	SendInput(1, input0, sizeof(INPUT));
	if (dX < 5 || dY < 5) {
		cout << '[' << AimX << ' ' << AimY << ']' << endl;
		INPUT input1[2] = {0};
		input1[0].type = INPUT_MOUSE;
		input1[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;	
		input1[1].type = INPUT_MOUSE;
		input1[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
		SendInput(2, input1, sizeof(INPUT));
	}
}

Point AimApp::FindBall(Mat inputImg, int CenterX, int CenterY) {
	Mat blurimg, hsv, mask0, mask1;

	blur(inputImg, blurimg, Size(10, 10));
	//	先模糊做降噪处理

	cvtColor(blurimg, hsv, COLOR_BGR2HSV);

	inRange(hsv, Scalar(0, 43, 46), Scalar(10, 255, 255), mask0);
	inRange(hsv, Scalar(156, 43, 46), Scalar(180, 255, 255), mask1);
	//	注意红色有两个范围

	Mat mask = mask0 | mask1;

	vector<vector<Point>> contours;
	//	存储轮廓的数组

	findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	float minDistance = 1.0e10;
	Point res;

	// 遍历所有轮廓
	for (const auto& contour : contours) {
		Moments M = moments(contour);
		// 计算轮廓的矩

		Point2f circleCenter(M.m10 / M.m00, M.m01 / M.m00);
		// 计算几何中心（圆心近似）
		//	https://www.docin.com/p-1822723112.html

		float distance = sqrt(pow(circleCenter.x - CenterX, 2) + std::pow(circleCenter.y - CenterY, 2));
		// 计算到图像中心的距离

		// 更新最小距离和最近圆心
		if (distance < minDistance) {
			minDistance = distance;
			res = circleCenter;
			//cout << res << endl;
		}
	}
	//res = Point(round(res.x), round(res.y));
	
	//Mat maskColor;
	//cvtColor(mask, maskColor, COLOR_GRAY2BGR);

	//circle(maskColor, res, 3, Scalar(0, 255, 0), FILLED);

	//imshow("test", mask);
	return res;
}

BOOL CALLBACK AimApp::EnumWindows_GetHWND(HWND hwnd, LPARAM lParam) {//	枚举顶层窗口找到对应Pid的句柄
	DWORD tempPid;
	GetWindowThreadProcessId(hwnd, &tempPid);
	//	获取对应句柄的Pid

	if (tempPid == lParam) {
		AimLabHWND = hwnd;
		return false;
	}
	return true;
}

LRESULT CALLBACK AimApp::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	//	WindowsHookEx 监控键盘的低层次钩子的回调函数
	//	https://blog.csdn.net/qq_29020861/article/details/54865332
	if (nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT* kbdStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
		if (kbdStruct->vkCode == VK_ESCAPE && wParam == WM_KEYDOWN) {
			// ESC 键被按下
			PressCount++;
			switch (PressCount) {
			case 1:
				cout << "ESC Pressed, Game Start!" << endl;
				IsStart = true;
				break;
			case 2:
				IsRun = false;
				Release();
				cout << "ESC Pressed, Game Over!" << PressCount << endl;
				break;
			}
		}
	}
	// 调用下一个钩子
	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

void AimApp::GetPid() {
	cout << "Enter the PID of AimLab: " << endl;
	cin >> AimLabPid;
}


void AimApp::Release() {
	if (hHook != nullptr) {
		UnhookWindowsHookEx(hHook);
		hHook = nullptr;
	}
}

AimApp::~AimApp() {}


