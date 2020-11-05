//=============================================================================
//
// メテオ処理【meteor.cpp】
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "fire.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define METEOR_MAX						(12)

//メテオの状態
typedef enum
{
	DIFFUSION,
	CIRCLING,
	FALLING,
}METEORSTATE;

struct METEOR
{
	bool			use;			// true:使っている  false:未使用
	float			w,h;			// 大きさ
	D3DXVECTOR3		pos;			// ポリゴンの座標
	D3DXVECTOR3		landingpoint;			// ポリゴンの座標
	D3DXVECTOR3		rot;			// y：周回時の角度　z：傾き
	D3DXVECTOR3		move;			// ポリゴンの回転量
	int				time;			//生存時間
	int				arrival_time;	//到着時刻
	D3DXCOLOR		color;
	METEORSTATE		state;			//状態
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitMeteor(void);
void UninitMeteor(void);
void UpdateMeteor(void);
void DrawMeteor(void);

void SetMeteor(D3DXVECTOR3 *pos,int num);
void SetMeteorParticle(D3DXVECTOR3 pos, D3DXCOLOR color);
METEOR *GetMeteor(void);


