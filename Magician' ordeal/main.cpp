//=============================================================================
//
// Main処理 [main.cpp]
// Author :  GP11B132 36 向田駿平
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "bg.h"
#include "sound.h"
#include "title.h"
#include "modeselect.h"
#include "ready.h"
#include "game.h"
#include "fade.h"
#include "result.h"
#include "tutorial.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME			"AppClass"				// ウインドウのクラス名
#define WINDOW_NAME			"Magician' ordeal"		// ウインドウのキャプション名

//*****************************************************************************
// 構造体定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

//*****************************************************************************
// グローバル変数:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;

MODE g_Mode = MODE_TITLE;

#ifdef _DEBUG
int		g_CountFPS;							// FPSカウンタ
char	g_DebugStr[2048] = WINDOW_NAME;		// デバッグ文字表示用

#endif

											//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	//UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	// 時間計測用
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;
	
	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW+1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;
	
	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindow(CLASS_NAME,
						WINDOW_NAME,
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT,																		// ウィンドウの左座標
						CW_USEDEFAULT,																		// ウィンドウの上座標
						SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME)*2,									// ウィンドウ横幅
						SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME)*2+GetSystemMetrics(SM_CYCAPTION),	// ウィンドウ縦幅
						NULL,
						NULL,
						hInstance,
						NULL);

	bool mode = true;

	int id = MessageBox(NULL, "Windowモードでプレイしますか？", "起動モード", MB_YESNOCANCEL | MB_ICONQUESTION);
	switch (id)
	{
	case IDYES:		// YesならWindowモードで起動
		mode = true;
		break;
	case IDNO:		// Noならフルスクリーンモードで起動
		mode = false;
		break;
	case IDCANCEL:	// CANCELなら終了
	default:
		return -1;
		break;
	}

	// DirectXの初期化(ウィンドウを作成してから行う)
	if(FAILED(Init(hInstance, hWnd, true)))
	{
		return -1;
	}

	// フレームカウント初期化
	timeBeginPeriod(1);	// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// システム時刻をミリ秒単位で取得
	dwCurrentTime = dwFrameCount = 0;
	
	// ウインドウの表示(Init()の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// メッセージループ
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳とディスパッチ
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();					// システム時刻を取得

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1秒ごとに実行
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPSを測定した時刻を保存
				dwFrameCount = 0;							// カウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60秒ごとに実行
			{
				dwExecLastTime = dwCurrentTime;	// 処理した時刻を保存

#ifdef _DEBUG	// デバッグ版の時だけFPSを表示する
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// 更新処理
				Draw();				// 描画処理

#ifdef _DEBUG	// デバッグ版の時だけ表示する
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;		// 処理回数のカウントを加算
			}
		}
	}
	
	timeEndPeriod(1);				// 分解能を戻す

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:					// [ESC]キーが押された
			DestroyWindow(hWnd);		// ウィンドウを破棄するよう指示する
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// レンダリング処理の初期化
	InitRenderer(hInstance, hWnd, bWindow);

	InitFade();
	// カメラ処理の初期化
	InitCamera();

	InitBg();

	//入力の初期化
	InitInput(hInstance, hWnd);

	InitSound(hWnd);
	
	SetMode(g_Mode);

	return S_OK;
}


//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	SetMode(MODE_MAX);
	//入力の終了処理
	UninitInput();

	UninitFade();

	UninitReady();

	// カメラの終了処理
	UninitCamera();

	UninitBg();

	// レンダリングの終了処理
	UninitRenderer();

	UninitSound();
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	//入力の更新処理
	UpdateInput();
	// カメラの更新処理
	UpdateCamera();

	UpdateBg();

	if (GetFade() != FADE_OUT)
	{
		switch (g_Mode)
		{
		case MODE_TITLE:
			UpdateTitle();
			break;

		case MODE_SELECT:
			UpdateModeSelect();
			break;

		case MODE_TUTORIAL:
			UpdateTutorial();
			break;

		case MODE_GAME:
			if (AreYouReady())
			{
				UpdateReady();
			}
			else
			{
				UpdateReady();
				UpdateGame();
			}

			break;

		case MODE_RESULT:
			UpdateResult();
			break;

		}

	}
	
	UpdateFade();

}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	// バックバッファクリア
	Clear();

	// カメラをセット
	SetCamera();

	// 2D描画なので深度無効
	SetDepthEnable(false);

	DrawBg();

	switch (g_Mode)
	{
	case MODE_TITLE:
		DrawTitle();
		break;

	case MODE_SELECT:
		DrawModeSelect();
		break;

	case MODE_TUTORIAL:
		DrawTutorial();
		break;

	case MODE_GAME:
		DrawGame();
		DrawReady();
		break;

	case MODE_RESULT:
		DrawResult();
		break;
	}

	

	DrawFade();
	// バックバッファ、フロントバッファ入れ替え
	Present();

}


long GetMousePosX(void)
{
	return g_MouseX;
}


long GetMousePosY(void)
{
	return g_MouseY;
}


#ifdef _DEBUG
char *GetDebugStr(void)
{
	return g_DebugStr;
}
#endif


void SetMode(MODE mode)
{
	// モードを変える前に全部メモリを解放しちゃう

	// タイトル画面の終了処理
	UninitTitle();

	UninitModeSelect();

	// ゲーム画面の終了処理
	UninitGame();

	// リザルト画面の終了処理
	UninitResult();

	UninitTutorial();

	ResetViewports();

	g_Mode = mode;	// 次のモードをセットしている

	switch (g_Mode)
	{
	case MODE_TITLE:
		// タイトル画面の初期化
		InitTitle();
		PlaySound(SOUND_LABEL_BGM_title);
		break;

	case MODE_SELECT:
		InitModeSelect();
		PlaySound(SOUND_LABEL_BGM_title);
		break;

	case MODE_TUTORIAL:
		InitTutorial();
		PlaySound(SOUND_LABEL_BGM_tutorial);
		break;

	case MODE_GAME:
		// ゲーム画面の初期化
		InitGame();
		InitReady();
		PlaySound(SOUND_LABEL_BGM_game);
		break;

	case MODE_RESULT:
		// リザルト画面の初期化
		InitResult();
		PlaySound(SOUND_LABEL_BGM_title);
		break;

		// ゲーム終了時の処理
	case MODE_MAX:
		break;
	}

	SetBg(g_Mode);
}

//=============================================================================
// モードの取得
//=============================================================================
MODE GetMode(void)
{
	return g_Mode;
}


