//=============================================================================
//
// メテオ処理【meteor.cpp】
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "fade.h"
#include "meteor.h"
#include "player.h"
#include "effect.h"
#include "game.h"
#include "score.h"
#include "tutorialflag.h"
#include "tutorial.h"
#include "sound.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(40)	// キャラサイズ
#define TEXTURE_HEIGHT				(40)	// キャラサイズ
#define TEXTURE_SIZE				(20)
#define TEXTURE_MAX					(3)		// テクスチャの数

#define PARTICLE_MAX				(120)	//パーティクルの最大値

#define METEOR_SPEED				(2.0f)

#define pi							float(3.14159265)	//ぱい
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/effect_ball.jpg",
	"data/TEXTURE/target.png",
	"data/TEXTURE/meteor.png",
};

static METEOR meteor[METEOR_MAX];					//メテオの構造体
static PARTICLE meteor_particle[PARTICLE_MAX];		//パーティクルの構造体
static METEOR landing_point[METEOR_MAX];			//着地点の構造体
static float rad = pi / 180.0f;						//ラジアン変換用
static int meteor_count;							//メテオの生成量
static D3DXVECTOR3 *g_Pos;							//メテオが参照する位置へのポインタ
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMeteor(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(
			GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// メテオの初期化
	for (int i = 0; i < METEOR_MAX; i++)
	{
		meteor[i].use = false;
		meteor[i].w = TEXTURE_WIDTH;
		meteor[i].h = TEXTURE_HEIGHT;
		meteor[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		meteor[i].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		meteor[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		meteor[i].time = 0;
		meteor[i].arrival_time = 0;
		meteor[i].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		meteor[i].state = DIFFUSION;

		
	}

	//　パーティクルの初期化
	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		meteor_particle[i].use = false;
		meteor_particle[i].w = TEXTURE_WIDTH;
		meteor_particle[i].h = TEXTURE_HEIGHT;
		meteor_particle[i].move = D3DXVECTOR3(0.0f, 0.2f, 0.0f);
		meteor_particle[i].time = 0;
		meteor_particle[i].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}

	g_Pos = NULL;	//参照する位置へのポインタ

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMeteor(void)
{
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateMeteor(void)
{
	if (meteor_count > 0)
	{
		for (int i = 0; i < meteor_count; i++)
		{
			if (meteor[i].use == false) continue;

			PLAYER *player = GetPlayer();
			switch (meteor[i].state)	//現在の状態に応じて処理を分ける
			{
			case DIFFUSION:	//拡散状態
				if (meteor[i].time == 60)	//次の状態へ
				{
					meteor[i].time = 0;
					meteor[i].state = CIRCLING;
				}
				else
				{
					meteor[i].time++;
					meteor[i].pos = *g_Pos + meteor[i].move * float(meteor[i].time);	//基準点から離れていく
				}
				break;
			case CIRCLING:	//回転状態
				if (meteor[i].time == 150 + (120 / meteor_count)*i)	//基準点の頭上に来たら次の状態へ移行
				{
					PlaySound(SOUND_LABEL_SE_throwing);
					D3DXVECTOR3 target = player[0].pos + D3DXVECTOR3(float(rand() % 200 - 100), float(rand() % 200 - 100),0.0f);	//プレイヤー付近めがけて発射
					D3DXVECTOR3 dif = target - meteor[i].pos;
					float rot = atan2f(dif.y, dif.x);
					D3DXVECTOR2 temp = D3DXVECTOR2(dif.x, dif.y);
					float distance = D3DXVec2Length(&temp);

					meteor[i].landingpoint = target;	//着弾地点
					meteor[i].arrival_time = int(distance / METEOR_SPEED);	//到着予定時間
					meteor[i].move = D3DXVECTOR3(cosf(rot) * METEOR_SPEED, sinf(rot) * METEOR_SPEED, 0.0f);	//移動量
					meteor[i].time = 0;
					meteor[i].state = FALLING;
					SetEffect(meteor[i].pos, SHOT);	//発射時の音を鳴らす
				}
				else
				{
					meteor[i].time++;
					meteor[i].rot.y -= rad * 3.0f;	//基準点に対する位置の角度の回転
					meteor[i].rot.z += rad * 5.0f;	//テクスチャの回転
					meteor[i].pos = *g_Pos + D3DXVECTOR3(cosf(meteor[i].rot.y) * 60.0f,sinf(meteor[i].rot.y) * 60.0f,0.0f);	//基準点から回転していく
				}
				break;
			case FALLING:
				if (meteor[i].time == meteor[i].arrival_time)
				{
					meteor[i].use = false;
					SetEffect(meteor[i].pos, HIT);
					PlaySound(SOUND_LABEL_SE_hitmeteor);
					bool ans = HitCheckBC(meteor[i].pos, player[0].pos, 20.0f, 8.0f);
					if (ans == true)
					{
						PlaySound(SOUND_LABEL_SE_hitenemyattack);
						player[0].damage_time = 60;
						AddScore(-300,HITMINUS);
						if (GetTutorialState() == MAGE_SKILL)
						{
							SetFailure();
						}
					}
				}
				else
				{
					if (meteor[i].time % 4 == 0)
					{
						SetMeteorParticle(meteor[i].pos - D3DXVECTOR3(0.0f, sinf(180.0f * rad * (meteor[i].time / float(meteor[i].arrival_time))) * 60.0f, 0.0f), meteor[i].color);
					}
					meteor[i].time++;
					meteor[i].rot.z += rad * 5.0f;
					meteor[i].pos += meteor[i].move;
				}
				break;
			}
		}
	}

	for (int i = 0; i < PARTICLE_MAX; i++)	//パーティクルの処理
	{
		if (meteor_particle[i].use == true)
		{
			if (meteor_particle[i].time == 40)	//生存時間が上限に達したら消す
			{
				meteor_particle[i].use = false;
			}
			else
			{
				meteor_particle[i].time++;
				meteor_particle[i].pos += meteor_particle[i].move;
				meteor_particle[i].w *= 0.98f;	//サイズを2％ずつ下げていく
				meteor_particle[i].h *= 0.98f;
			}
		}
	}
#ifdef _DEBUG	// デバッグ情報を表示する
	char *str = GetDebugStr();
	sprintf(&str[strlen(str)], " fire%d", meteor_count);
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMeteor(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	SetBlendState(BLEND_MODE_ADD);

	for (int i = 0; i < METEOR_MAX; i++)
	{
		if (meteor[i].use == true)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			float dx = meteor[i].pos.x;
			float dy = meteor[i].pos.y;
			float sy = meteor[i].state == FALLING ? sinf(180.0f * rad * (meteor[i].time / float(meteor[i].arrival_time))) * 60.0f: 0.0f;	//降下中はサインカーブの値を与え、弧を描かせる
			float dw = meteor[i].w;
			float dh = meteor[i].h;
			D3DXCOLOR color = meteor[i].color;
			float rot = meteor[i].rot.z;

			SetSpriteColorRotation(g_VertexBuffer, dx, dy - sy, dw * 1.2f, dh * 1.2f, 0.0f, 0.0f, 1.0f, 1.0f,
				color, rot);

			// まずは円を描画
			GetDeviceContext()->Draw(4, 0);

			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

			//3つの楕円を描画
			SetSpriteColorRotation(g_VertexBuffer, dx, dy - sy, dw, dh, 0.0f, 0.0f, 1.0f, 1.0f,
				color, rot);

			GetDeviceContext()->Draw(4, 0);

			rot += 60.0f * rad;

			SetSpriteColorRotation(g_VertexBuffer, dx, dy - sy, dw, dh, 0.0f, 0.0f, 1.0f, 1.0f,
				color, rot);

			GetDeviceContext()->Draw(4, 0);

			rot += 60.0f * rad;

			SetSpriteColorRotation(g_VertexBuffer, dx, dy - sy, dw, dh, 0.0f, 0.0f, 1.0f, 1.0f,
				color, rot);

			GetDeviceContext()->Draw(4, 0);

			if (meteor[i].state == FALLING)	//降下中は着弾地点にサークルを描画
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

				dx = meteor[i].landingpoint.x;
				dy = meteor[i].landingpoint.y;
				float size = 60.0f - (TEXTURE_SIZE * meteor[i].time / float(meteor[i].arrival_time));

				SetSpriteColor(g_VertexBuffer, dx, dy, size, size, 0.0f, 0.0f, 1.0f, 1.0f,
					color, true);

				GetDeviceContext()->Draw(4, 0);
			}
		
		}
	}

	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	for (int i = 0; i < PARTICLE_MAX; i++)	//パーティクルの描画
	{
		if (meteor_particle[i].use == true)
		{
			float dx = meteor_particle[i].pos.x;
			float dy = meteor_particle[i].pos.y;
			float dw = meteor_particle[i].w;
			float dh = meteor_particle[i].h;
			D3DXCOLOR color = meteor_particle[i].color;

			SetSpriteColor(g_VertexBuffer, dx, dy, dw, dh, 0.0f, 0.0f, 1.0f, 1.0f,
				color, true);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	SetBlendState(BLEND_MODE_ALPHABLEND);

}

void SetMeteor(D3DXVECTOR3 *pos, int num)	//メテオのセット
{
	PlaySound(SOUND_LABEL_SE_meteorcharge);	//詠唱の音を鳴らす

	g_Pos = pos;	//メイジの位置へのポインタを格納
	meteor_count = num;	//生成するメテオの個数を格納

	float angle = 360.0f * rad / num;

	for (int i = 0; i < num; i++)
	{
		meteor[i].use = true;
		meteor[i].w = TEXTURE_WIDTH;
		meteor[i].h = TEXTURE_HEIGHT;
		meteor[i].pos = *pos;
		meteor[i].move = D3DXVECTOR3(cosf(angle*i), sinf(angle*i), 0.0f);	//生成する個数分、円を分割
		meteor[i].rot = D3DXVECTOR3(0.0f, angle*i, 0.0f);
		meteor[i].time = 0;
		meteor[i].arrival_time = 0;
		meteor[i].color = D3DXCOLOR(0.8f, 0.2f, 0.8f, 1.0f);
		meteor[i].state = DIFFUSION;	//拡散状態からスタート
	}

	return;
}

void SetMeteorParticle(D3DXVECTOR3 pos, D3DXCOLOR color)	//パーティクルのセット
{
	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		if (meteor_particle[i].use == false)
		{
			meteor_particle[i].use = true;
			meteor_particle[i].pos = pos;
			meteor_particle[i].w = TEXTURE_WIDTH;
			meteor_particle[i].h = TEXTURE_HEIGHT;
			meteor_particle[i].color = color;
			meteor_particle[i].time = 0;

			return;
		}
	}
	return;
}

METEOR *GetMeteor(void)	//メテオの構造体へのアドレス
{
	return(&meteor[0]);
}