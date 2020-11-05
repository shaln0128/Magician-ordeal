//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define PLAYER_MAX				(1)		// プレイヤーのMax人数
#define PLAYER_FLASH			(60)
#define PLAYER_HP_MAX			(300)

//向き
typedef enum
{
	DOWN,
	LEFT,
	RIGHT,
	UP,
}DIRECTION;


struct PLAYER
{
	bool						use;						// true:使っている  false:未使用
	float						w, h;					// 幅と高さ
	D3DXVECTOR3					pos;						// ポリゴンの座標
	D3DXVECTOR3					prev_pos;						// ポリゴンの座標
	D3DXVECTOR3					rot;						// ポリゴンの回転量
	int							countAnim;				// アニメーションカウント
	int							patternAnim;				// アニメーションパターンナンバー
	int							texNo;					// テクスチャ番号

	DIRECTION					direction;				//向き

	bool						free;

	float						color_Alpha;

	float						speed;

	float						hitbox_w;
	float						hitbox_h;

	int							flash_time;				//攻撃間隔
	int							damage_time;			//被ダメ演出
	int							laser_stock;			//レーザーのストック

	int							hp;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);





