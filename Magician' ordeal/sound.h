//=============================================================================
//
// サウンド処理 [sound.h]
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_title,				// タイトル
	SOUND_LABEL_BGM_tutorial,			// チュートリアル
	SOUND_LABEL_BGM_game,				// ゲーム
	SOUND_LABEL_SE_decision,			// 決定
	SOUND_LABEL_SE_pause,				// ポーズ
	SOUND_LABEL_SE_switch,	 			// 切り替え
	SOUND_LABEL_SE_drumroll,			// ドラムロール
	SOUND_LABEL_SE_scoredecision,		// スコア決定
	SOUND_LABEL_SE_rankdecision,		// ランク決定
	SOUND_LABEL_SE_rankindecision,		// ランクイン
	SOUND_LABEL_SE_laserbuff,			// レーザーバフ獲得
	SOUND_LABEL_SE_attack,				// 攻撃
	SOUND_LABEL_SE_fire,				// 炎
	SOUND_LABEL_SE_enemylasercharge,	// 敵のレーザーチャージ
	SOUND_LABEL_SE_hitattack, 			// 攻撃ヒット
	SOUND_LABEL_SE_hitenemyattack,		// 敵の攻撃ヒット
	SOUND_LABEL_SE_hitmeteor,			// メテオ着弾
	SOUND_LABEL_SE_laser,				// レーザー
	SOUND_LABEL_SE_laser2,				// レーザー２
	SOUND_LABEL_SE_vanish,				// 敵消失
	SOUND_LABEL_SE_success,				// 成功
	SOUND_LABEL_SE_failure,		 		// 失敗
	SOUND_LABEL_SE_throwing,			// メテオ発射
	SOUND_LABEL_SE_ghost,				// ゴースト出現
	SOUND_LABEL_SE_meteorcharge,		// メテオ詠唱
	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);
void SetSoundVolume(int n, int g);
#endif
