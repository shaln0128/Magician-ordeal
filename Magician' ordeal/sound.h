//=============================================================================
//
// �T�E���h���� [sound.h]
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

#include <windows.h>
#include "xaudio2.h"						// �T�E���h�����ŕK�v

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_title,				// �^�C�g��
	SOUND_LABEL_BGM_tutorial,			// �`���[�g���A��
	SOUND_LABEL_BGM_game,				// �Q�[��
	SOUND_LABEL_SE_decision,			// ����
	SOUND_LABEL_SE_pause,				// �|�[�Y
	SOUND_LABEL_SE_switch,	 			// �؂�ւ�
	SOUND_LABEL_SE_drumroll,			// �h�������[��
	SOUND_LABEL_SE_scoredecision,		// �X�R�A����
	SOUND_LABEL_SE_rankdecision,		// �����N����
	SOUND_LABEL_SE_rankindecision,		// �����N�C��
	SOUND_LABEL_SE_laserbuff,			// ���[�U�[�o�t�l��
	SOUND_LABEL_SE_attack,				// �U��
	SOUND_LABEL_SE_fire,				// ��
	SOUND_LABEL_SE_enemylasercharge,	// �G�̃��[�U�[�`���[�W
	SOUND_LABEL_SE_hitattack, 			// �U���q�b�g
	SOUND_LABEL_SE_hitenemyattack,		// �G�̍U���q�b�g
	SOUND_LABEL_SE_hitmeteor,			// ���e�I���e
	SOUND_LABEL_SE_laser,				// ���[�U�[
	SOUND_LABEL_SE_laser2,				// ���[�U�[�Q
	SOUND_LABEL_SE_vanish,				// �G����
	SOUND_LABEL_SE_success,				// ����
	SOUND_LABEL_SE_failure,		 		// ���s
	SOUND_LABEL_SE_throwing,			// ���e�I����
	SOUND_LABEL_SE_ghost,				// �S�[�X�g�o��
	SOUND_LABEL_SE_meteorcharge,		// ���e�I�r��
	SOUND_LABEL_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);
void SetSoundVolume(int n, int g);
#endif
