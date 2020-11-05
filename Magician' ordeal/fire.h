
//=============================================================================
//
// ���p�[�e�B�N���̏����yfire.cpp�z
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define FIRE_MAX						(250)
#define BREATH_TIME_MAX					(300)

struct PARTICLE
{
	bool			use;			// true:�g���Ă���  false:���g�p
	float			size;			// �傫��
	float			w, h;
	D3DXVECTOR3		pos;			// �|���S���̍��W
	D3DXVECTOR3		rot;			// �|���S���̉�]��
	D3DXVECTOR3		move;			// �|���S���̉�]��
	int				time;			//��������
	D3DXCOLOR		color;
	DIRECTION		direction;
	bool			hit;			//������������
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitFire(void);
void UninitFire(void);
void UpdateFire(void);
void DrawFire(void);

int *GetBreathTime(void);
PARTICLE *GetFire(void);


