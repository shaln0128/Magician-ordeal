//=============================================================================
//
// メイジ処理 [player.h]
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "player.h"

#define ENEMY_MAX		(3)		// メイジのMax人数


struct ENEMY
{
	bool						use;						// true:使っている  false:未使用
	float						w, h;					// 幅と高さ
	D3DXVECTOR3					pos;						// ポリゴンの座標
	D3DXVECTOR3					move;						// ポリゴンの座標
	D3DXVECTOR3					rot;						// ポリゴンの回転量
	int							countAnim;				// アニメーションカウント
	int							patternAnim;				// アニメーションパターンナンバー
	int							texNo;					// テクスチャ番号

	DIRECTION					direction;//向き

	float						color_R;
	float						color_G;
	float						color_B;

	float						speed;

	int						invincible_time;	//無敵時間

	float					rad;
	float					rotation;			//傾き

	int						hp;

	int						damage;

	int						attack_count;

	int						time;			//何らかの時間
	bool					chase;			//追尾
};
//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);

