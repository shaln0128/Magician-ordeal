#pragma once
//=============================================================================
//
// effect�����yeffect.cpp�z
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define EFFECT_MAX					(20)

//�G�t�F�N�g�̎��
typedef enum
{
	VANISH,
	SHOT,
	HIT,
	EFFECTTYPE_MAX,
}EFFECTTYPE;
struct EFFECT
{
	bool			use;			// true:�g���Ă���  false:���g�p
	float			w, h;
	D3DXVECTOR3		pos;			// �|���S���̍��W
	D3DXVECTOR3		rot;			// �|���S���̉�]��
	D3DXVECTOR3		move;			// �|���S���̉�]��
	int				time;			//��������
	int				countAnim;				// �A�j���[�V�����J�E���g
	int				patternAnim;				// �A�j���[�V�����p�^�[���i���o�[
	int				texNo;
	D3DXCOLOR		color;
	EFFECTTYPE		type;			//���
};
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEffect(void);
void UninitEffect(void);
void UpdateEffect(void);
void DrawEffect(void);

EFFECT *GetEffect(void);
void SetEffect(D3DXVECTOR3 pos, EFFECTTYPE type);


