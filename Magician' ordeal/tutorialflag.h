//=============================================================================
//
//�`���[�g���A���t���O���� [tutorialflag.cpp]
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

//�`���[�g���A���̏��
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
// �v���g�^�C�v�錾
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