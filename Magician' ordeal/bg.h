//=============================================================================
//
// 背景処理 [bg.h]
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************




//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBg(void);
void UninitBg(void);
void UpdateBg(void);
void DrawBg(void);

void SetBg(MODE label);
