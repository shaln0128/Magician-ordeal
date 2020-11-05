//=============================================================================
//
// チュートリアル[tutorial.cpp]
// Author : GP11B132 36 向田駿平
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

#include "fade.h"
#include "sound.h"
#include "tutorialflag.h"
#include "minimap.h"

#include "fire.h"
#include "effectcover.h"
#include "laser.h"
#include "summon.h"
#include "hiteffect.h"
#include "effect.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************

//クリアフラグ
typedef enum
{
	SUCCESS,
	FAILURE,
}FLAGSTATE;

static FLAGSTATE clearflag;
static int ghost_count;	//全滅判定用
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTutorial(void)
{
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

	InitTutorialFlag();

	clearflag = SUCCESS;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTutorial(void)
{
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

	UninitTutorialFlag();
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTutorial(void)
{
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

	UpdateTutorialFlag();

	SetViewports();
	//ビューポートを更新

	HitCheckTutorial();
	//チュートリアル用の当たり判定

	TimeLineTutorial(GetTutorialTime());
	//チュートリアル用のタイムライン
#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTutorial(void)
{
	DrawEffectCover();

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

	DrawTutorialFlag();
}

void HitCheckTutorial(void)
{
	PLAYER *player = GetPlayer();
	ENEMY *enemy = GetEnemy();
	ENEMY *ghost = GetGhost();
	PARTICLE *attack = GetAttack();
	PARTICLE *laser = GetLaser();
	PARTICLE *enemylaser = GetEnemyLaser();
	int *breath_time = GetBreathTime();
	TUTORIALSTATE state = GetTutorialState();

	//プレイヤーとゴースト
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (player[i].use == false) continue;

		for (int j = 0; j < GHOST_MAX; j++)
		{
			if (ghost[j].use == false ||
				ghost[j].invincible_time > 0) continue;

			bool ans = HitCheckBB(ghost[j].pos, ghost[j].w * 0.4f, ghost[j].h * 0.4f,
				player[i].pos, player[i].w * 0.4f, player[i].h * 0.4f);

			if (ans == true)
			{
				AddScore(-100, HITMINUS);
				ghost[j].use = false;
				SetEffect(ghost[j].pos, VANISH);
				player[i].damage_time = 60;
				clearflag = FAILURE;	//当たってしまった場合クリアフラグをfalseに
			}
			else if (ghost[j].chase == false)
			{
				ans = HitCheckBC(ghost[j].pos, player[0].pos, 120, 0);

				if (ans == true)
				{
					ghost[j].chase = true;
				}
			}

		}
	}

	//プレイヤーとレーザー
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (player[i].use == false) continue;

		for (int j = 0; j < ENEMY_LASER_MAX; j++)
		{
			if (enemylaser[j].use == false ||
				enemylaser[j].hit == true ||
				enemylaser[j].time > 60) continue;

			bool ans = HitCheckBB(enemylaser[j].pos, enemylaser[j].w * 0.5f, enemylaser[j].h * 0.5f,
				player[i].pos, player[i].w * 0.4f, player[i].h * 0.4f);

			if (ans == true)
			{
				AddScore(-300, HITMINUS);
				enemylaser[j].hit = true;
				player[i].damage_time = 60;
				if (state == MAGE_SKILL)
				{
					clearflag = FAILURE;	//当たってしまった場合クリアフラグをfalseに
				}
			}
		}
	}

	//プレイヤーとステージ
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (player[i].use == false) continue;
		bool ans = HitCheckStage(player[i].pos);

		if (ans == false)
		{
			player[i].pos = player[i].prev_pos;
		}
	}

	//メイジとステージ
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (enemy[i].use == false) continue;
		bool ans = HitCheckStage(enemy[i].pos);

		if (ans == false)
		{
			enemy[i].move *= -1;
			enemy[i].pos += enemy[i].move;
		}
	}

	//ゴーストとステージ
	for (int i = 0; i < GHOST_MAX; i++)
	{
		if (ghost[i].use == false ||
			ghost[i].chase == true ||
			ghost[i].invincible_time > 0) continue;

		bool ans = HitCheckStage(ghost[i].pos);

		if (ans == false)
		{
			ghost[i].move *= -1;
			ghost[i].pos += ghost[i].move;
		}
	}

	//ゴーストとレーザー
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
				AddScore(50, DAMAGE);
				ghost[i].hp -= 100;

				if (ghost[i].hp <= 0)
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
						if (state == SPELL_RARE)	//このタイミングで赤を倒したら、敵が5体湧く
						{
							for (int j = 0; j < 5; j++)
							{
								ghost[j].speed = GHOST_NORMAL_SPEED;
								ghost[j].hp = GHOST_NORMAL_HP_MAX;
								ghost[j].texNo = NORMAL;
								ghost[j].use = true;
								ghost[j].chase = true;
								ghost[j].direction = DOWN;
								ghost[j].pos = D3DXVECTOR3(600.0f, 180.0f + 45.0f * j, 0.0f);
								ghost[j].invincible_time = GHOST_INVINCIBLE_TIME;
								SetSummon(ghost[j].pos, ghost[j].texNo);
							}
						}
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

	//ゴーストと炎
	if (*breath_time > 0)
	{
		D3DXVECTOR3 pos;

		switch (player[0].direction)
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
				AddScore(2, DAMAGE);
				ghost[i].hp -= 2;
				ghost[i].pos -= ghost[i].move*0.9f;

				if (ghost[i].hp <= 0)
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

		for (int i = 0; i < ENEMY_MAX; i++)
		{
			if (enemy[i].use == false ||
				enemy[i].invincible_time > 0) continue;

			bool ans = HitCheckBC(enemy[i].pos, pos, 60, 0);

			if (ans == true)
			{
				AddScore(6, DAMAGE);
				enemy[i].pos -= enemy[i].move*0.9f;
			}
		}

	}

	//ゴーストと星
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
				attack[j].use = false;
				ghost[i].hp -= 40;
				ghost[i].speed *= 1.1f;
				SetHitEffect(attack[j].pos);

				if (ghost[i].chase == true)
				{
					ghost[i].pos += -ghost[i].move * 7.0f;
				}
				else
				{
					ghost[i].chase = true;
				}

				if (ghost[i].hp <= 0)
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
						if (state == SPELL_RARE)	//このタイミングで赤を倒したら、敵が5体湧く
						{
							for (int k = 0; k < 5; k++)
							{
								ghost[k].speed = GHOST_NORMAL_SPEED;
								ghost[k].hp = GHOST_NORMAL_HP_MAX;
								ghost[k].texNo = NORMAL;
								ghost[k].use = true;
								ghost[k].chase = true;
								ghost[k].direction = DOWN;
								ghost[k].pos = D3DXVECTOR3(600.0f, 230.0f + 20.0f * k, 0.0f);
								ghost[k].invincible_time = GHOST_INVINCIBLE_TIME;
								SetSummon(ghost[k].pos, ghost[k].texNo);
							}
						}
						break;
					case BLUE:
						AddScore(100, DEADRARE);
						if (player[0].laser_stock < LASER_STOCK)
						{
							player[0].laser_stock++;
							PlaySound(SOUND_LABEL_SE_laserbuff);
						}
						if (state == SPELL_RARE)	//このタイミングで青を倒したら、赤が湧く
						{
							ghost[0].speed = GHOST_RARE_SPEED;
							ghost[0].hp = GHOST_RARE_HP_MAX;
							ghost[0].texNo = RED;
							ghost[0].use = true;
							ghost[0].chase = true;
							ghost[0].direction = DOWN;
							ghost[0].pos = D3DXVECTOR3(600.0f, 270.0f, 0.0f);
							ghost[0].invincible_time = GHOST_INVINCIBLE_TIME;
							SetSummon(ghost[0].pos, ghost[0].texNo);
						}
						break;
					}
				}

				for (int k = 0; k < GHOST_MAX; k++)
				{
					if (ghost[k].use == false ||
						k == i) continue;

					ans = HitCheckBC(ghost[k].pos, attack[j].pos, 50, 0);

					if (ans == true)
					{
						AddScore(20, DAMAGE);
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

	//エネミーとレーザー
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (enemy[i].use == false ||
			enemy[i].invincible_time > 0) continue;
		for (int j = 0; j < LASER_MAX; j++)
		{
			if (laser[j].use == false ||
				laser[j].hit == true) continue;

			bool ans = HitCheckBB(laser[j].pos, laser[j].w * 0.5f, laser[j].h * 0.5f,
				enemy[i].pos, enemy[i].w / 2, enemy[i].h / 3);

			if (ans == true)
			{
				AddScore(150, DAMAGE);
				laser[j].hit = true;
			}
		}
	}

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
				AddScore(120, DAMAGE);
				attack[j].use = false;
				SetHitEffect(attack[j].pos);
			}
		}
	}

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (enemy[i].use == false) continue;
		bool ans = HitCheckStage(enemy[i].pos);

		if (ans == false)
		{
			enemy[i].move *= -1;
			enemy[i].pos += enemy[i].move;
		}
	}

	int count = 0;

	for (int i = 0; i < GHOST_MAX; i++)
	{
		if (ghost[i].use == true)
		{
			count++;
		}
	}

	ghost_count = count;	//残っている敵の数を格納

}

