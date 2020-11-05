//=============================================================================
//
// ���[�U�[�̏����ylaser.cpp�z
// Author :GP11B132 36  ���c�x��
////
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "fire.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define LASER_MAX					(4)
#define LASER_STOCK					(4)
#define PARTICLE_MAX					(30)
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitLaser(void);
void UninitLaser(void);
void UpdateLaser(void);
void DrawLaser(void);

PARTICLE *GetLaser(void);
void SetLaser(D3DXVECTOR3 pos,DIRECTION vect);