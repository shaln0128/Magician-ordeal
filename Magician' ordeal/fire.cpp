//=============================================================================
//
// 炎パーティクルの処理【fire.cpp】
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "fade.h"
#include "fire.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_SIZE				(20)	// キャラサイズ
#define TEXTURE_MAX					(2)		// テクスチャの数

#define MAHOUJIN_WIDTH				(60)	//魔法陣のサイズ
#define MAHOUJIN_HEIGHT				(20)

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
	"data/TEXTURE/mahoujin.png"
};

static PARTICLE fire[FIRE_MAX];	//炎パーティクルの構造体
static int breath_time;			//炎の生成時間
static float rad = pi / 180.0f;	//ラジアンへの変換
static D3DXVECTOR3 g_Pos;		//魔法陣の表示位置
static float g_w, g_h;			//魔法陣のサイズ
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitFire(void)
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

	breath_time = 0;

	// 炎パーティクルの初期化
	for (int i = 0; i < FIRE_MAX; i++)
	{
		fire[i].use = false;
		fire[i].size = TEXTURE_SIZE;
		fire[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		fire[i].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		fire[i].time = 0;
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitFire(void)
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
void UpdateFire(void)
{
	PLAYER *player = GetPlayer();

	int fire_count = 0;
	int count = 0;	//1フレームで生成された個数をカウント

	for (int i = 0; i < FIRE_MAX; i++)
	{
		if (breath_time > 0 &&
			fire[i].use == false &&
			count < 5)	//もし生成時間内で、フレーム内での生成可能個数に達していないなら
		{
			float angle = (rand() % 120 - 60) * rad;	//－60～60度
			fire[i].use = true;
			fire[i].size = TEXTURE_SIZE;
			float speed = angle > 0 ? 2.2f - angle : 2.2f + angle;	//角度に応じて速度を変化　真ん中ほど速い

			switch (player[0].direction)	//プレイヤーの向いてる方向に生成
			{
			case DOWN:
				fire[i].pos = player[0].pos + D3DXVECTOR3(rand() % 40 - 20.0f, 30.0f, 0.0f);
				angle += 90 * rad;
				break;
			case LEFT:
				fire[i].pos = player[0].pos + D3DXVECTOR3(-20.0f, rand() % 40 - 20.0f, 0.0f);
				angle += 180 * rad;
				break;
			case RIGHT:
				fire[i].pos = player[0].pos + D3DXVECTOR3(20.0f, rand() % 40 - 20.0f, 0.0f);
				break;
			case UP:
				fire[i].pos = player[0].pos + D3DXVECTOR3(rand() % 40 - 20.0f, -30.0f, 0.0f);
				angle -= 90 * rad;
				break;
			}

			fire[i].move = D3DXVECTOR3(cosf(angle)*speed, sinf(angle)*speed, 0.0f);
			fire[i].time = rand() % 10 + 40;	//生存時間にもブレを持たせる
			count++;
		}
		else if (fire[i].use == true)	//生成されたものの処理
		{
			fire[i].time -= 1;
			fire[i].pos += fire[i].move;
			
			if (fire[i].time == 0)	//生存時間が切れたら消す
			{
				fire[i].use = false;
			}
			else 
			{
				if (fire[i].time < 10)	//残り10フレームの間は小さくする
				{
					fire[i].size *= 0.95f;	//5％ずつ
				}

				fire_count += 1;
			}
		}
	}

	if (breath_time > 0)	//生成時間中
	{
		breath_time--;
		switch (player[0].direction)	//プレイヤーの向きに応じて魔法陣の位置とサイズを変化
		{
		case DOWN:
			g_Pos = player[0].pos + D3DXVECTOR3(0.0f, 30.0f, 0.0f);
			g_w = MAHOUJIN_WIDTH;
			g_h = MAHOUJIN_HEIGHT;
			break;
		case LEFT:
			g_Pos = player[0].pos + D3DXVECTOR3(-20.0f, 0.0f, 0.0f);
			g_w = MAHOUJIN_HEIGHT;
			g_h = MAHOUJIN_WIDTH;
			break;
		case RIGHT:
			g_Pos = player[0].pos + D3DXVECTOR3(20.0f, 0.0f, 0.0f);
			g_w = MAHOUJIN_HEIGHT;
			g_h = MAHOUJIN_WIDTH;
			break;
		case UP:
			g_Pos = player[0].pos + D3DXVECTOR3(0.0f, -30.0f, 0.0f);
			g_w = MAHOUJIN_WIDTH;
			g_h = MAHOUJIN_HEIGHT;
			break;
		}
	}
	
#ifdef _DEBUG	// デバッグ情報を表示する
	char *str = GetDebugStr();
	sprintf(&str[strlen(str)], " fire%d", fire_count);
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawFire(void)
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

	for (int i = 0; i < FIRE_MAX; i++)
	{
		if (fire[i].use == true)	//パーティクルの描画
		{
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			float dx = fire[i].pos.x;
			float dy = fire[i].pos.y;

			float size = fire[i].size;

			SetSpriteColor(g_VertexBuffer, dx, dy, size, size, 0.0f, 0.0f, 1.0f, 1.0f,
					D3DXCOLOR(1.0f, 0.2f, 0.1f, 1.0f),true);

				// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	if (breath_time > 0)	//魔法陣の描画
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		float dx = g_Pos.x;
		float dy = g_Pos.y;
		float dw = g_w;
		float dh = g_h;

		SetSpriteColor(g_VertexBuffer, dx, dy, dw, dh, 0.0f, 0.0f, 1.0f, 1.0f,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), true);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	SetBlendState(BLEND_MODE_ALPHABLEND);

}

int *GetBreathTime(void)	//生成時間へのアドレス
{
	return(&breath_time);
}

PARTICLE *GetFire(void)	//炎パーティクルへのアドレス
{
	return(&fire[0]);
}