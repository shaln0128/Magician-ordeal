//=============================================================================
//
// �Q�[����ʏ��� [game.h]
// Author : GP11B132 36 ���c�x��
//
#pragma once

#include "main.h"

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitGame(void);
void UninitGame(void);
void UpdateGame(void);
void DrawGame(void);

bool HitCheckBB(D3DXVECTOR3 mpos, float mhw, float mhh,
	D3DXVECTOR3 ypos, float yhw, float yhh);
bool HitCheckBC(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, float size1, float size2);
bool HitCheckStage(D3DXVECTOR3 mpos);
void HitCheck(void);

void TimeLine(int time);