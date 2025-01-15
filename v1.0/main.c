/*
	Basic GDI Windows Application Demo

	Author: Rafael Sabe
	Email: rafaelmsabe@gmail.com
*/

#include "globldef.h"
#include "strdef.h"

#define MAINWND_WNDCLASS_NAME TEXT("__MAINWNDCLASS__")

#define RUNTIME_STATUS_INIT 0
#define RUNTIME_STATUS_IDLE 1
#define RUNTIME_STATUS_FIRSTSCREEN 2
#define RUNTIME_STATUS_SECONDSCREEN 3

#define CUSTOMCOLOR_BLACK 0x00000000
#define CUSTOMCOLOR_WHITE 0x00ffffff
#define CUSTOMCOLOR_LTGRAY 0x00c0c0c0
#define CUSTOMCOLOR_RED 0x000000ff
#define CUSTOMCOLOR_GREEN 0x0000ff00
#define CUSTOMCOLOR_BLUE 0x00ff0000
#define CUSTOMCOLOR_YELLOW 0x0000ffff
#define CUSTOMCOLOR_MAGENTA 0x00ff00ff
#define CUSTOMCOLOR_CYAN 0x00ffff00

#define CUSTOMBRUSHINDEX_TRANSPARENT 0U
#define CUSTOMBRUSHINDEX_SOLID_BLACK 1U
#define CUSTOMBRUSHINDEX_SOLID_WHITE 2U
#define CUSTOMBRUSHINDEX_SOLID_LTGRAY 3U
#define CUSTOMBRUSHINDEX_SOLID_RED 4U
#define CUSTOMBRUSHINDEX_SOLID_GREEN 5U
#define CUSTOMBRUSHINDEX_SOLID_BLUE 6U
#define CUSTOMBRUSHINDEX_SOLID_YELLOW 7U
#define CUSTOMBRUSHINDEX_SOLID_MAGENTA 8U
#define CUSTOMBRUSHINDEX_SOLID_CYAN 9U

#define PP_CUSTOMBRUSH_LENGTH 10U
#define PP_CUSTOMBRUSH_SIZE (PP_CUSTOMBRUSH_LENGTH*sizeof(VOID*))

#define CUSTOMFONT_LARGE_CHARSET DEFAULT_CHARSET
#define CUSTOMFONT_LARGE_WIDTH 20
#define CUSTOMFONT_LARGE_HEIGHT 35
#define CUSTOMFONT_LARGE_WEIGHT FW_NORMAL

#define CUSTOMFONT_NORMAL_CHARSET DEFAULT_CHARSET
#define CUSTOMFONT_NORMAL_WIDTH 10
#define CUSTOMFONT_NORMAL_HEIGHT 20
#define CUSTOMFONT_NORMAL_WEIGHT FW_NORMAL

#define CUSTOMFONTINDEX_LARGE 0U
#define CUSTOMFONTINDEX_NORMAL 1U

#define PP_CUSTOMFONT_LENGTH 2U
#define PP_CUSTOMFONT_SIZE (PP_CUSTOMFONT_LENGTH*sizeof(VOID*))

#define MAINWND_CAPTION TEXT("WinGDI Demo Application")

//Default Window Color. Not really necessary, but just to be safe.
#define MAINWND_BKCOLOR_DEFAULT CUSTOMCOLOR_WHITE
#define MAINWND_BRUSHINDEX_DEFAULT CUSTOMBRUSHINDEX_SOLID_WHITE

//"First Screen" condition color.
#define MAINWND_BKCOLOR_FIRSTSCREEN CUSTOMCOLOR_LTGRAY
#define MAINWND_BRUSHINDEX_FIRSTSCREEN CUSTOMBRUSHINDEX_SOLID_LTGRAY

