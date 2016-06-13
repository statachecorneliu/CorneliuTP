#include <windows.h>
#include <wchar.h>
#include <commctrl.h>
#include <strsafe.h>
#include "Header.h"
#include <string.h>

HWND _hwnd; 
HINSTANCE _hInstance;
MSG  msg;
int _nCmdShow;
HDC hdc;
PAINTSTRUCT ps;
bool check_WndAdauga = false, check_WndInstruction = false;

HWND hEdit_NUME, hEdit_PRENUME, hEdit_NRom, hEdit_NMAte, hEdit_NInfo;
HWND hwnd_InsertWindow, hwnd_InstructionWnd;

struct Elev{
	char nume[100];
	char prenume[100];
	char nota_rom[10];
	char nota_mate[10];
	char nota_info[10];
	char media[10];
	float media_num;
};
Elev elevi_catalog[16];
int contor_elevi=0;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void Paint_Catalog(HWND hwnd);
void Create_Labels(HWND hwnd);
void Reset_Catalog();
void Clear_Slot(int i);
void Sort_Medie();
void Sort_Nume();
//Fereastra de insertie
LRESULT CALLBACK WndProc_InsertionWnd(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void Regist_Insertion_Wnd();
void Afiseaza_Elev_nou(int i);
bool verify_number(char *str);
//Fereastra de instructiuni
LRESULT CALLBACK WndProc_InstructionWnd(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void Regist_Instruction_Wnd();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	_hInstance = hInstance;
	_nCmdShow = nCmdShow;
	
	WNDCLASSW wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpszClassName = L"MainWindow";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpszMenuName = NULL;
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassW(&wc);

	_hwnd = CreateWindowW(L"MainWindow", L"Catalog virtual",
		WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE,
		20, 20, 700, 700, NULL, NULL, hInstance, NULL);

	ShowWindow(_hwnd, nCmdShow);
	UpdateWindow(_hwnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	static wchar_t *copy_rights = L"Designed and created by: Statache Corneliu";
	static wchar_t *instructiuni;
	 
	switch (msg) {

	case WM_PAINT:
		Paint_Catalog(hwnd);
		break;

	case WM_CREATE:
	{
		HGDIOBJ hfDefault = GetStockObject(DEFAULT_GUI_FONT);
		HWND Nota_Subsol = CreateWindowW(L"Static", copy_rights,
			WS_CHILD | WS_VISIBLE | SS_LEFT ,
			20, 612, 160, 50,
			hwnd, (HMENU)1, NULL, NULL);
		SendMessage(Nota_Subsol, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
		HWND _hwnd_Instructiuni = CreateWindowW(L"Static", instructiuni,
			WS_CHILD | WS_VISIBLE | SS_LEFT,
			660, 20, 220, 400,
			hwnd, (HMENU)1, NULL, NULL);
		//SendMessage(_hwnd_Instructiuni, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
		
		CreateWindow("BUTTON", "Adauga Elev",
			WS_BORDER | WS_CHILD | WS_VISIBLE,
			20, 20, 157, 30,
			hwnd, (HMENU)IDC_MAIN_INSERTION, _hInstance, NULL);
		CreateWindow("BUTTON", "Sorteaza Medii",
			WS_BORDER | WS_CHILD | WS_VISIBLE,
			177, 20, 157, 30,
			hwnd, (HMENU)IDC_MAIN_SORT_MEDIE, _hInstance, NULL);
		CreateWindow("BUTTON", "Sorteaza Nume",
			WS_BORDER | WS_CHILD | WS_VISIBLE,
			334, 20, 157, 30,
			hwnd, (HMENU)IDC_MAIN_SORT_NUME, _hInstance, NULL);
		CreateWindow("BUTTON", "Resetare Catalog",
			WS_BORDER | WS_CHILD | WS_VISIBLE,
			491, 20, 157, 30,
			hwnd, (HMENU)IDC_MAIN_RESET_CTL, _hInstance, NULL);
		CreateWindow("BUTTON", "Instructiuni",
			WS_BORDER | WS_CHILD | WS_VISIBLE,
			491, 620, 157, 30,
			hwnd, (HMENU)IDC_MAIN_INSTRUCTION, _hInstance, NULL);
	}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_MAIN_INSERTION:
			if (!check_WndAdauga)
			{
				check_WndAdauga = true;
				Regist_Insertion_Wnd();
				hwnd_InsertWindow = CreateWindowEx(NULL, "InsertionWindow", "Adauga elev", WS_OVERLAPPEDWINDOW, 800, 100, 400, 300, hwnd, NULL, _hInstance, NULL);
				ShowWindow(hwnd_InsertWindow, _nCmdShow);
				UpdateWindow(hwnd_InsertWindow);
			}
			else
			{
				check_WndAdauga = false;
				DestroyWindow(hwnd_InsertWindow);
			}
			break;
		case IDC_MAIN_INSTRUCTION:
			if (!check_WndInstruction)
			{
				check_WndInstruction = true;
				Regist_Instruction_Wnd();
				hwnd_InstructionWnd = CreateWindowEx(NULL, "InstructionWnd", "Instructiuni", WS_OVERLAPPEDWINDOW, 800, 100, 600, 320, hwnd, NULL, _hInstance, NULL);
				ShowWindow(hwnd_InstructionWnd, _nCmdShow);
				UpdateWindow(hwnd_InstructionWnd);
			}
			else
			{
				check_WndInstruction = false;
				DestroyWindow(hwnd_InstructionWnd);
			}
			break;
		case IDC_MAIN_SORT_MEDIE:
			Sort_Medie();
			break;
		case IDC_MAIN_SORT_NUME:
			Sort_Nume();
			break;
		case IDC_MAIN_RESET_CTL:
			Reset_Catalog();
			contor_elevi = 0;
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}
void Paint_Catalog(HWND hwnd){
	hdc = BeginPaint(hwnd, &ps);

	HPEN hPenOld;
	HPEN hLinePen;
	COLORREF qLineColor;

	Create_Labels(hwnd);

	//Delimitari tabel
	qLineColor = RGB(0, 0, 255);
	hLinePen = CreatePen(PS_SOLID, 7, qLineColor);
	hPenOld = (HPEN)SelectObject(hdc, hLinePen);
	MoveToEx(hdc, 20, 120, NULL);
	LineTo(hdc, 650, 120);

	qLineColor = RGB(0, 0, 255);
	hLinePen = CreatePen(PS_SOLID, 3, qLineColor);
	hPenOld = (HPEN)SelectObject(hdc, hLinePen);
	for (int i = 32; i <= 448; i = i + 32){
		MoveToEx(hdc, 20, 120 + i, NULL);
		LineTo(hdc, 650, 120 + i);
	}

	//Chenar
	qLineColor = RGB(255, 0, 0);
	hLinePen = CreatePen(PS_SOLID, 7, qLineColor);
	hPenOld = (HPEN)SelectObject(hdc, hLinePen);
	MoveToEx(hdc, 20, 70, NULL);
	LineTo(hdc, 650, 70);
	LineTo(hdc, 650, 605);
	LineTo(hdc, 20, 605);
	LineTo(hdc, 20, 70);

	//Linile verticale
	qLineColor = RGB(255, 0, 0);
	hLinePen = CreatePen(PS_SOLID, 5, qLineColor);
	hPenOld = (HPEN)SelectObject(hdc, hLinePen);
	MoveToEx(hdc, 60, 70, NULL);
	LineTo(hdc, 60, 600);
	MoveToEx(hdc, 220, 70, NULL);
	LineTo(hdc, 220, 600);
	MoveToEx(hdc, 400, 70, NULL);
	LineTo(hdc, 400, 600);
	MoveToEx(hdc, 462, 70, NULL);
	LineTo(hdc, 462, 600);
	MoveToEx(hdc, 525, 70, NULL);
	LineTo(hdc, 525, 600);
	MoveToEx(hdc, 587, 70, NULL);
	LineTo(hdc, 587, 600);

	SelectObject(hdc, hPenOld);
	DeleteObject(hLinePen);

	EndPaint(hwnd, &ps);
}
void Create_Labels(HWND hwnd){
	CreateWindowW(L"static", L"Nr.",WS_CHILD | WS_VISIBLE,30, 85, 25, 25,hwnd, (HMENU)1, NULL, NULL);
	CreateWindowW(L"static", L"Numele", WS_CHILD | WS_VISIBLE, 65, 85, 100, 25, hwnd, (HMENU)1, NULL, NULL);
	CreateWindowW(L"static", L"Prenumele", WS_CHILD | WS_VISIBLE, 225, 85, 100, 25, hwnd, (HMENU)1, NULL, NULL);
	CreateWindowW(L"static", L"Rom.", WS_CHILD | WS_VISIBLE, 405, 85, 50, 25, hwnd, (HMENU)1, NULL, NULL);
	CreateWindowW(L"static", L"Mate", WS_CHILD | WS_VISIBLE, 467, 85, 50, 25, hwnd, (HMENU)1, NULL, NULL);
	CreateWindowW(L"static", L"Info", WS_CHILD | WS_VISIBLE, 530, 85, 50, 25, hwnd, (HMENU)1, NULL, NULL);
	CreateWindowW(L"static", L"Medie", WS_CHILD | WS_VISIBLE, 592, 85, 50, 25, hwnd, (HMENU)1, NULL, NULL);
	wchar_t *mod1 = L"1.2.3.4.5.6.7.8.9.";
	wchar_t *mod2 = L"10.11.12.13.14.15.";
	int k = 0;
	for (int i = 0; i <= 256; i = i + 32){
		wchar_t buff[3];
		buff[0] = mod1[k++];	buff[1] = mod1[k++];	buff[2] = '\0';
		CreateWindowW(L"static", buff, WS_CHILD | WS_VISIBLE, 30, 125+i, 25, 25, hwnd, (HMENU)1, NULL, NULL);
	}
	k = 0;
	for (int i = 288; i <= 448; i = i + 32){
		wchar_t buff[4];
		buff[0] = mod2[k++];	buff[1] = mod2[k++];	buff[2] = mod2[k++];	buff[3] = '\0';
		CreateWindowW(L"static", buff, WS_CHILD | WS_VISIBLE, 30, 125 + i, 25, 23, hwnd, (HMENU)1, NULL, NULL);
	}
}
void Reset_Catalog()
{
	for (int i = 0; i < 15; i++)
	{
		CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 65, 125 + 32 * i, 120, 25, _hwnd, (HMENU)1, NULL, NULL);
		CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 225, 125 + 32 * i, 130, 25, _hwnd, (HMENU)1, NULL, NULL);
		CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 405, 125 + 32 * i, 50, 25, _hwnd, (HMENU)1, NULL, NULL);
		CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 467, 125 + 32 * i, 50, 25, _hwnd, (HMENU)1, NULL, NULL);
		CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 530, 125 + 32 * i, 50, 25, _hwnd, (HMENU)1, NULL, NULL);
		CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 592, 125 + 32 * i, 50, 25, _hwnd, (HMENU)1, NULL, NULL);
	}
}
void Clear_Slot(int i)
{
	CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 65, 125 + 32 * i, 120, 25, _hwnd, (HMENU)1, NULL, NULL);
	CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 225, 125 + 32 * i, 130, 25, _hwnd, (HMENU)1, NULL, NULL);
	CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 405, 125 + 32 * i, 50, 25, _hwnd, (HMENU)1, NULL, NULL);
	CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 467, 125 + 32 * i, 50, 25, _hwnd, (HMENU)1, NULL, NULL);
	CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 530, 125 + 32 * i, 50, 25, _hwnd, (HMENU)1, NULL, NULL);
	CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 592, 125 + 32 * i, 50, 25, _hwnd, (HMENU)1, NULL, NULL);
}
void Sort_Medie(){
	for (int i = 0; i < contor_elevi; i++)
	{
		for (int j = 0; j < contor_elevi - i -1; j++)
		{
			if (elevi_catalog[j].media_num < elevi_catalog[j+1].media_num)
			{
				Elev aux;
				aux = elevi_catalog[j];
				elevi_catalog[j] = elevi_catalog[j + 1];
				elevi_catalog[j + 1] = aux;
				Clear_Slot(j); 
				Clear_Slot(j + 1);
				Afiseaza_Elev_nou(j); 
				Afiseaza_Elev_nou(j + 1);
			}
		}
	}
}
void Sort_Nume(){
	for (int i = 0; i < contor_elevi; i++)
	{
		for (int j = 0; j < contor_elevi - i - 1; j++)
		{
			if (strcmp(elevi_catalog[j].nume, elevi_catalog[j + 1].nume) > 0)
			{
				Elev aux;
				aux = elevi_catalog[j];
				elevi_catalog[j] = elevi_catalog[j + 1];
				elevi_catalog[j + 1] = aux;
				Clear_Slot(j);
				Clear_Slot(j + 1);
				Afiseaza_Elev_nou(j);
				Afiseaza_Elev_nou(j + 1);
			}
		}
	}
}

