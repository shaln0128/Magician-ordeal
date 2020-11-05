//=============================================================================
//
// 敵のレーザー処理【enemylaser.h】
// Author : GP11B132 36 向田駿平 
////
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "fire.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ENEMY_LASER_MAX					(10)
#define PARTICLE_MAX					(30)
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemyLaser(void);
void UninitEnemyLaser(void);
void UpdateEnemyLaser(void);
void DrawEnemyLaser(void);

PARTICLE *GetEnemyLaser(void);
void SetEnemyLaser(D3DXVECTOR3 pos, DIRECTION vect);