//"Second Screen" condition color
#define MAINWND_BKCOLOR_SECONDSCREEN CUSTOMCOLOR_CYAN
#define MAINWND_BRUSHINDEX_SECONDSCREEN CUSTOMBRUSHINDEX_SOLID_BLUE

//Default text color. Not really necessary, but just to be safe.
#define TEXTWND_TEXTCOLOR_DEFAULT CUSTOMCOLOR_BLACK
#define TEXTWND_BKCOLOR_DEFAULT CUSTOMCOLOR_WHITE
#define TEXTWND_BRUSHINDEX_DEFAULT CUSTOMBRUSHINDEX_SOLID_WHITE

//"First Screen" condition text color
#define TEXTWND_TEXTCOLOR_FIRSTSCREEN CUSTOMCOLOR_BLACK
#define TEXTWND_BKCOLOR_FIRSTSCREEN MAINWND_BKCOLOR_FIRSTSCREEN
#define TEXTWND_BRUSHINDEX_FIRSTSCREEN CUSTOMBRUSHINDEX_TRANSPARENT

//"Second Screen" condition text color
#define TEXTWND_TEXTCOLOR_SECONDSCREEN CUSTOMCOLOR_WHITE
#define TEXTWND_BKCOLOR_SECONDSCREEN CUSTOMCOLOR_RED
#define TEXTWND_BRUSHINDEX_SECONDSCREEN CUSTOMBRUSHINDEX_SOLID_GREEN

//Runtime Status defines the current condition of the program. It defines what is about to happen.
//Prev Status defines the last relevant condition of the program. It defines what was the last thing to happen.

INT runtime_status = -1;
INT prev_status = -1;

//Custom Brush and Custom Font are arrays of brush and font objects (HBRUSH) (HFONT) respectivelly.

VOID **pp_custombrush = NULL;
VOID **pp_customfont = NULL;

HWND p_mainwnd = NULL;
HWND p_text1 = NULL;
HWND p_button1 = NULL;
HWND p_button2 = NULL;

WORD mainwnd_wndclass_id = 0u;

//extern here in this context means declaration only.
//These functions are not really external, they're defined in this code,
//extern keyword is just to emphasize that I'm just declaring and not yet defining them.
//I know, for function declaration you don't need the extern keyword because it's already implied by the compiler,
//But I like making things as explicit as possible and also I have OCD so bear with me.

extern BOOL WINAPI app_init(VOID);
extern VOID WINAPI app_deinit(VOID);

//Custom GDIOBJ Init/Deinit: Init/Deinit custom fonts and brushes.
extern BOOL WINAPI custom_gdiobj_init(VOID);
extern VOID WINAPI custom_gdiobj_deinit(VOID);

extern BOOL WINAPI register_mainwnd_wndclass(VOID);
extern BOOL WINAPI create_mainwnd(VOID);
extern BOOL WINAPI create_ctrls(VOID);

//Returns "reference" status. If runtime_status is idle, than it returns prev_status.
//Else it returns runtime_status.

extern INT WINAPI app_get_ref_status(VOID);

extern VOID WINAPI runtime_loop(VOID);

extern VOID WINAPI paintscreen_first(VOID);
extern VOID WINAPI paintscreen_second(VOID);

//Set text font and ctrls size and position.
//These are used when changing screens or when resizing the window.

extern VOID WINAPI text_choose_font(VOID);
extern VOID WINAPI text_align(VOID);
extern VOID WINAPI button_align(VOID);
extern VOID WINAPI ctrls_setup(BOOL text_font_setup);

//Retrieves the size and position of a window, as well as it center coordinates.
//For unused values, set the pointers to NULL.
//returns TRUE if sucessful, FALSE otherwise.

extern BOOL WINAPI window_get_dimensions(HWND p_wnd, INT *p_xpos, INT *p_ypos, INT *p_width, INT *p_height, INT *p_centerx, INT *p_centery);

//Check for messages in the message buffer, dispatch them to the wndproc.
//Returns FALSE when it receives a WM_QUIT message.

