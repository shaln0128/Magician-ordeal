//=============================================================================
//
// 敵のレーザー処理【enemylaser.cpp】
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "enemylaser.h"
#include "sound.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_SIZE					(40)	// 短いほうの長さ
#define TEXTURE_LONG_WIDTH				(840)	// 横長の場合
#define TEXTURE_LONG_HEIGHT				(490)	// 縦長の場合
#define TEXTURE_MAX					(2)				// テクスチャの数

#define BALL_WIDTH				(60)	//根元の大きさ
#define BALL_HEIGHT				(30)
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/effect_laser.jpg",
	"data/TEXTURE/effect_ball.jpg",
};

static PARTICLE enemylaser[ENEMY_LASER_MAX];	//エネミーレーザーの構造体
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemyLaser(void)
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

	// エネミーレーザーの初期化
	for (int i = 0; i < ENEMY_LASER_MAX; i++)
	{
		enemylaser[i].use = false;
		enemylaser[i].hit = false;	//当たっているかどうか
		enemylaser[i].w = TEXTURE_SIZE;
		enemylaser[i].h = TEXTURE_LONG_HEIGHT;
		enemylaser[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		enemylaser[i].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		enemylaser[i].time = 0;
		enemylaser[i].color = D3DXCOLOR(0.0f, 0.0f, 0.0f,0.0f);
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemyLaser(void)
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
void UpdateEnemyLaser(void)
{

	for (int i = 0; i < ENEMY_LASER_MAX; i++)
	{
		if (enemylaser[i].use == true)
		{
			enemylaser[i].time -= 1;

			if (enemylaser[i].time == 0)	//生存時間が切れたら消す
			{
				enemylaser[i].use = false;
			}
			else if (enemylaser[i].time == 60)	//発射の段階で音を鳴らす
			{
				PlaySound(SOUND_LABEL_SE_laser);
			}
		}
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	/*char *str = GetDebugStr();
	sprintf(&str[strlen(str)], " R%.2f G%.2f B%.2f are%.6f", uo.r, uo.g, uo.b, are);
*/
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemyLaser(void)
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


	SetBlendState(BLEND_MODE_ADD);	//加算合成

	for (int i = 0; i < ENEMY_LASER_MAX; i++)
	{
		//テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		if (enemylaser[i].use == true)
		{
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			float dx = enemylaser[i].pos.x;
			float dy = enemylaser[i].pos.y;

			float dw = enemylaser[i].w;
			float dh = enemylaser[i].h;
			float sx, sy;	//ずれ幅

			D3DXCOLOR color = enemylaser[i].color;
			color.a = enemylaser[i].time > 60 ? (180 - enemylaser[i].time) / 120.0f : 1.0f;	//徐々に不透明度を上げる

			switch (enemylaser[i].direction)	//与えられた向きに応じて表示サイズを変更
			{
			case DOWN:	//縦長
			case UP:
				dh = enemylaser[i].time > 60 ? dh * (180 - enemylaser[i].time) / 120.0f : dh;
				dw = enemylaser[i].time > 60 ? dw : dw + rand() % 10 - 5;	//短いほうにはブレを生じさせる
				break;
			case LEFT:	//横長
			case RIGHT:
				dh = enemylaser[i].time > 60 ? dh : dh + rand() % 10 - 5;	//短いほうにはブレを生じさせる
				dw = enemylaser[i].time > 60 ? dw * (180 - enemylaser[i].time) / 120.0f : dw;
				break;
			}

			SetSpriteColor(g_VertexBuffer, dx, dy, dw, dh, 0.0f, 0.0f, 1.0f, 1.0f,
				color, true);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

			if (enemylaser[i].time <= 60)	//発射中はパーティクルを生成
			{
				for (int j = 0; j < PARTICLE_MAX; j++)
				{

					// 色はランダム
					float cr = rand() % (10 + 1) / 10.0f;
					float cg = rand() % (10 + 1) / 10.0f;
					float cb = rand() % (10 + 1) / 10.0f;
					float pdw, pdh;	//パーティクルの表示幅

					switch (enemylaser[i].direction)	//与えられた向きに応じて乱数の生成範囲を変える
					{
					case DOWN:
					case UP:
						sx = (float)(rand() % int(dw)) - dw / 2;
						sy = ((float)(rand() % int(dh)) - dh / 2) * 0.95f;
						pdw = 9.0f;
						pdh = (float)(rand() % 80) + 20.0f;
						break;
					case LEFT:
					case RIGHT:
						sx = ((float)(rand() % int(dw)) - dw / 2) * 0.95f;
						sy = (float)(rand() % int(dh)) - dh / 2;
						pdw = (float)(rand() % 80) + 20.0f;
						pdh = 9.0f;
						break;
					}

					SetSpriteColor(g_VertexBuffer, dx + sx, dy + sy, pdw, pdh, 0.0f, 0.0f, 1.0f, 1.0f,
						D3DXCOLOR(cr, cg, cb, 1.0f), true);

					// ポリゴン描画
					GetDeviceContext()->Draw(4, 0);
				}

			}
			
			//根元の描画設定
			//テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

			dx = enemylaser[i].pos.x;
			dy = enemylaser[i].pos.y;
			sx = 0.0f;
			sy = 0.0f;
			color.a = 1.0f;

			switch (enemylaser[i].direction)	//与えられた向きに応じて表示位置と幅を変える
			{
			case DOWN:
				sy = enemylaser[i].h * 0.5f;
				dw = BALL_WIDTH;
				dh = BALL_HEIGHT;
				break;
			case LEFT:
				sx = enemylaser[i].w * 0.5f;
				dw = BALL_HEIGHT;
				dh = BALL_WIDTH;
				break;
			}

			sx = enemylaser[i].time > 60 ? sx * (180 - enemylaser[i].time) / 120.0f : sx;
			sy = enemylaser[i].time > 60 ? sy * (180 - enemylaser[i].time) / 120.0f : sy;

			for (int j = 0; j < 5; j++)	//ランダムな倍率（0.9～1.1）のものを5枚重ねて描画
			{
				float mag = (rand() % 20 + 90) * 0.01f;

				SetSpriteColor(g_VertexBuffer, dx + sx, dy + sy, dw * mag, dh * mag, 0.0f, 0.0f, 1.0f, 1.0f,
					color, true);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				SetSpriteColor(g_VertexBuffer, dx - sx, dy - sy, dw * mag, dh * mag, 0.0f, 0.0f, 1.0f, 1.0f,
					color, true);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}

	SetBlendState(BLEND_MODE_ALPHABLEND);
}

PARTICLE *GetEnemyLaser(void)	//エネミーレーザーの構造体へのアドレス
{
	return(&enemylaser[0]);
}

void SetEnemyLaser(D3DXVECTOR3 pos, DIRECTION vect)	//エネミーレーザーのセット
{
	PlaySound(SOUND_LABEL_SE_enemylasercharge);	//セット時に音を鳴らす
	for (int i = 0; i < ENEMY_LASER_MAX; i++)
	{
		if (enemylaser[i].use == false)
		{
			enemylaser[i].use = true;
			enemylaser[i].hit = false;
			enemylaser[i].time = 180;
			enemylaser[i].direction = vect;
			enemylaser[i].color = D3DXCOLOR( 1.0f, 0.2f, 0.2f,1.0f);

			switch (vect)
			{
			case DOWN:
				enemylaser[i].pos = pos;
				enemylaser[i].w = TEXTURE_SIZE;
				enemylaser[i].h = TEXTURE_LONG_HEIGHT;
				break;
			case LEFT:
				enemylaser[i].pos = pos;
				enemylaser[i].w = TEXTURE_LONG_WIDTH;
				enemylaser[i].h = TEXTURE_SIZE;
				break;
			}
			return;
		}
	}
}






