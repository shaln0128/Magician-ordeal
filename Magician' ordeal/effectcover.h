//=============================================================================
//
// 背景に炎パーティクルを加算合成させないためのカバー【effectcover.h】
// Author :		GP11B132 36　向田駿平
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
HRESULT InitEffectCover(void);
void UninitEffectCover(void);
void UpdateEffectCover(void);
void DrawEffectCover(void);