extern BOOL WINAPI catch_messages(VOID);

extern LRESULT CALLBACK mainwnd_wndproc(HWND p_wnd, UINT msg, WPARAM wparam, LPARAM lparam);

extern LRESULT CALLBACK mainwnd_event_wmcommand(HWND p_wnd, WPARAM wparam, LPARAM lparam);
extern LRESULT CALLBACK mainwnd_event_wmpaint(HWND p_wnd, WPARAM wparam, LPARAM lparam);
extern LRESULT CALLBACK mainwnd_event_wmctlcolorstatic(HWND p_wnd, WPARAM wparam, LPARAM lparam);
extern LRESULT CALLBACK mainwnd_event_wmsize(HWND p_wnd, WPARAM wparam, LPARAM lparam);
extern LRESULT CALLBACK mainwnd_event_wmdestroy(HWND p_wnd, WPARAM wparam, LPARAM lparam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	p_instance = hInstance;

	if(!app_init())
	{
		app_deinit();
		return 1;
	}

	runtime_loop();

	app_deinit();
	return 0;
}

BOOL WINAPI app_init(VOID)
{
	if(p_instance == NULL)
	{
		MessageBox(NULL, TEXT("Error: Invalid Instance."), TEXT("INIT ERROR"), (MB_ICONSTOP | MB_OK));
		return FALSE;
	}

	p_processheap = GetProcessHeap();
	if(p_processheap == NULL)
	{
		MessageBox(NULL, TEXT("Error: Invalid Process Heap."), TEXT("INIT ERROR"), (MB_ICONSTOP | MB_OK));
		return FALSE;
	}

	if(!custom_gdiobj_init())
	{
		MessageBox(NULL, TEXT("Error: GDIOBJ Init Failed."), TEXT("INIT ERROR"), (MB_ICONSTOP | MB_OK));
		return FALSE;
	}

	if(!register_mainwnd_wndclass())
	{
		MessageBox(NULL, TEXT("Error: Register WNDCLASS Failed."), TEXT("INIT ERROR"), (MB_ICONSTOP | MB_OK));
		return FALSE;
	}

	if(!create_mainwnd())
	{
		MessageBox(NULL, TEXT("Error: Create MAINWND Failed."), TEXT("INIT ERROR"), (MB_ICONSTOP | MB_OK));
		return FALSE;
	}

	if(!create_ctrls())
	{
		MessageBox(NULL, TEXT("Error: Create CTRLS Failed."), TEXT("INIT ERROR"), (MB_ICONSTOP | MB_OK));
		return FALSE;
	}

	runtime_status = RUNTIME_STATUS_INIT;
	return TRUE;
}

VOID WINAPI app_deinit(VOID)
{
	if(p_mainwnd != NULL) DestroyWindow(p_mainwnd);

	if(mainwnd_wndclass_id)
	{
		UnregisterClass(MAINWND_WNDCLASS_NAME, p_instance);
		mainwnd_wndclass_id = 0u;
	}

	custom_gdiobj_deinit();
	return;
}

