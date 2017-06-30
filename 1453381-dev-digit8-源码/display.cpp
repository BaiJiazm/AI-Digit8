#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<conio.h>
#include<iostream>
#include<vector>
using namespace std;
#define FRONT 15
#define BACK 8

typedef BOOL(WINAPI *PROCSETCONSOLEFONT)(HANDLE, DWORD);

void setcolor(HANDLE hout, const int bg_color, const int fg_color)
{
	SetConsoleTextAttribute(hout, bg_color * 16 + fg_color);
}
void setconsolefont(const HANDLE hout, const int font_no)
{
	HMODULE hKernel32 = GetModuleHandleA("kernel32");
	PROCSETCONSOLEFONT setConsoleFont = (PROCSETCONSOLEFONT)GetProcAddress(hKernel32, "SetConsoleFont");
	setConsoleFont(hout, font_no);
	return;
}
void gotoxy(HANDLE hout, const int X, const int Y)
{
	COORD coord;
	coord.X = X;
	coord.Y = Y;
	SetConsoleCursorPosition(hout, coord);
}

void setconsoleborder(const HANDLE hout, const int cols, const int lines)
{
	char cmd[80];
	system("cls");
	sprintf(cmd, "mode con cols=%d lines=%d", cols, lines);
	system(cmd);
	return;
}

int num[9][7] = {
	{ 0,0,0,0,0,0,0 },
	{ 0,0xC0,0xC0,0xC0,0xC0,0xC0,0 },
	{ 0,0x3F0,0x30,0x3F0,0x300,0x3F0,0 },
	{ 0,0x3F0,0x30,0x3F0,0x30,0x3F0,0 },
	{ 0,0x330,0x330,0x3F0,0x30,0x30,0 },
	{ 0,0x3F0,0x300,0x3F0,0x30,0x3F0,0 },
	{ 0,0x3F0,0x300,0x3F0,0x330,0x3F0,0 },
	{ 0,0x3F0,0x30,0x30,0x30,0x30,0 },
	{ 0,0x3F0,0x330,0x3F0,0x330,0x3F0,0 }
};

void displayOneState(int state, HANDLE hout) {
	int col, row, i, j, pos0, pos1, n;
	pos0 = state >> 27;
	for (row = 0; row < 3; ++row)
		for (col = 0; col < 3; ++col) {
			n = state >> (pos1 = row * 3 + col) * 3 & 7;
			pos0 == pos1 ? n : ++n;
			for (i = 0; i < 7; ++i) {
				for (j = 0; j < 14; ++j) {
					gotoxy(hout, col * 14 + j, row * 7 + i);
					if (num[n][i] >> (13 - j) & 1)
						setcolor(hout, FRONT, FRONT);
					else
						setcolor(hout, BACK, BACK);
					printf(" ");
				}
				printf("\n");
			}
		}
	Sleep(50);
	//_getch();
}

void display(const vector<int>&step) {
	const HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
	setconsoleborder(hout, 46, 23);
	setconsolefont(hout, 0);
	for (int i = 0; i < (int)step.size(); ++i) {
		displayOneState(step[i], hout);
		setcolor(hout, 0, 7);
		printf("µÚ%d²½", i);
	}
	_getch();
}
