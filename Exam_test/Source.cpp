#include <windows.h>
#include <fstream>
#include <tchar.h>
#include <windowsX.h>
#include <string>
#include "resource.h"
#pragma comment(lib, "comctl32")
#include <CommCtrl.h>
using std::wstring;
using namespace std;

HWND hKeyboard[27];
HWND hLastKey = NULL;
HWND hEasy, hMedium, hHard, hText, hMiss, hTime, hChar_c, hPerc, hProg;
TCHAR text[255], buffer_s[255];
HANDLE Time_Score_Thread, Timer_Thread;
int time_score = 0;
string s_buf;
fstream words;
int mistake = 0, percents;
int rand_count, sym_co = 0, timer = 0;

BOOL CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void KeyDownHandler(HWND hwnd, WPARAM wParam, LPARAM lParam);

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPTSTR lpszCmdLine, int nCmdShow)
{
	srand(time(0));
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
}

DWORD WINAPI TimeScore(LPVOID lp)
{
	while (true)
	{
		Sleep((int)lp * 1000);
		time_score++;
	}
}

DWORD WINAPI Timer(LPVOID lp)
{
	TCHAR buf[32];
	SetWindowText(hTime, TEXT("0 сек"));
	while (true)
	{
		Sleep(1000);
		timer++;
		wsprintf(buf, TEXT("%d сек"), timer);
		SetWindowText(hTime, buf);
	}
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		
		for (int i = 0; i < 27; i++)
			hKeyboard[i] = GetDlgItem(hwnd, IDC_A + i);
		words.open(TEXT("Words.txt"));
		rand_count = rand() % (1 - 10) + 1;
		for (int i = 0; i < rand_count; i++)
			getline(words, s_buf);
		for (int i = 0; i < s_buf.size(); i++)
			text[i] = s_buf[i];
		sym_co = _tcslen(text);
		wsprintf(buffer_s, TEXT("%d"), sym_co);
		hEasy = GetDlgItem(hwnd, IDC_RADIO1);
		hMedium = GetDlgItem(hwnd, IDC_RADIO2);
		hHard = GetDlgItem(hwnd, IDC_RADIO3);
		hText = GetDlgItem(hwnd, IDC_EDIT1);
		hMiss = GetDlgItem(hwnd, IDC_EDIT2);
		hTime = GetDlgItem(hwnd, IDC_EDIT3);
		hChar_c = GetDlgItem(hwnd, IDC_EDIT4);
		hPerc = GetDlgItem(hwnd, IDC_EDIT5);
		hProg = GetDlgItem(hwnd, IDC_PROGRESS1);
		SendMessage(hProg, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
		SendMessage(hProg, PBM_SETPOS, 0, 0);

		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_RADIO1)
		{
			Time_Score_Thread = CreateThread(NULL, 0, TimeScore, (LPVOID)30, 0, NULL);
			SetWindowText(hText, text);
			SetWindowText(hMiss, TEXT("0"));
			SetWindowText(hPerc, TEXT("0"));
			SetWindowText(hChar_c, buffer_s);
			Timer_Thread = CreateThread(NULL, 0, Timer, NULL, NULL,NULL);
			EnableWindow(hEasy, FALSE);
			EnableWindow(hMedium, FALSE);
			EnableWindow(hHard, FALSE);
		}
		if (LOWORD(wParam) == IDC_RADIO2)
		{
			Time_Score_Thread = CreateThread(NULL, 0, TimeScore, (LPVOID)20, 0, NULL);
			SetWindowText(hText, text);
			SetWindowText(hMiss, TEXT("0"));
			SetWindowText(hPerc, TEXT("0"));
			SetWindowText(hChar_c, buffer_s);
			Timer_Thread = CreateThread(NULL, 0, Timer, NULL, NULL, NULL);
			EnableWindow(hEasy, FALSE);
			EnableWindow(hMedium, FALSE);
			EnableWindow(hHard, FALSE);
		}
		if (LOWORD(wParam) == IDC_RADIO3)
		{
			Time_Score_Thread = CreateThread(NULL, 0, TimeScore, (LPVOID)10, 0, NULL);
			SetWindowText(hText, text);
			SetWindowText(hMiss, TEXT("0"));
			SetWindowText(hPerc, TEXT("0"));
			SetWindowText(hChar_c, buffer_s);
			Timer_Thread = CreateThread(NULL, 0, Timer, NULL, NULL, NULL);
			EnableWindow(hEasy, FALSE);
			EnableWindow(hMedium, FALSE);
			EnableWindow(hHard, FALSE);
		}
		break;
	case WM_SYSKEYDOWN:
		KeyDownHandler(hwnd, wParam, lParam);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return TRUE;
}

void KeyDownHandler(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	EnableWindow(hLastKey, FALSE);
	int num_key = 65;
	int vkey = (TCHAR)wParam;
	WCHAR str[50];
	wsprintf(str, L"vkey: %c", vkey);
	for (int i = 0; num_key <= 90; i++, num_key++)
	{
		if (vkey == num_key)
		{
			EnableWindow(hKeyboard[i], TRUE);
			hLastKey = hKeyboard[i];
		}
		else if (vkey == 32)
		{
			EnableWindow(hKeyboard[26], TRUE);
			hLastKey = hKeyboard[26];
		}
	}
	if (TCHAR(vkey + 32) == (int)text[0] || (vkey == 32 && text[0] == ' '))
	{
		int i;
		for (i = 1; i < sym_co; i++)
		{
			text[i - 1] = text[i];
			text[sym_co - 1] = '\0';
		}
		if (mistake == 0)
		{
			SetWindowText(hPerc, TEXT("100"));
			SendMessage(hProg, PBM_SETPOS, 100, 0);
		}
		sym_co--;
		SetWindowText(hText, text);
	}
	else
	{
		mistake++;
		wsprintf(buffer_s, TEXT("%d"), mistake);
		SetWindowText(hMiss, buffer_s);
		percents = 100 - ((mistake * 100) / sym_co);
		wsprintf(buffer_s, TEXT("%d"), percents);
		if (percents > 0)
		{
			SetWindowText(hPerc, buffer_s);
			SendMessage(hProg, PBM_SETPOS, percents, 0);
		}
		else
		{
			SetWindowText(hPerc, TEXT("1"));
			SendMessage(hProg, PBM_SETPOS, 1, 0);
		}
	}
	if (text[0] == '\0')
	{
		TerminateThread(Timer_Thread, 0);
		TerminateThread(Time_Score_Thread, 0);
		EnableWindow(hText, TRUE);
		SetFocus(hText);
		int result = 10 - (mistake / 5) - time_score;
		wsprintf(buffer_s, TEXT("Твоя оценка = %d / 10"), result);
		SetWindowText(hwnd, buffer_s);
	}
}