BOOL WINAPI custom_gdiobj_init(VOID)
{
	SIZE_T n_obj = 0u;
	LOGFONT logfont;

	pp_custombrush = (VOID**) HeapAlloc(p_processheap, HEAP_ZERO_MEMORY, PP_CUSTOMBRUSH_SIZE);
	if(pp_custombrush == NULL) return FALSE;

	pp_custombrush[CUSTOMBRUSHINDEX_TRANSPARENT] = GetStockObject(HOLLOW_BRUSH);
	pp_custombrush[CUSTOMBRUSHINDEX_SOLID_BLACK] = CreateSolidBrush(CUSTOMCOLOR_BLACK);
	pp_custombrush[CUSTOMBRUSHINDEX_SOLID_WHITE] = CreateSolidBrush(CUSTOMCOLOR_WHITE);
	pp_custombrush[CUSTOMBRUSHINDEX_SOLID_LTGRAY] = CreateSolidBrush(CUSTOMCOLOR_LTGRAY);
	pp_custombrush[CUSTOMBRUSHINDEX_SOLID_RED] = CreateSolidBrush(CUSTOMCOLOR_RED);
	pp_custombrush[CUSTOMBRUSHINDEX_SOLID_GREEN] = CreateSolidBrush(CUSTOMCOLOR_GREEN);
	pp_custombrush[CUSTOMBRUSHINDEX_SOLID_BLUE] = CreateSolidBrush(CUSTOMCOLOR_BLUE);
	pp_custombrush[CUSTOMBRUSHINDEX_SOLID_YELLOW] = CreateSolidBrush(CUSTOMCOLOR_YELLOW);
	pp_custombrush[CUSTOMBRUSHINDEX_SOLID_MAGENTA] = CreateSolidBrush(CUSTOMCOLOR_MAGENTA);
	pp_custombrush[CUSTOMBRUSHINDEX_SOLID_CYAN] = CreateSolidBrush(CUSTOMCOLOR_CYAN);

	for(n_obj = 0u; n_obj < PP_CUSTOMBRUSH_LENGTH; n_obj++) if(pp_custombrush[n_obj] == NULL) return FALSE;

	pp_customfont = (VOID**) HeapAlloc(p_processheap, HEAP_ZERO_MEMORY, PP_CUSTOMFONT_SIZE);
	if(pp_customfont == NULL) return FALSE;

	ZeroMemory(&logfont, sizeof(LOGFONT));

	logfont.lfCharSet = CUSTOMFONT_LARGE_CHARSET;
	logfont.lfWidth = CUSTOMFONT_LARGE_WIDTH;
	logfont.lfHeight = CUSTOMFONT_LARGE_HEIGHT;
	logfont.lfWeight = CUSTOMFONT_LARGE_WEIGHT;

	pp_customfont[CUSTOMFONTINDEX_LARGE] = CreateFontIndirect(&logfont);

	logfont.lfCharSet = CUSTOMFONT_NORMAL_CHARSET;
	logfont.lfWidth = CUSTOMFONT_NORMAL_WIDTH;
	logfont.lfHeight = CUSTOMFONT_NORMAL_HEIGHT;
	logfont.lfWeight = CUSTOMFONT_NORMAL_WEIGHT;

	pp_customfont[CUSTOMFONTINDEX_NORMAL] = CreateFontIndirect(&logfont);

	for(n_obj = 0u; n_obj < PP_CUSTOMFONT_LENGTH; n_obj++) if(pp_customfont[n_obj] == NULL) return FALSE;

	return TRUE;
}

VOID WINAPI custom_gdiobj_deinit(VOID)
{
	SIZE_T n_obj = 0u;

	if(p_processheap == NULL) return;

	if(pp_custombrush != NULL)
	{
		for(n_obj = 0u; n_obj < PP_CUSTOMBRUSH_LENGTH; n_obj++)
		{
			if(pp_custombrush[n_obj] != NULL)
			{
				DeleteObject(pp_custombrush[n_obj]);
				pp_custombrush[n_obj] = NULL;
			}
		}

		HeapFree(p_processheap, 0, pp_custombrush);
		pp_custombrush = NULL;
	}

	if(pp_customfont != NULL)
	{
		for(n_obj = 0u; n_obj < PP_CUSTOMFONT_LENGTH; n_obj++)
		{
			if(pp_customfont[n_obj] != NULL)
			{
				DeleteObject(pp_customfont[n_obj]);
				pp_customfont[n_obj] = NULL;
			}
		}

		HeapFree(p_processheap, 0, pp_customfont);
		pp_customfont = NULL;
	}

	return;
}

