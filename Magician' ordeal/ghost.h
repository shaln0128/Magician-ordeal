//=============================================================================
//
// �S�[�X�g���� [ghost.h]
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "enemy.h"

#define GHOST_MAX		(300)		// �S�[�X�g��Max�l��
#define GHOST_NORMAL_HP_MAX	(60)
#define GHOST_RARE_HP_MAX	(100)

#define GHOST_NORMAL_SPEED			(0.7f)
#define GHOST_RARE_SPEED			(1.2f)
#define GHOST_INVINCIBLE_TIME		(60)
//*****************************************************************************
// �}�N����`
//*****************************************************************************

//�S�[�X�g�̎��
enum
{
	NORMAL,
	RED,
	BLUE,
};
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitGhost(void);
void UninitGhost(void);
void UpdateGhost(void);
void DrawGhost(void);

ENEMY *GetGhost(void);
void SetGhost(int normal, int rare);