void TimeLineTutorial(int time)
{
	PLAYER *player = GetPlayer();
	ENEMY *enemy = GetEnemy();
	ENEMY *ghost = GetGhost();
	PARTICLE *attack = GetAttack();
	PARTICLE *laser = GetLaser();
	PARTICLE *enemylaser = GetEnemyLaser();
	int *breath_time = GetBreathTime();
	TUTORIALSTATE state = GetTutorialState();
	bool ans;

	switch (state)
	{
	case CONVERSION:	//移行中はEnterでスキップ
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			SkipTime();
		}
		break;
	case WASD_CAMERA:	//Tutorial1
		ans = HitCheckBC(enemy[0].pos, player[0].pos,enemy[0].h / 2.0f, player[0].h / 2.0f);
		if (ans == true)
		{
			ConversionTutorial(ATTACK_MOB, true);	//所定の場所にいくと、次に移行
		}
		break;
	case ATTACK_MOB:	//Tutorial2
		if (time == 180)
		{
			for (int i = 0; i < 3; i++)	//3秒後に敵が3体湧く
			{
				ghost[i].speed = GHOST_NORMAL_SPEED;
				ghost[i].hp = GHOST_NORMAL_HP_MAX;
				ghost[i].texNo = NORMAL;
				ghost[i].use = true;
				ghost[i].chase = true;
				ghost[i].direction = DOWN;
				ghost[i].pos = D3DXVECTOR3(600.0f, 180.0f + 90.0f * i, 0.0f);
				ghost[i].invincible_time = GHOST_INVINCIBLE_TIME;
				SetSummon(ghost[i].pos, ghost[i].texNo);
			}
		}

		if (clearflag == FAILURE)	//クリアフラグがfalseならもう一度
		{
			clearflag = SUCCESS;
			ConversionTutorial(ATTACK_MOB, false);
			for (int i = 0; i < GHOST_MAX; i++)
			{
				ghost[i].use = false;
			}
		}
		else if (time > 190 && ghost_count == 0)
		{ 
			clearflag = SUCCESS;
			ConversionTutorial(SPELL_RARE, true);
		}

		break;
	case SPELL_RARE:	//Tutorial3
		if (time == 180)	//3秒後に青が湧く
		{
			ghost[0].speed = GHOST_RARE_SPEED;
			ghost[0].hp = GHOST_RARE_HP_MAX;
			ghost[0].texNo = BLUE;
			ghost[0].use = true;
			ghost[0].chase = true;
			ghost[0].direction = DOWN;
			ghost[0].pos = D3DXVECTOR3(600.0f, 270.0f, 0.0f);
			ghost[0].invincible_time = GHOST_INVINCIBLE_TIME;
			SetSummon(ghost[0].pos, ghost[0].texNo);	
		}

		if (clearflag == FAILURE)	//クリアフラグがfalseならもう一度
		{
			clearflag = SUCCESS;
			player[0].laser_stock = 0;
			ConversionTutorial(SPELL_RARE, false);
			for (int i = 0; i < GHOST_MAX; i++)
			{
				ghost[i].use = false;
			}
		}
		else if (time > 190 && ghost_count == 0)	//敵がすべて倒されると次に移行
		{
			clearflag = SUCCESS;
			ConversionTutorial(MAGE_SKILL, true);
			enemy[0].pos = D3DXVECTOR3(600.0f, 270.0f, 0.0f);
		}

		break;
	case MAGE_SKILL:	//Tutorial4

		if (time == 180)	//3秒後にメテオ発射
		{
			SetMeteor(&enemy[0].pos, 3);
		}
		else if (time == 480)	//8秒後にレーザー発射
		{
			SetEnemyLaser(D3DXVECTOR3(player[0].pos.x, SCREEN_CENTER_Y, 0.0f), DOWN);
			SetEnemyLaser(D3DXVECTOR3(SCREEN_CENTER_X, player[0].pos.y, 0.0f), LEFT);
		}
		else if (time > 720)	//12秒後まで生き延びたら、次に移行
		{
			clearflag = SUCCESS;
			ConversionTutorial(FREE_PRACTICE, true);
		}

		if (clearflag == FAILURE)	//クリアフラグがfalseならもう一度
		{
			clearflag = SUCCESS;
			ConversionTutorial(MAGE_SKILL, false);
		}
		break;
	case FREE_PRACTICE:
		if (GetKeyboardTrigger(DIK_RETURN))	//Enterでゲームに移行
		{
			SetFade(FADE_OUT, MODE_GAME);
		}

		//以下自動生成
		if (time % 600 == 240)
		{
			SetMeteor(&enemy[0].pos, 4);
		}

		if (time % 480 == 180)
		{
			SetEnemyLaser(D3DXVECTOR3(player[0].pos.x, SCREEN_CENTER_Y, 0.0f), DOWN);
			SetEnemyLaser(D3DXVECTOR3(SCREEN_CENTER_X, player[0].pos.y, 0.0f), LEFT);
		}

		if (time % 300 == 60)
		{
			SetGhost(4, 1);
		}

		break;
	}
}

void SetFailure(void)	//クリアフラグをfalseに
{
	clearflag = FAILURE;
}