BOOL WINAPI register_mainwnd_wndclass(VOID)
{
	WNDCLASS wndclass;
	ZeroMemory(&wndclass, sizeof(WNDCLASS));

	wndclass.style = CS_OWNDC;
	wndclass.lpfnWndProc = &mainwnd_wndproc;
	wndclass.hInstance = p_instance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) pp_custombrush[MAINWND_BRUSHINDEX_DEFAULT];
	wndclass.lpszClassName = MAINWND_WNDCLASS_NAME;

	mainwnd_wndclass_id = RegisterClass(&wndclass);

	return (BOOL) mainwnd_wndclass_id;
}

BOOL WINAPI create_mainwnd(VOID)
{
	DWORD style = (WS_VISIBLE | WS_SYSMENU | WS_OVERLAPPED | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX);
	p_mainwnd = CreateWindow(MAINWND_WNDCLASS_NAME, MAINWND_CAPTION, style, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, NULL, NULL, p_instance, NULL);

	return (p_mainwnd != NULL);
}

BOOL WINAPI create_ctrls(VOID)
{
	DWORD style = 0u;

	style = (WS_CHILD | SS_CENTER);
	p_text1 = CreateWindow(TEXT("STATIC"), NULL, style, 0, 0, 0, 0, p_mainwnd, NULL, p_instance, NULL);

	style = (WS_CHILD | WS_TABSTOP | BS_TEXT | BS_PUSHBUTTON | BS_CENTER | BS_VCENTER);
	p_button1 = CreateWindow(TEXT("BUTTON"), NULL, style, 0, 0, 0, 0, p_mainwnd, NULL, p_instance, NULL);
	p_button2 = CreateWindow(TEXT("BUTTON"), NULL, style, 0, 0, 0, 0, p_mainwnd, NULL, p_instance, NULL);

	if(p_text1 == NULL) return FALSE;
	if(p_button1 == NULL) return FALSE;
	if(p_button2 == NULL) return FALSE;

	return TRUE;
}

INT WINAPI app_get_ref_status(VOID)
{
	if(runtime_status == RUNTIME_STATUS_IDLE) return prev_status;

	return runtime_status;
}

VOID WINAPI runtime_loop(VOID)
{
	while(catch_messages())
	{
		switch(runtime_status)
		{
			case RUNTIME_STATUS_IDLE:
				Sleep(10);
				break;

			case RUNTIME_STATUS_INIT:
				text_choose_font();
				text_align();

				runtime_status = RUNTIME_STATUS_FIRSTSCREEN;

			case RUNTIME_STATUS_FIRSTSCREEN:
				paintscreen_first();
				break;

			case RUNTIME_STATUS_SECONDSCREEN:
				paintscreen_second();
				break;
		}

		if(runtime_status != RUNTIME_STATUS_IDLE)
		{
			prev_status = runtime_status;
			runtime_status = RUNTIME_STATUS_IDLE;
		}
	}

	return;
}

VOID WINAPI paintscreen_first(VOID)
{
	ShowWindow(p_text1, SW_HIDE);
	ShowWindow(p_button1, SW_HIDE);
	ShowWindow(p_button2, SW_HIDE);

	button_align();

	SendMessage(p_text1, WM_SETTEXT, 0, (LPARAM) TEXT("This is First Screen"));
	SendMessage(p_button1, WM_SETTEXT, 0, (LPARAM) TEXT("Go To Second Screen"));
	SendMessage(p_button2, WM_SETTEXT, 0, (LPARAM) TEXT("Pop Message Box"));

	ShowWindow(p_text1, SW_SHOW);
	ShowWindow(p_button1, SW_SHOW);
	ShowWindow(p_button2, SW_SHOW);

	RedrawWindow(p_mainwnd, NULL, NULL, (RDW_ERASE | RDW_FRAME | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN));
	return;
}

