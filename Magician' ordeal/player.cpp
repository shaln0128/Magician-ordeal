//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#include "player.h"
#include "sprite.h"
#include "input.h"
#include "attack.h"
#include "time.h"
#include "score.h"
#include "fire.h"
#include "laser.h"
#include "sound.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(15)	// キャラサイズ
#define TEXTURE_HEIGHT				(30)	// 
#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(3)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(4)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値

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
	"data/TEXTURE/mage000.png",
};

PLAYER player[PLAYER_MAX];	//プレイヤーの構造体

static float rad = 45 * pi / 180;	//斜め移動用の数値
static float g_cos = cosf(rad);
static float g_sin = sinf(rad);

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
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


	// プレイヤーの初期化

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		player[i].use = true;
		player[i].w = TEXTURE_WIDTH;
		player[i].h = TEXTURE_HEIGHT;
		player[i].pos = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f);	//初期位置は真ん中
		player[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		player[i].texNo = 0;

		player[i].countAnim = 0;
		player[i].patternAnim = 0;

		player[i].direction = DOWN;	//初期は下を向く

		player[i].free = true;

		player[i].speed = 1.8f;

		player[i].flash_time = 0;
		player[i].laser_stock = 0;

		player[i].hp = PLAYER_HP_MAX;
	}
	
	VIEWPORT *viewport = GetViewports();

	//ビューポートの倍率、左上座標をプレイヤー基準で更新
	viewport->mag = 2.0f;	//初期は2倍
	viewport->topleft.x = SCREEN_CENTER_X - player[0].pos.x * viewport->mag;
	viewport->topleft.y = SCREEN_CENTER_Y - player[0].pos.y * viewport->mag;

	SetViewports();

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
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
void UpdatePlayer(void)
{

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (player[i].use == true)		// プレイヤーが使われている？
		{						// Yes
			// アニメーション  
			player[i].countAnim++;
			if ((player[i].countAnim % ANIM_WAIT) == 0)
			{
				// パターンの切り替え
				player[i].patternAnim = (player[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

		}
		
		player[i].prev_pos = player[i].pos;

		if (player[i].free == true)
		{	

			if (player[i].laser_stock > 0 && GetKeyboardTrigger(DIK_L))	//スタックがあればLでレーザー発射
			{
				SetLaser(player[i].pos,player[i].direction);
				player[i].laser_stock--;
				PlaySound(SOUND_LABEL_SE_laser2);
			}

			if (player[i].damage_time > 0)	//被弾演出時間の更新
			{
				player[i].damage_time--;
			}

			if (player[i].flash_time == 0)	//攻撃間隔があけたら
			{
				if (GetKeyboardTrigger(DIK_K))	//Kで攻撃
				{
					SetAttack(player[i].pos,player[i].direction);
					player[i].flash_time = 30;	//30フレームの攻撃間隔を付与
					PlaySound(SOUND_LABEL_SE_attack);
				}
			}
			else
			{
				player[i].flash_time--;
			}
			

			//押されたキーに応じて向きを変更
			if (GetKeyboardPress(DIK_S))
			{
				player[i].direction = DOWN;
				if (GetKeyboardPress(DIK_D))
				{
					player[i].pos.x += g_cos * player[i].speed;
					player[i].pos.y += g_sin * player[i].speed;
				}
				else if (GetKeyboardPress(DIK_A))
				{
					player[i].pos.x -= g_cos * player[i].speed;
					player[i].pos.y += g_sin * player[i].speed;
				}
				else
				{
					player[i].pos.y += player[i].speed;
				}
			}
			else if (GetKeyboardPress(DIK_W))
			{
				player[i].direction = UP;
				if (GetKeyboardPress(DIK_D))
				{
					player[i].pos.x += g_cos * player[i].speed;
					player[i].pos.y -= g_sin * player[i].speed;
				}
				else if (GetKeyboardPress(DIK_A))
				{
					player[i].pos.x -= g_cos * player[i].speed;
					player[i].pos.y -= g_sin * player[i].speed;
				}
				else
				{
					player[i].pos.y -= player[i].speed;
				}
			}
			else if (GetKeyboardPress(DIK_D))
			{
				player[i].direction = RIGHT;
				player[i].pos.x += player[i].speed;
			}
			else if (GetKeyboardPress(DIK_A))
			{
				player[i].direction = LEFT;
				player[i].pos.x -= player[i].speed;
			}

			//ビューポートの倍率、左上座標をプレイヤー基準で更新

			VIEWPORT *viewport = GetViewports();

			if (GetKeyboardPress(DIK_I) && viewport->mag < 3.0f)
			{
				viewport->mag += 0.02f;
			}
			else if (GetKeyboardPress(DIK_O) && viewport->mag > 1.0f)
			{
				viewport->mag -= 0.02f;

				if (viewport->mag < 1.0f)
				{
					viewport->mag = 1.0f;
				}
			}

			viewport->topleft.x = SCREEN_CENTER_X - player[i].pos.x * viewport->mag;
			viewport->topleft.y = SCREEN_CENTER_Y - player[i].pos.y * viewport->mag;

			if (viewport->topleft.x > 0)
			{
				viewport->topleft.x = 0.0f;
			}
			else if (viewport->topleft.x < SCREEN_WIDTH * (1 - viewport->mag))
			{
				viewport->topleft.x = SCREEN_WIDTH * (1 - viewport->mag);
			}

			if (viewport->topleft.y > 0)
			{
				viewport->topleft.y = 0.0f;
			}
			else if (viewport->topleft.y < SCREEN_HEIGHT * (1 - viewport->mag))
			{
				viewport->topleft.y = SCREEN_HEIGHT * (1 - viewport->mag);
			}

		}

	}


#ifdef _DEBUG	// デバッグ情報を表示する
	char *str = GetDebugStr();
	VIEWPORT *viewport = GetViewports();
	float mag = viewport->mag;

	sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f MAG:%d", player[0].pos.x, player[0].pos.y, player[0].damage_time);
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
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

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (player[i].use == true)		// プレイヤーが使われている？
			{						// Yes
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[player[i].texNo]);

				// プレイヤーの位置やテクスチャー座標を反映
				float px = player[i].pos.x;	// プレイヤーの表示位置X
				float py = player[i].pos.y;	// プレイヤーの表示位置Y
				float pw = player[i].w;		// プレイヤーの表示幅
				float ph = player[i].h;		// プレイヤーの表示高さ

				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
				float tx = tw* player[i].patternAnim;	// テクスチャの左上X座標
				float ty = th*player[i].direction;	// テクスチャの左上Y座標

				float r = 1.0f;
				float g = player[i].damage_time > 0 ? 0.2f : 1.0f;	//被弾演出中は赤くなる
				float b = player[i].damage_time > 0 ? 0.2f : 1.0f;

				float alpha = (player[i].damage_time / 10) % 2 == 0 ? 1.0f : 0.3f;	//被弾演出中は点滅

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,D3DXCOLOR(r,g,b,alpha),true);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
	}
	
}


//=============================================================================
// プレイヤーの座標を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
	return(player);
}




