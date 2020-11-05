//=============================================================================
//
// ���C�W���� [player.h]
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "player.h"

#define ENEMY_MAX		(3)		// ���C�W��Max�l��


struct ENEMY
{
	bool						use;						// true:�g���Ă���  false:���g�p
	float						w, h;					// ���ƍ���
	D3DXVECTOR3					pos;						// �|���S���̍��W
	D3DXVECTOR3					move;						// �|���S���̍��W
	D3DXVECTOR3					rot;						// �|���S���̉�]��
	int							countAnim;				// �A�j���[�V�����J�E���g
	int							patternAnim;				// �A�j���[�V�����p�^�[���i���o�[
	int							texNo;					// �e�N�X�`���ԍ�

	DIRECTION					direction;//����

	float						color_R;
	float						color_G;
	float						color_B;

	float						speed;

	int						invincible_time;	//���G����

	float					rad;
	float					rotation;			//�X��

	int						hp;

	int						damage;

	int						attack_count;

	int						time;			//���炩�̎���
	bool					chase;			//�ǔ�
};
//*****************************************************************************
// �}�N����`
//*****************************************************************************

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);