VOID WINAPI paintscreen_second(VOID)
{
	ShowWindow(p_text1, SW_HIDE);
	ShowWindow(p_button1, SW_HIDE);
	ShowWindow(p_button2, SW_HIDE);

	button_align();

	SendMessage(p_text1, WM_SETTEXT, 0, (LPARAM) TEXT("This is Second Screen"));
	SendMessage(p_button1, WM_SETTEXT, 0, (LPARAM) TEXT("Go To First Screen"));

	ShowWindow(p_text1, SW_SHOW);
	ShowWindow(p_button1, SW_SHOW);

	RedrawWindow(p_mainwnd, NULL, NULL, (RDW_ERASE | RDW_FRAME | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN));
	return;
}

VOID WINAPI text_choose_font(VOID)
{
	INT mainwnd_width = 0;
	INT mainwnd_height = 0;

	window_get_dimensions(p_mainwnd, NULL, NULL, &mainwnd_width, &mainwnd_height, NULL, NULL);

	if((mainwnd_width < 640) || (mainwnd_height < 480)) SendMessage(p_text1, WM_SETFONT, (WPARAM) pp_customfont[CUSTOMFONTINDEX_NORMAL], (LPARAM) TRUE);
	else SendMessage(p_text1, WM_SETFONT, (WPARAM) pp_customfont[CUSTOMFONTINDEX_LARGE], (LPARAM) TRUE);

	return;
}

VOID WINAPI text_align(VOID)
{
	INT mainwnd_width = 0;
	INT mainwnd_height = 0;

	INT text1_xpos = 0;
	INT text1_ypos = 0;
	INT text1_width = 0;
	INT text1_height = 0;

	BOOL small_wnd = FALSE;

	window_get_dimensions(p_mainwnd, NULL, NULL, &mainwnd_width, &mainwnd_height, NULL, NULL);

	small_wnd = ((mainwnd_width < 640) || (mainwnd_height < 480));

	if(small_wnd)
	{
		text1_xpos = 20;
		text1_ypos = 20;
		text1_height = CUSTOMFONT_NORMAL_HEIGHT;
	}
	else
	{
		text1_xpos = 40;
		text1_ypos = 40;
		text1_height = CUSTOMFONT_LARGE_HEIGHT;
	}

	text1_width = mainwnd_width - 2*text1_xpos;

	SetWindowPos(p_text1, NULL, text1_xpos, text1_ypos, text1_width, text1_height, 0);
	return;
}

VOID WINAPI button_align(VOID)
{
	const INT BUTTON_WIDTH = 200;
	const INT BUTTON_HEIGHT = 20;

	INT mainwnd_width = 0;
	INT mainwnd_height = 0;
	INT mainwnd_centerx = 0;

	INT margin_bottom = 0;

	INT ref_status = -1;

	INT button1_xpos = 0;
	INT button1_ypos = 0;

	INT button2_xpos = 0;
	INT button2_ypos = 0;

	BOOL small_wnd = FALSE;
	BOOL supersmall_wnd = FALSE;

	window_get_dimensions(p_mainwnd, NULL, NULL, &mainwnd_width, &mainwnd_height, &mainwnd_centerx, NULL);
	ref_status = app_get_ref_status();

	small_wnd = ((mainwnd_width < 640) || (mainwnd_height < 480));

	if(small_wnd) margin_bottom = 60;
	else margin_bottom = 100;

	switch(ref_status)
	{
		case RUNTIME_STATUS_FIRSTSCREEN:
			button2_ypos = mainwnd_height - margin_bottom - BUTTON_HEIGHT;

			if(small_wnd)
			{
				button1_ypos = button2_ypos - BUTTON_HEIGHT - 10;
				button1_xpos = mainwnd_centerx - BUTTON_WIDTH/2;
				button2_xpos = button1_xpos;
			}
			else
			{
				button1_ypos = button2_ypos;
				button1_xpos = mainwnd_centerx - BUTTON_WIDTH - 10;
				button2_xpos = mainwnd_centerx + 10;
			}
			break;

		case RUNTIME_STATUS_SECONDSCREEN:
			button1_ypos = mainwnd_height - margin_bottom - BUTTON_HEIGHT;
			button1_xpos = mainwnd_centerx - BUTTON_WIDTH/2;
			break;
	}

	SetWindowPos(p_button1, NULL, button1_xpos, button1_ypos, BUTTON_WIDTH, BUTTON_HEIGHT, 0);
	SetWindowPos(p_button2, NULL, button2_xpos, button2_ypos, BUTTON_WIDTH, BUTTON_HEIGHT, 0);
	return;
}

