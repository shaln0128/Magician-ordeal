
//=============================================================================
//
// �X�R�A���� [score.cpp]
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define SCORE_DIGIT_MAX					(6)
#define SCORE_RANK_MAX					(5)

//�X�R�A�f�[�^�̎��
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
// �v���g�^�C�v�錾
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
