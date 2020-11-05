//=============================================================================
//
// �Q�[����ʏ��� [game.cpp]
// Author : GP11B132 36�@���c�x��
//
//=============================================================================
#include "game.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"

#include "player.h"
#include "enemy.h"
#include "ghost.h"
#include "attack.h"
#include "enemylaser.h"
#include "meteor.h"
#include "hpbar.h"
#include "score.h"
#include "time.h"
#include "ready.h"

#include "fade.h"
#include "sound.h"
#include "minimap.h"

#include "fire.h"
#include "effectcover.h"
#include "laser.h"
#include "summon.h"
#include "hiteffect.h"
#include "effect.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define pi							float(3.14159265)	//�ς�


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************



//=============================================================================
// ����������
//=============================================================================
HRESULT InitGame(void)
{
	//����������
	InitPlayer();

	InitEnemy();

	InitGhost();
	
	InitAttack();

	InitEnemyLaser();

	InitMeteor();
	
	InitScore();

	InitMiniMap();

	InitHpbar();

	InitTime();

	InitFire();

	InitEffectCover();

	InitLaser();

	InitSummon();

	InitHitEffect();

	InitEffect();
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGame(void)
{
	//�I������
	UninitScore();

	UninitAttack();

	UninitEnemyLaser();

	UninitMeteor();

	UninitEnemy();

	UninitGhost();
	
	UninitPlayer();

	UninitHpbar();

	UninitMiniMap();

	UninitTime();

	UninitFire();

	UninitEffectCover();

	UninitLaser();

	UninitSummon();

	UninitHitEffect();

	UninitEffect();
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGame(void)
{
	//�X�V����
	UpdatePlayer();

	UpdateEnemy();

	UpdateGhost();

	UpdateAttack();

	UpdateEnemyLaser();

	UpdateMeteor();

	UpdateScore();

	UpdateMiniMap();

	UpdateHpbar();

	UpdateTime();

	UpdateFire();

	UpdateLaser();

	UpdateSummon();

	UpdateHitEffect();

	UpdateEffect();

	SetViewports();	//�r���[�|�[�g���X�V
	
	HitCheck();

	TimeLine(GetTime());	//�^�C�����C���̎��s
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGame(void)
{
	DrawEffectCover();	//���p�[�e�B�N���̃J�o�[��`��@�ʒu��ς���Ə�ŕ`�悳�ꂽ���ɂ͉��Z��������Ȃ��Ȃ�

	DrawPlayer();
	
	DrawEnemy();

	DrawGhost();

	DrawAttack();

	DrawEnemyLaser();

	DrawMeteor();

	DrawEffect();

	DrawSummon();

	DrawHitEffect();

	DrawFire();

	DrawLaser();

	DrawHpbar();

	DrawMiniMap();

	DrawScore();

	DrawTime();
}


//=============================================================================
// BB�ɂ�铖���蔻�菈��
// ��]�͍l�����Ȃ�
// �߂�l�F�������Ă���true
//=============================================================================
bool HitCheckBB(D3DXVECTOR3 mpos, float mhw, float mhh,
	D3DXVECTOR3 ypos, float yhw, float yhh)
{
	bool ans = false;

	if (mpos.x + mhw > ypos.x - yhw &&
		mpos.x - mhw < ypos.x + yhw &&
		mpos.y + mhh > ypos.y - yhh &&
		mpos.y - mhh < ypos.y + yhh)
	{
		ans = true;
	}

	return ans;
}


//=============================================================================
// BB�ɂ�铖���蔻�菈��
// ��]�͍l�����Ȃ�
// �߂�l�F�������Ă���true
//=============================================================================
bool HitCheckStage(D3DXVECTOR3 mpos)	//�X�e�[�W�Ƃ̓����蔻��
{
	bool ans = false;

	if (mpos.x > 90.0f &&
		mpos.x < 870.0f &&
		mpos.y > 45.0f &&
		mpos.y < 420.0f)	//��`�Ƃ̓����蔻����N���A���Ă���΁A�ΖʂƂ̓����蔻����Ƃ�
	{
		float cross;
		float x, y;

		D3DXVECTOR2 ver[4] =	//�x�N�g���̎n�_�̒��_���
		{
			D3DXVECTOR2(90.0f,95.0f),
			D3DXVECTOR2(820.0f,45.0f),
			D3DXVECTOR2(870.0f,370.0f),
			D3DXVECTOR2(140.0f,420.0f),
		};
		D3DXVECTOR2 vec[4] =	//��r����x�N�g��
		{
			D3DXVECTOR2(50.0f,-50.0f),
			D3DXVECTOR2(50.0f,50.0f),
			D3DXVECTOR2(-50.0f,50.0f),
			D3DXVECTOR2(-50.0f,-50.0f),
		};

		for (int i = 0; i < 4; i++)	//�e�x�N�g���Ƃ̊O�ς����
		{
			x = mpos.x - ver[i].x;
			y = mpos.y - ver[i].y;

			cross = vec[i].x * y - x * vec[i].y;

			if (cross < 0)	//��ł����i�O���j�ł����false
			{
				return ans;
			}
		}
		
		ans = true;
	}

	return ans;
}

//=============================================================================
// BC�ɂ�铖���蔻�菈��
// �T�C�Y�͔��a
// �߂�l�F�������Ă���true
//=============================================================================
bool HitCheckBC(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, float size1, float size2)
{
	float len = (size1 + size2) * (size1 + size2);	// ���a��2�悵����
	D3DXVECTOR3 temp = pos1 - pos2;
	float fLengthSq = D3DXVec3LengthSq(&temp);		// 2�_�Ԃ̋����i2�悵�����j

	if (len > fLengthSq)
	{
		return true;
	}

	return false;
}


//=============================================================================
// �����蔻�菈��
//=============================================================================
void HitCheck(void)
{
	PLAYER *player = GetPlayer();
	ENEMY *enemy = GetEnemy();
	ENEMY *ghost = GetGhost();
	PARTICLE *attack = GetAttack();
	PARTICLE *laser = GetLaser();
	PARTICLE *enemylaser = GetEnemyLaser();
	int *breath_time = GetBreathTime();

	//�v���C���[�ƃS�[�X�g
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (player[i].use == false) continue;

		for (int j = 0; j < GHOST_MAX; j++)
		{
			if (ghost[j].use == false ||
				ghost[j].invincible_time > 0) continue;

			bool ans = HitCheckBB(ghost[j].pos, ghost[j].w * 0.4f, ghost[j].h * 0.4f,
				player[i].pos, player[i].w * 0.4f, player[i].h * 0.4f);

			if (ans == true)	//�����������̏���
			{
				PlaySound(SOUND_LABEL_SE_hitenemyattack);
				player[i].damage_time = 60;
				AddScore(-100,HITMINUS);
				ghost[j].use = false;
				SetEffect(ghost[j].pos, VANISH);
			}
			else if (ghost[j].chase == false)
			{
				ans = HitCheckBC(ghost[j].pos, player[0].pos, 120, 0);

				if (ans == true)	//�����͈͓��ɂ���Βǔ����n�߂�
				{
					ghost[j].chase = true;
				}
			}

		}
	}

	//�v���C���[�ƃ��[�U�[
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (player[i].use == false) continue;

		for (int j = 0; j < ENEMY_LASER_MAX; j++)
		{
			if (enemylaser[j].use == false ||
				enemylaser[j].hit == true ||
				enemylaser[j].time > 60) continue;	//���˒����܂��������Ă��Ȃ��ꍇ

			bool ans = HitCheckBB(enemylaser[j].pos, enemylaser[j].w * 0.5f, enemylaser[j].h * 0.5f,
				player[i].pos, player[i].w * 0.4f, player[i].h * 0.4f);

			if (ans == true)	//�����������̏���
			{
				PlaySound(SOUND_LABEL_SE_hitenemyattack);
				player[i].damage_time = 60;
				AddScore(-300,HITMINUS);
				enemylaser[j].hit = true;	//������������ɂ��A�Ȍセ�̃��[�U�[�Ƃ͓����蔻������Ȃ�
			}
		}
	}

	//�v���C���[�ƃX�e�[�W
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (player[i].use == false) continue;
		bool ans = HitCheckStage(player[i].pos);

		if (ans == false)
		{
			player[i].pos = player[i].prev_pos;	//���̈ʒu�ɖ߂�
		}
	}

	//�S�[�X�g�ƃX�e�[�W
	for (int i = 0; i < GHOST_MAX; i++)
	{
		if (ghost[i].use == false ||
			ghost[i].chase == true ||
			ghost[i].invincible_time > 0) continue;

		bool ans = HitCheckStage(ghost[i].pos);

		if (ans == false)
		{
			ghost[i].move *= -1;	//�ړ��ʂ𔽓]
			ghost[i].pos += ghost[i].move;	//���܂�̂�����邽�߂̏���
		}
	}

	//�S�[�X�g�ƃ��[�U�[
	for (int i = 0; i < GHOST_MAX; i++)
	{
		if (ghost[i].use == false ||
			ghost[i].invincible_time > 0) continue;
		for (int j = 0; j < LASER_MAX; j++)
		{
			if (laser[j].use == false) continue;

			bool ans = HitCheckBB(laser[j].pos, laser[j].w * 0.5f, laser[j].h * 0.5f,
				ghost[i].pos, ghost[i].w / 2, ghost[i].h / 3);

			if (ans == true)
			{
				AddScore(50,DAMAGE);
				ghost[i].hp -= 100;

				if (ghost[i].hp <= 0)	//���S���̏���
				{
					ghost[i].use = false;
					SetEffect(ghost[i].pos,VANISH);
					switch (ghost[i].texNo)	//��ނŏꍇ����
					{
					case NORMAL:
						AddScore(150, DEAD);
						break;
					case RED:
						AddScore(100, DEADRARE);
						*breath_time = BREATH_TIME_MAX;	//�Œ�l�ɕύX�@���������ł͂Ȃ�
						PlaySound(SOUND_LABEL_SE_fire);
						break;
					case BLUE:
						AddScore(100, DEADRARE);
						if (player[0].laser_stock < LASER_STOCK)	//�X�g�b�N������łȂ���΁{�P
						{
							player[0].laser_stock++;
							PlaySound(SOUND_LABEL_SE_laserbuff);
						}
						break;
					}
				}
			}
		}
	}

	//�S�[�X�g�Ɖ�
	if (*breath_time > 0)	//�������ł����
	{
		D3DXVECTOR3 pos;

		switch (player[0].direction)	//�v���C���[�̌����ɉ����ē����蔻������ʒu��ύX
		{
		case DOWN:
			pos = player[0].pos + D3DXVECTOR3(0.0f, 75.0f, 0.0f);
			break;
		case LEFT:
			pos = player[0].pos + D3DXVECTOR3(-65.0f, 0.0f, 0.0f);
			break;
		case RIGHT:
			pos = player[0].pos + D3DXVECTOR3(65.0f, 0.0f, 0.0f);
			break;
		case UP:
			pos = player[0].pos + D3DXVECTOR3(0.0f, -75.0f, 0.0f);
			break;
		}

		for (int i = 0; i < GHOST_MAX; i++)
		{
			if (ghost[i].use == false ||
				ghost[i].invincible_time > 0) continue;

			bool ans = HitCheckBC(ghost[i].pos, pos, 60, 0);

			if (ans == true)
			{
				AddScore(2,DAMAGE);
				ghost[i].hp -= 2;
				ghost[i].pos -= ghost[i].move*1.1f;	//�������Ă���Ԃ͉����Ԃ��悤�ȏ���

				if (ghost[i].hp <= 0)	//���S���̏���
				{
					ghost[i].use = false;
					SetEffect(ghost[i].pos, VANISH);
					switch (ghost[i].texNo)
					{
					case NORMAL:
						AddScore(150, DEAD);
						break;
					case RED:
						AddScore(100, DEADRARE);
						*breath_time = BREATH_TIME_MAX;
						PlaySound(SOUND_LABEL_SE_fire);
						break;
					case BLUE:
						AddScore(100, DEADRARE);
						if (player[0].laser_stock < LASER_STOCK)
						{
							player[0].laser_stock++;
							PlaySound(SOUND_LABEL_SE_laserbuff);
						}
						break;
					}
				}
			}
		}

		for (int i = 0; i < ENEMY_MAX; i++)	//���C�W�Ƃ������蔻������
		{
			if (enemy[i].use == false ||
				enemy[i].invincible_time > 0) continue;

			bool ans = HitCheckBC(enemy[i].pos, pos, 60, 0);

			if (ans == true)
			{
				AddScore(6, DAMAGE);	//����
				enemy[i].pos -= enemy[i].move*0.9f;	//�����Ԃ��͂����x���Ȃ���x
			}
		}

	}

	//�S�[�X�g�Ɛ�
	for (int i = 0; i < GHOST_MAX; i++)
	{
		if (ghost[i].use == false ||
			ghost[i].invincible_time > 0) continue;

		for (int j = 0; j < ATTACK_MAX; j++)
		{
			if (attack[j].use == false) continue;

			bool ans = HitCheckBB(attack[j].pos, attack[j].size * 0.25f, attack[j].size * 0.25f,
				ghost[i].pos, ghost[i].w / 2, ghost[i].h / 3);

			if (ans == true)
			{
				attack[j].use = false;	//���͓�����Ə���
				ghost[i].hp -= 40;
				AddScore(40, DAMAGE);
				ghost[i].speed *= 1.1f;	//�U����H�炤���т�10������������
				SetHitEffect(attack[j].pos);

				if (ghost[i].chase == true)
				{
					ghost[i].pos += -ghost[i].move * 7.0f;	//�ǔ����̓G�ɂ̓m�b�N�o�b�N
				}
				else
				{
					ghost[i].chase = true;	//�U�����ꂽ�G�͒ǔ���Ԃ�
				}

				if (ghost[i].hp <= 0)	//���񂾂Ƃ��̏���
				{
					ghost[i].use = false;
					SetEffect(ghost[i].pos, VANISH);
					switch (ghost[i].texNo)
					{
					case NORMAL:
						AddScore(150, DEAD);
						break;
					case RED:
						AddScore(100, DEADRARE);
						*breath_time = BREATH_TIME_MAX;
						PlaySound(SOUND_LABEL_SE_fire);
						break;
					case BLUE:
						AddScore(100, DEADRARE);
						if (player[0].laser_stock < LASER_STOCK)
						{
							player[0].laser_stock++;
							PlaySound(SOUND_LABEL_SE_laserbuff);
						}
						break;
					}
				}

				for (int k = 0; k < GHOST_MAX; k++)	//�����蔻�肪�������ꍇ�A���̈ʒu�Ŕ������ēx�����蔻�����鏈��
				{
					if (ghost[k].use == false ||
						k == i) continue;	//��œ������Ă��ꍇ�͓����蔻������Ȃ�

					ans = HitCheckBC(ghost[k].pos, attack[j].pos, 50, 0);

					if (ans == true)
					{
						AddScore(20,DAMAGE);
						ghost[k].hp -= 20;
						ghost[k].speed *= 1.1f;

						if (ghost[k].chase == true)
						{
							ghost[k].pos += -ghost[k].move * 7.0f;
						}
						else
						{
							ghost[k].chase = true;
						}

						if (ghost[k].hp <= 0)
						{
							ghost[k].use = false;
							SetEffect(ghost[k].pos, VANISH);
							switch (ghost[k].texNo)
							{
							case NORMAL:
								AddScore(150, DEAD);
								break;
							case RED:
								AddScore(100, DEADRARE);
								*breath_time = BREATH_TIME_MAX;
								PlaySound(SOUND_LABEL_SE_fire);
								break;
							case BLUE:
								AddScore(100, DEADRARE);
								if (player[0].laser_stock < LASER_STOCK)
								{
									player[0].laser_stock++;
									PlaySound(SOUND_LABEL_SE_laserbuff);
								}
								break;
							}
						}
					}
				}
			}

		}
	}

	//�G�l�~�[�ƃ��[�U�[
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (enemy[i].use == false ||
			enemy[i].invincible_time > 0) continue;
		for (int j = 0; j < LASER_MAX; j++)
		{
			if (laser[j].use == false ||
				laser[j].hit == true) continue;	//�܂��������Ă��Ȃ��ꍇ

			bool ans = HitCheckBB(laser[j].pos, laser[j].w * 0.5f, laser[j].h * 0.5f,
				enemy[i].pos, enemy[i].w / 2, enemy[i].h / 3);

			if (ans == true)
			{
				AddScore(150, DAMAGE);
				laser[j].hit = true;	//������������ɂ���
			}
		}
	}
	
	//�G�l�~�[�Ɛ�
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (enemy[i].use == false ||
			enemy[i].invincible_time > 0) continue;

		for (int j = 0; j < ATTACK_MAX; j++)
		{
			if (attack[j].use == false) continue;

			bool ans = HitCheckBB(attack[j].pos, attack[j].size * 0.25f, attack[j].size * 0.25f,
				enemy[i].pos, enemy[i].w / 2, enemy[i].h / 3);
			if (ans == true)
			{
				AddScore(120, DAMAGE);	//����
				attack[j].use = false;
				SetHitEffect(attack[j].pos);
			}
		}
	}

	//�G�l�~�[�ƃX�e�[�W
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (enemy[i].use == false) continue;
		bool ans = HitCheckStage(enemy[i].pos);

		if (ans == false)
		{
			enemy[i].move *= -1;	//�ړ��ʂ𔽓]
			enemy[i].pos += enemy[i].move;
		}
	}

}

