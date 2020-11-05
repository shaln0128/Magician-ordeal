#pragma once
//=============================================================================
//
// effect処理【effect.cpp】
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define EFFECT_MAX					(20)

//エフェクトの種類
typedef enum
{
	VANISH,
	SHOT,
	HIT,
	EFFECTTYPE_MAX,
}EFFECTTYPE;
struct EFFECT
{
	bool			use;			// true:使っている  false:未使用
	float			w, h;
	D3DXVECTOR3		pos;			// ポリゴンの座標
	D3DXVECTOR3		rot;			// ポリゴンの回転量
	D3DXVECTOR3		move;			// ポリゴンの回転量
	int				time;			//生存時間
	int				countAnim;				// アニメーションカウント
	int				patternAnim;				// アニメーションパターンナンバー
	int				texNo;
	D3DXCOLOR		color;
	EFFECTTYPE		type;			//種類
};
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEffect(void);
void UninitEffect(void);
void UpdateEffect(void);
void DrawEffect(void);

EFFECT *GetEffect(void);
void SetEffect(D3DXVECTOR3 pos, EFFECTTYPE type);