LRESULT CALLBACK WndProc_InsertionWnd(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		//hEdit_PRENUME = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 93, 50, 280, 25, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL); !! Atentie la parametrii cu care definesti o fereastra

		//Numele
		CreateWindowW(L"static", L"Nume: ", WS_CHILD | WS_VISIBLE, 10, 20, 50, 25, hwnd, (HMENU)1, NULL, NULL);
		hEdit_NUME = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE, 93, 19, 280, 25, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);
		//Prenume
		CreateWindowW(L"static", L"Prenume: ", WS_CHILD | WS_VISIBLE, 10, 50, 75, 25, hwnd, (HMENU)1, NULL, NULL);
		hEdit_PRENUME = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE, 93, 50, 280, 25, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);
		
		CreateWindowW(L"static", L"Notele la examene:", WS_CHILD | WS_VISIBLE, 10, 80, 150, 25, hwnd, (HMENU)1, NULL, NULL);
		//Nota Romana
		CreateWindowW(L"static", L"*Romana:", WS_CHILD | WS_VISIBLE, 50, 110, 80, 25, hwnd, (HMENU)1, NULL, NULL);
		hEdit_NRom = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE, 140, 110, 110, 25, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);
		//Nota Mate
		CreateWindowW(L"static", L"*Mate: ", WS_CHILD | WS_VISIBLE, 50, 140, 80, 25, hwnd, (HMENU)1, NULL, NULL);
		hEdit_NMAte = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE, 140, 140, 110, 25, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);
		//Nota Info
		CreateWindowW(L"static", L"*Info: ", WS_CHILD | WS_VISIBLE, 50, 170, 80, 25, hwnd, (HMENU)1, NULL, NULL);
		hEdit_NInfo = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE, 140, 170, 110, 25, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

		//Butonul pentru care vom salva datele
		HGDIOBJ hfDefault = GetStockObject(DEFAULT_GUI_FONT);
		HWND hwndButton = CreateWindowEx(NULL, "BUTTON", "OK", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 50, 220, 120, 25, hwnd, (HMENU)IDC_MAIN_INSERT_BUTTON, GetModuleHandle(NULL), NULL);
		//SendMessage(hwndButton, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));	//Pentru alt stil
	}
		break;

	case WM_KEYDOWN:
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_MAIN_INSERT_BUTTON:
		{
			if (contor_elevi == 15)
			{
				MessageBox(NULL, "Ati depasit numarul maxim de 15 elevi!", "ERROR", MB_ICONERROR);
			}
			else
			{
				SendMessage(hEdit_NUME, WM_GETTEXT, sizeof(elevi_catalog[contor_elevi].nume) / sizeof(char), reinterpret_cast<LPARAM>(elevi_catalog[contor_elevi].nume));
				SendMessage(hEdit_PRENUME, WM_GETTEXT, sizeof(elevi_catalog[contor_elevi].prenume) / sizeof(char), reinterpret_cast<LPARAM>(elevi_catalog[contor_elevi].prenume));
				SendMessage(hEdit_NRom, WM_GETTEXT, sizeof(elevi_catalog[contor_elevi].nota_rom) / sizeof(char), reinterpret_cast<LPARAM>(elevi_catalog[contor_elevi].nota_rom));
				SendMessage(hEdit_NMAte, WM_GETTEXT, sizeof(elevi_catalog[contor_elevi].nota_mate) / sizeof(char), reinterpret_cast<LPARAM>(elevi_catalog[contor_elevi].nota_mate));
				SendMessage(hEdit_NInfo, WM_GETTEXT, sizeof(elevi_catalog[contor_elevi].nota_info) / sizeof(char), reinterpret_cast<LPARAM>(elevi_catalog[contor_elevi].nota_info));
				if (strlen(elevi_catalog[contor_elevi].nume)==0)
				{
					MessageBoxW(hwnd, L"Trebuie introdus un nume!", L"ERROR", MB_ICONERROR);
				}
				else if (verify_number(elevi_catalog[contor_elevi].nota_rom) || verify_number(elevi_catalog[contor_elevi].nota_info) || verify_number(elevi_catalog[contor_elevi].nota_mate))
				{
					MessageBoxW(hwnd, L"Notele introduse nu sunt corecte!\nRespectati Formatul!", L"ERROR", MB_ICONERROR);
				}
				else
				{
					elevi_catalog[contor_elevi].media_num = ((float)atoi(elevi_catalog[contor_elevi].nota_rom) + (float)atoi(elevi_catalog[contor_elevi].nota_mate) + (float)atoi(elevi_catalog[contor_elevi].nota_info)) / 3.0;
					sprintf(elevi_catalog[contor_elevi].media, "%.2f", elevi_catalog[contor_elevi].media_num);
					Afiseaza_Elev_nou(contor_elevi);
					contor_elevi++;
				}
			}
		}
			break;
		}
		break;
	case WM_DESTROY:
	{
		check_WndAdauga = false;
		DestroyWindow(hwnd_InsertWindow);
		return 0;
	}
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}
void Regist_Insertion_Wnd()
{
	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(WNDCLASSEX));
	wClass.cbClsExtra = NULL;
	wClass.cbSize = sizeof(WNDCLASSEX);
	wClass.cbWndExtra = NULL;
	wClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wClass.hIcon = NULL;
	wClass.hIconSm = NULL;
	wClass.hInstance = _hInstance;
	wClass.lpfnWndProc = (WNDPROC)WndProc_InsertionWnd;
	wClass.lpszClassName = "InsertionWindow";
	wClass.lpszMenuName = NULL;
	wClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClassEx(&wClass);
}
void Afiseaza_Elev_nou(int i){
	CreateWindow("static", elevi_catalog[i].nume, WS_CHILD | WS_VISIBLE, 65, 125 + 32 * i, 120, 25, _hwnd, (HMENU)1, NULL, NULL);
	CreateWindow("static", elevi_catalog[i].prenume, WS_CHILD | WS_VISIBLE, 225, 125 + 32 * i, 130, 25, _hwnd, (HMENU)1, NULL, NULL);
	CreateWindow("static", elevi_catalog[i].nota_rom, WS_CHILD | WS_VISIBLE, 405, 125 + 32 * i, 50, 25, _hwnd, (HMENU)1, NULL, NULL);
	CreateWindow("static", elevi_catalog[i].nota_mate, WS_CHILD | WS_VISIBLE, 467, 125 + 32 * i, 50, 25, _hwnd, (HMENU)1, NULL, NULL);
	CreateWindow("static", elevi_catalog[i].nota_info, WS_CHILD | WS_VISIBLE, 530, 125 + 32 * i, 50, 25, _hwnd, (HMENU)1, NULL, NULL);
	CreateWindow("static", elevi_catalog[i].media, WS_CHILD | WS_VISIBLE, 592, 125 + 32 * i, 50, 25, _hwnd, (HMENU)1, NULL, NULL);
}
bool verify_number(char *str){
	if (strlen(str) == 1){
		if (isdigit(str[0])&&str[0]!='0'){
			return false;
		}
		else{ 
			return true;
		}
	}
	else if (strlen(str) == 2){
		if (isdigit(str[0])&&isdigit(str[1])){
			if (atoi(str) < 11){
				return false;
			}
			else{
				return true;
			}
		}
		else{
			return true;
		}
	}
	else{
		return true;
	}
}

LRESULT CALLBACK WndProc_InstructionWnd(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		int i = 0;
		FILE *fd = fopen("Text.txt", "rt");
		while (!feof(fd))
		{
			char bufff[147];
			fgets(bufff,147,fd );
			CreateWindow("static", bufff, WS_CHILD | WS_VISIBLE, 10, 10 + i * 25, 550, 25, hwnd, (HMENU)1, NULL, NULL);
			i++;
		}
		fclose(fd);
	}
		break;

	case WM_DESTROY:
	{
		check_WndInstruction = false;
		DestroyWindow(hwnd_InstructionWnd);
		return 0;
	}
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}
void Regist_Instruction_Wnd()
{
	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(WNDCLASSEX));
	wClass.cbClsExtra = NULL;
	wClass.cbSize = sizeof(WNDCLASSEX);
	wClass.cbWndExtra = NULL;
	wClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wClass.hIcon = NULL;
	wClass.hIconSm = NULL;
	wClass.hInstance = _hInstance;
	wClass.lpfnWndProc = (WNDPROC)WndProc_InstructionWnd;
	wClass.lpszClassName = "InstructionWnd";
	wClass.lpszMenuName = NULL;
	wClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClassEx(&wClass);
}
