
//=============================================================================
//
// 炎パーティクルの処理【fire.cpp】
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define FIRE_MAX						(250)
#define BREATH_TIME_MAX					(300)

struct PARTICLE
{
	bool			use;			// true:使っている  false:未使用
	float			size;			// 大きさ
	float			w, h;
	D3DXVECTOR3		pos;			// ポリゴンの座標
	D3DXVECTOR3		rot;			// ポリゴンの回転量
	D3DXVECTOR3		move;			// ポリゴンの回転量
	int				time;			//生存時間
	D3DXCOLOR		color;
	DIRECTION		direction;
	bool			hit;			//当たった判定
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitFire(void);
void UninitFire(void);
void UpdateFire(void);
void DrawFire(void);

int *GetBreathTime(void);
PARTICLE *GetFire(void);