void TimeLine(int time)	//���݂̎��Ԃɉ������������s���^�C�����C���@���ׂĂ����ŊǗ�����Ă���̂ł킩��₷��
{
	PLAYER *player = GetPlayer();
	ENEMY *enemy = GetEnemy();

	if (time == 3599)
	{
		SetGhost(5, 2);
	}
	if (time % 300 == 0)	//5�b�����ɃS�[�X�g���m�[�}��5�́A���A2�̐���
	{
		SetGhost(5, 2);
	}

	if (time % 360 == 0)	//6�b�����Ƀv���C���[������ʒu�Ƀ��[�U�[�𐶐�
	{
		SetEnemyLaser(D3DXVECTOR3(player[0].pos.x, SCREEN_CENTER_Y, 0.0f), DOWN);
		SetEnemyLaser(D3DXVECTOR3(SCREEN_CENTER_X, player[0].pos.y, 0.0f), LEFT);
	}

	//���e�I�̐���
	if (time == 3500)
	{
		SetMeteor(&enemy[0].pos, 3);
	}
	
	if (time == 2900)
	{
		SetMeteor(&enemy[0].pos, 3);
	}

	if (time == 2300)
	{
		SetMeteor(&enemy[0].pos, 4);
	}

	if (time == 1700)
	{
		SetMeteor(&enemy[0].pos, 6);
	}

	if (time == 1100)
	{
		SetMeteor(&enemy[0].pos, 8);
	}

	if (time == 500)
	{
		SetMeteor(&enemy[0].pos, 12);
	}
	
	if (time == 0)
	{
		SetFade(FADE_OUT, MODE_RESULT);
		RecordScore();
	}
}