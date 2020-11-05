
//=============================================================================
//
// スコア処理 [score.cpp]
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SCORE_DIGIT_MAX					(6)
#define SCORE_RANK_MAX					(5)

//スコアデータの種類
typedef enum
{
	DEAD,
	DEADRARE,
	DAMAGE,
	HITMINUS,
	TOTALSCORE,
	TYPEMAX,
}SCORETYPE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitScore(void);
void UninitScore(void);
void UpdateScore(void);
void DrawScore(void);

void AddScore(int n,SCORETYPE type);
void RecordScore(void);
int *GetScoreRank(void);
int *GetScoreData(void);
void SetScorePos(void);