VOID WINAPI ctrls_setup(BOOL text_font_setup)
{
	if(text_font_setup) text_choose_font();

	text_align();
	button_align();
	return;
}

BOOL WINAPI window_get_dimensions(HWND p_wnd, INT *p_xpos, INT *p_ypos, INT *p_width, INT *p_height, INT *p_centerx, INT *p_centery)
{
	RECT rect;

	if(p_wnd == NULL) return FALSE;
	if(!GetWindowRect(p_wnd, &rect)) return FALSE;

	if(p_xpos != NULL) *p_xpos = rect.left;
	if(p_ypos != NULL) *p_ypos = rect.top;
	if(p_width != NULL) *p_width = rect.right - rect.left;
	if(p_height != NULL) *p_height = rect.bottom - rect.top;
	if(p_centerx != NULL) *p_centerx = (rect.right - rect.left)/2;
	if(p_centery != NULL) *p_centery = (rect.bottom - rect.top)/2;

	return TRUE;
}

BOOL WINAPI catch_messages(VOID)
{
	MSG msg;

	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if(msg.message == WM_QUIT) return FALSE;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return TRUE;
}

LRESULT CALLBACK mainwnd_wndproc(HWND p_wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
		case WM_COMMAND:
			return mainwnd_event_wmcommand(p_wnd, wparam, lparam);

		case WM_PAINT:
			return mainwnd_event_wmpaint(p_wnd, wparam, lparam);

		case WM_CTLCOLORSTATIC:
			return mainwnd_event_wmctlcolorstatic(p_wnd, wparam, lparam);

		case WM_SIZE:
			return mainwnd_event_wmsize(p_wnd, wparam, lparam);

		case WM_DESTROY:
			return mainwnd_event_wmdestroy(p_wnd, wparam, lparam);
	}

	return DefWindowProc(p_wnd, msg, wparam, lparam);
}

LRESULT CALLBACK mainwnd_event_wmcommand(HWND p_wnd, WPARAM wparam, LPARAM lparam)
{
	INT msgbox_result = 0;

	if(p_wnd == NULL) return 0;
	if(lparam == 0) return 0;

	if(((ULONG_PTR) lparam) == ((ULONG_PTR) p_button1))
	{
		switch(prev_status)
		{
			case RUNTIME_STATUS_FIRSTSCREEN:
				runtime_status = RUNTIME_STATUS_SECONDSCREEN;
				break;

			case RUNTIME_STATUS_SECONDSCREEN:
				runtime_status = RUNTIME_STATUS_FIRSTSCREEN;
				break;
		}
	}
	else if(((ULONG_PTR) lparam) == ((ULONG_PTR) p_button2))
	{
		msgbox_result = MessageBox(NULL, TEXT("Here is your message box..."), TEXT("Just A Message Box"), MB_YESNO);

		if(msgbox_result == IDNO) MessageBox(NULL, TEXT("YES!\r\nlol"), TEXT("Just Another Message Box"), MB_OK);
		else if(msgbox_result == IDYES) MessageBox(NULL, TEXT("NO!\r\nlol"), TEXT("Just Another Message Box"), MB_OK);
	}

	return 0;
}

