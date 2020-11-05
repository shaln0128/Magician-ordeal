//=============================================================================
//
// ゲーム画面処理 [game.cpp]
// Author : GP11B132 36　向田駿平
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
// マクロ定義
//*****************************************************************************
#define pi							float(3.14159265)	//ぱい


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGame(void)
{
	//初期化処理
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
// 終了処理
//=============================================================================
void UninitGame(void)
{
	//終了処理
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
// 更新処理
//=============================================================================
void UpdateGame(void)
{
	//更新処理
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

	SetViewports();	//ビューポートを更新
	
	HitCheck();

	TimeLine(GetTime());	//タイムラインの実行
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGame(void)
{
	DrawEffectCover();	//炎パーティクルのカバーを描画　位置を変えると上で描画された物には加算合成されなくなる

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
// BBによる当たり判定処理
// 回転は考慮しない
// 戻り値：当たってたらtrue
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
// BBによる当たり判定処理
// 回転は考慮しない
// 戻り値：当たってたらtrue
//=============================================================================
bool HitCheckStage(D3DXVECTOR3 mpos)	//ステージとの当たり判定
{
	bool ans = false;

	if (mpos.x > 90.0f &&
		mpos.x < 870.0f &&
		mpos.y > 45.0f &&
		mpos.y < 420.0f)	//矩形との当たり判定をクリアしていれば、斜面との当たり判定をとる
	{
		float cross;
		float x, y;

		D3DXVECTOR2 ver[4] =	//ベクトルの始点の頂点情報
		{
			D3DXVECTOR2(90.0f,95.0f),
			D3DXVECTOR2(820.0f,45.0f),
			D3DXVECTOR2(870.0f,370.0f),
			D3DXVECTOR2(140.0f,420.0f),
		};
		D3DXVECTOR2 vec[4] =	//比較するベクトル
		{
			D3DXVECTOR2(50.0f,-50.0f),
			D3DXVECTOR2(50.0f,50.0f),
			D3DXVECTOR2(-50.0f,50.0f),
			D3DXVECTOR2(-50.0f,-50.0f),
		};

		for (int i = 0; i < 4; i++)	//各ベクトルとの外積を取る
		{
			x = mpos.x - ver[i].x;
			y = mpos.y - ver[i].y;

			cross = vec[i].x * y - x * vec[i].y;

			if (cross < 0)	//一つでも負（外側）であればfalse
			{
				return ans;
			}
		}
		
		ans = true;
	}

	return ans;
}

//=============================================================================
// BCによる当たり判定処理
// サイズは半径
// 戻り値：当たってたらtrue
//=============================================================================
bool HitCheckBC(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, float size1, float size2)
{
	float len = (size1 + size2) * (size1 + size2);	// 半径を2乗した物
	D3DXVECTOR3 temp = pos1 - pos2;
	float fLengthSq = D3DXVec3LengthSq(&temp);		// 2点間の距離（2乗した物）

	if (len > fLengthSq)
	{
		return true;
	}

	return false;
}


//=============================================================================
// 当たり判定処理
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

			if (ans == true)	//当たった時の処理
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

				if (ans == true)	//もし範囲内にいれば追尾を始める
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
				enemylaser[j].time > 60) continue;	//発射中かつまだ当たっていない場合

			bool ans = HitCheckBB(enemylaser[j].pos, enemylaser[j].w * 0.5f, enemylaser[j].h * 0.5f,
				player[i].pos, player[i].w * 0.4f, player[i].h * 0.4f);

			if (ans == true)	//当たった時の処理
			{
				PlaySound(SOUND_LABEL_SE_hitenemyattack);
				player[i].damage_time = 60;
				AddScore(-300,HITMINUS);
				enemylaser[j].hit = true;	//当たった判定にし、以後そのレーザーとは当たり判定を取らない
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
			player[i].pos = player[i].prev_pos;	//元の位置に戻す
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
			ghost[i].move *= -1;	//移動量を反転
			ghost[i].pos += ghost[i].move;	//挟まるのを避けるための処理
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
				AddScore(50,DAMAGE);
				ghost[i].hp -= 100;

				if (ghost[i].hp <= 0)	//死亡時の処理
				{
					ghost[i].use = false;
					SetEffect(ghost[i].pos,VANISH);
					switch (ghost[i].texNo)	//種類で場合分け
					{
					case NORMAL:
						AddScore(150, DEAD);
						break;
					case RED:
						AddScore(100, DEADRARE);
						*breath_time = BREATH_TIME_MAX;	//固定値に変更　足す処理ではない
						PlaySound(SOUND_LABEL_SE_fire);
						break;
					case BLUE:
						AddScore(100, DEADRARE);
						if (player[0].laser_stock < LASER_STOCK)	//ストックが上限でなければ＋１
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
	if (*breath_time > 0)	//生成中であれば
	{
		D3DXVECTOR3 pos;

		switch (player[0].direction)	//プレイヤーの向きに応じて当たり判定を取る位置を変更
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
				ghost[i].pos -= ghost[i].move*1.1f;	//当たっている間は押し返すような処理

				if (ghost[i].hp <= 0)	//死亡時の処理
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

		for (int i = 0; i < ENEMY_MAX; i++)	//メイジとも当たり判定を取る
		{
			if (enemy[i].use == false ||
				enemy[i].invincible_time > 0) continue;

			bool ans = HitCheckBC(enemy[i].pos, pos, 60, 0);

			if (ans == true)
			{
				AddScore(6, DAMAGE);	//多め
				enemy[i].pos -= enemy[i].move*0.9f;	//押し返しはせず遅くなる程度
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
				attack[j].use = false;	//星は当たると消滅
				ghost[i].hp -= 40;
				AddScore(40, DAMAGE);
				ghost[i].speed *= 1.1f;	//攻撃を食らうたびに10％加速させる
				SetHitEffect(attack[j].pos);

				if (ghost[i].chase == true)
				{
					ghost[i].pos += -ghost[i].move * 7.0f;	//追尾中の敵にはノックバック
				}
				else
				{
					ghost[i].chase = true;	//攻撃された敵は追尾状態に
				}

				if (ghost[i].hp <= 0)	//死んだときの処理
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

				for (int k = 0; k < GHOST_MAX; k++)	//当たり判定があった場合、その位置で爆発し再度当たり判定を取る処理
				{
					if (ghost[k].use == false ||
						k == i) continue;	//上で当たってた場合は当たり判定を取らない

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

	//エネミーとレーザー
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (enemy[i].use == false ||
			enemy[i].invincible_time > 0) continue;
		for (int j = 0; j < LASER_MAX; j++)
		{
			if (laser[j].use == false ||
				laser[j].hit == true) continue;	//まだ当たっていない場合

			bool ans = HitCheckBB(laser[j].pos, laser[j].w * 0.5f, laser[j].h * 0.5f,
				enemy[i].pos, enemy[i].w / 2, enemy[i].h / 3);

			if (ans == true)
			{
				AddScore(150, DAMAGE);
				laser[j].hit = true;	//当たった判定にする
			}
		}
	}
	
	//エネミーと星
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
				AddScore(120, DAMAGE);	//多め
				attack[j].use = false;
				SetHitEffect(attack[j].pos);
			}
		}
	}

	//エネミーとステージ
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (enemy[i].use == false) continue;
		bool ans = HitCheckStage(enemy[i].pos);

		if (ans == false)
		{
			enemy[i].move *= -1;	//移動量を反転
			enemy[i].pos += enemy[i].move;
		}
	}

}

void TimeLine(int time)	//現在の時間に応じた処理を行うタイムライン　すべてここで管理されているのでわかりやすい
{
	PLAYER *player = GetPlayer();
	ENEMY *enemy = GetEnemy();

	if (time == 3599)
	{
		SetGhost(5, 2);
	}
	if (time % 300 == 0)	//5秒おきにゴーストをノーマル5体、レア2体生成
	{
		SetGhost(5, 2);
	}

	if (time % 360 == 0)	//6秒おきにプレイヤーがいる位置にレーザーを生成
	{
		SetEnemyLaser(D3DXVECTOR3(player[0].pos.x, SCREEN_CENTER_Y, 0.0f), DOWN);
		SetEnemyLaser(D3DXVECTOR3(SCREEN_CENTER_X, player[0].pos.y, 0.0f), LEFT);
	}

	//メテオの生成
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