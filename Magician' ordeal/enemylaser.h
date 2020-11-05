//=============================================================================
//
// �G�̃��[�U�[�����yenemylaser.h�z
// Author : GP11B132 36 ���c�x�� 
////
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "fire.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define ENEMY_LASER_MAX					(10)
#define PARTICLE_MAX					(30)
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemyLaser(void);
void UninitEnemyLaser(void);
void UpdateEnemyLaser(void);
void DrawEnemyLaser(void);

PARTICLE *GetEnemyLaser(void);
void SetEnemyLaser(D3DXVECTOR3 pos, DIRECTION vect);