LRESULT CALLBACK mainwnd_event_wmpaint(HWND p_wnd, WPARAM wparam, LPARAM lparam)
{
	HDC p_wnddc = NULL;
	HBRUSH p_brush = NULL;
	INT ref_status = -1;
	PAINTSTRUCT paintstruct;

	if(p_wnd == NULL) return 0;

	ref_status = app_get_ref_status();

	switch(ref_status)
	{
		case RUNTIME_STATUS_FIRSTSCREEN:
			p_brush = (HBRUSH) pp_custombrush[MAINWND_BRUSHINDEX_FIRSTSCREEN];
			break;

		case RUNTIME_STATUS_SECONDSCREEN:
			p_brush = (HBRUSH) pp_custombrush[MAINWND_BRUSHINDEX_SECONDSCREEN];
			break;

		default:
			p_brush = (HBRUSH) pp_custombrush[MAINWND_BRUSHINDEX_DEFAULT];
			break;
	}

	p_wnddc = BeginPaint(p_wnd, &paintstruct);
	if(p_wnddc == NULL) return 0;

	FillRect(paintstruct.hdc, &paintstruct.rcPaint, p_brush);
	EndPaint(p_wnd, &paintstruct);

	return 0;
}

LRESULT CALLBACK mainwnd_event_wmctlcolorstatic(HWND p_wnd, WPARAM wparam, LPARAM lparam)
{
	COLORREF textcolor = 0u;
	COLORREF bkcolor = 0u;
	HBRUSH p_brush = NULL;
	INT ref_status = -1;

	if(p_wnd == NULL) return 0;
	if(wparam == 0) return 0;
	if(lparam == 0) return 0;

	ref_status = app_get_ref_status();

	switch(ref_status)
	{
		case RUNTIME_STATUS_FIRSTSCREEN:
			textcolor = TEXTWND_TEXTCOLOR_FIRSTSCREEN;
			bkcolor = TEXTWND_BKCOLOR_FIRSTSCREEN;
			p_brush = (HBRUSH) pp_custombrush[TEXTWND_BRUSHINDEX_FIRSTSCREEN];
			break;

		case RUNTIME_STATUS_SECONDSCREEN:
			textcolor = TEXTWND_TEXTCOLOR_SECONDSCREEN;
			bkcolor = TEXTWND_BKCOLOR_SECONDSCREEN;
			p_brush = (HBRUSH) pp_custombrush[TEXTWND_BRUSHINDEX_SECONDSCREEN];
			break;

		default:
			textcolor = TEXTWND_TEXTCOLOR_DEFAULT;
			bkcolor = TEXTWND_BKCOLOR_DEFAULT;
			p_brush = (HBRUSH) pp_custombrush[TEXTWND_BRUSHINDEX_DEFAULT];
			break;
	}

	SetTextColor((HDC) wparam, textcolor);
	SetBkColor((HDC) wparam, bkcolor);

	return (LRESULT) p_brush;
}

LRESULT CALLBACK mainwnd_event_wmsize(HWND p_wnd, WPARAM wparam, LPARAM lparam)
{
	if(p_wnd == NULL) return 0;

	ctrls_setup(TRUE);
	RedrawWindow(p_wnd, NULL, NULL, (RDW_ERASE | RDW_FRAME | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN));

	return 0;
}

LRESULT CALLBACK mainwnd_event_wmdestroy(HWND p_wnd, WPARAM wparam, LPARAM lparam)
{
	//Closing mainwnd will invoke DestroyWindow.
	//DestroyWindow mainwnd will also destroy any child window before itself.
	//All to do here is set the pointers to NULL and post message WM_QUIT.
	//Do Not use PostMessage() to post WM_QUIT (WM_QUIT should be posted from an application context, never from a window context). Use PostQuitMessage() instead.

	p_mainwnd = NULL;
	p_text1 = NULL;
	p_button1 = NULL;
	p_button2 = NULL;

	PostQuitMessage(0);
	return 0;
}
