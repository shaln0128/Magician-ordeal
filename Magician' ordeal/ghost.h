//=============================================================================
//
// ゴースト処理 [ghost.h]
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "enemy.h"

#define GHOST_MAX		(300)		// ゴーストのMax人数
#define GHOST_NORMAL_HP_MAX	(60)
#define GHOST_RARE_HP_MAX	(100)

#define GHOST_NORMAL_SPEED			(0.7f)
#define GHOST_RARE_SPEED			(1.2f)
#define GHOST_INVINCIBLE_TIME		(60)
//*****************************************************************************
// マクロ定義
//*****************************************************************************

//ゴーストの種類
enum
{
	NORMAL,
	RED,
	BLUE,
};
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitGhost(void);
void UninitGhost(void);
void UpdateGhost(void);
void DrawGhost(void);

ENEMY *GetGhost(void);
void SetGhost(int normal, int rare);