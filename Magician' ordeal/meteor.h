//=============================================================================
//
// ���e�I�����ymeteor.cpp�z
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "fire.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define METEOR_MAX						(12)

//���e�I�̏��
typedef enum
{
	DIFFUSION,
	CIRCLING,
	FALLING,
}METEORSTATE;

struct METEOR
{
	bool			use;			// true:�g���Ă���  false:���g�p
	float			w,h;			// �傫��
	D3DXVECTOR3		pos;			// �|���S���̍��W
	D3DXVECTOR3		landingpoint;			// �|���S���̍��W
	D3DXVECTOR3		rot;			// y�F���񎞂̊p�x�@z�F�X��
	D3DXVECTOR3		move;			// �|���S���̉�]��
	int				time;			//��������
	int				arrival_time;	//��������
	D3DXCOLOR		color;
	METEORSTATE		state;			//���
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitMeteor(void);
void UninitMeteor(void);
void UpdateMeteor(void);
void DrawMeteor(void);

void SetMeteor(D3DXVECTOR3 *pos,int num);
void SetMeteorParticle(D3DXVECTOR3 pos, D3DXCOLOR color);
METEOR *GetMeteor(void);


