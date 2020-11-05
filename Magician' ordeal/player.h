//=============================================================================
//
// �v���C���[���� [player.h]
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define PLAYER_MAX				(1)		// �v���C���[��Max�l��
#define PLAYER_FLASH			(60)
#define PLAYER_HP_MAX			(300)

//����
typedef enum
{
	DOWN,
	LEFT,
	RIGHT,
	UP,
}DIRECTION;


struct PLAYER
{
	bool						use;						// true:�g���Ă���  false:���g�p
	float						w, h;					// ���ƍ���
	D3DXVECTOR3					pos;						// �|���S���̍��W
	D3DXVECTOR3					prev_pos;						// �|���S���̍��W
	D3DXVECTOR3					rot;						// �|���S���̉�]��
	int							countAnim;				// �A�j���[�V�����J�E���g
	int							patternAnim;				// �A�j���[�V�����p�^�[���i���o�[
	int							texNo;					// �e�N�X�`���ԍ�

	DIRECTION					direction;				//����

	bool						free;

	float						color_Alpha;

	float						speed;

	float						hitbox_w;
	float						hitbox_h;

	int							flash_time;				//�U���Ԋu
	int							damage_time;			//��_�����o
	int							laser_stock;			//���[�U�[�̃X�g�b�N

	int							hp;
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);





