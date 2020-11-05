//=============================================================================
//
// Mainヘッダー [main.h]
// Author : 向田駿平
//
//=============================================================================
#pragma once


#pragma warning(push)
#pragma warning(disable:4005)

#define _CRT_SECURE_NO_WARNINGS			// scanf のwarning防止
#include <stdio.h>
#include <time.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <d3dx9.h>

#include <D2d1.h>
#include <Dwrite.h>

#define DIRECTINPUT_VERSION 0x0800		// 警告対処
#include "dinput.h"
#include "mmsystem.h"

#pragma warning(pop)


//*****************************************************************************
// ライブラリのリンク
//*****************************************************************************
#pragma comment (lib, "d3d11.lib")		
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "d3dx11.lib")	
#pragma comment (lib, "d3dx9.lib")	
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dxerr.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dinput8.lib")

//*****************************************************************************
// 画面の情報
//*****************************************************************************
typedef enum
{
	MODE_TITLE,
	MODE_SELECT,
	MODE_TUTORIAL,
	MODE_GAME,
	MODE_RESULT,
	MODE_MAX
} MODE;

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SCREEN_WIDTH	(960)				// ウインドウの幅
#define SCREEN_HEIGHT	(540)				// ウインドウの高さ
#define SCREEN_CENTER_X	(SCREEN_WIDTH / 2)	// ウインドウの中心Ｘ座標
#define SCREEN_CENTER_Y	(SCREEN_HEIGHT / 2)	// ウインドウの中心Ｙ座標

#define	NUM_VERTEX		(4)					// 頂点数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
long GetMousePosX(void);
long GetMousePosY(void);
char *GetDebugStr(void);

void SetMode(MODE mode);
MODE GetMode(void);