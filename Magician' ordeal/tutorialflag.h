//=============================================================================
//
//チュートリアルフラグ処理 [tutorialflag.cpp]
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//チュートリアルの状態
typedef enum
{
	WASD_CAMERA,
	ATTACK_MOB,
	SPELL_RARE,
	MAGE_SKILL,
	FREE_PRACTICE,
	CONVERSION, 
}TUTORIALSTATE;
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitTutorialFlag(void);
void UninitTutorialFlag(void);
void UpdateTutorialFlag(void);
void DrawTutorialFlag(void);
void SetTutorialFlagPos(void);

void HitCheckTutorial(void);
void TimeLineTutorial(int time);
TUTORIALSTATE GetTutorialState(void);
void ConversionTutorial(TUTORIALSTATE state, bool flag);
int GetTutorialTime(void);

void SkipTime(void);