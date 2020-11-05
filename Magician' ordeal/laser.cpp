//=============================================================================
//
// レーザーの処理【laser.cpp】
// Author :GP11B132 36  向田駿平
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "laser.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_SIZE				(60)	// キャラサイズ
#define TEXTURE_LONG				(500)	// キャラサイズ
#define TEXTURE_MAX					(2)		// テクスチャの数

#define BALL_WIDTH				(80)		//根元の幅
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

static PARTICLE laser[LASER_MAX];	//レーザーの構造体
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitLaser(void)
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
	for (int i = 0; i < LASER_MAX; i++)
	{
		laser[i].use = false;
		laser[i].hit = false;	//当たってない判定
		laser[i].w = 960.0f;
		laser[i].h = 540.0f;
		laser[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		laser[i].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		laser[i].time = 0;
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitLaser(void)
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
void UpdateLaser(void)
{

	for (int i = 0; i < LASER_MAX; i++)
	{
		if (laser[i].use == true)
		{
			laser[i].time -= 1;
			if (laser[i].time == 0)	//生存時間が切れたら消す
			{
				laser[i].use = false;
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
void DrawLaser(void)
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

	for (int i = 0; i < LASER_MAX; i++)
	{
		//テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		if (laser[i].use == true)
		{
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			float dx = laser[i].pos.x;
			float dy = laser[i].pos.y;

			float dw = laser[i].w;
			float dh = laser[i].h;

			switch (laser[i].direction)	//与えられた向きにおうじて幅を調整
			{
			case DOWN:
			case UP:
				dw = laser[i].time > 50 ? (60 - laser[i].time) * 6.0f : dw + rand() % 10 - 5;
				break;
			case LEFT:
			case RIGHT:
				dh = laser[i].time > 50 ? (60 - laser[i].time) * 6.0f : dh + rand() % 10 - 5;
				break;
			}

			SetSpriteColor(g_VertexBuffer, dx, dy, dw, dh, 0.0f, 0.0f, 1.0f, 1.0f,
				D3DXCOLOR(0.3f, 0.3f, 1.0f, 1.0f), true);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

			for (int j = 0; j < PARTICLE_MAX; j++)	//パーティクルの描画
			{
				// １枚のポリゴンの頂点とテクスチャ座標を設定
				float sx, sy;
				float cr = rand() % (10 + 1) / 10.0f;
				float cg = rand() % (10 + 1) / 10.0f;
				float cb = rand() % (10 + 1) / 10.0f;
				float pdw, pdh;

				switch (laser[i].direction)	//与えられた向きに応じて生成範囲と幅を調整
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

			//根元の描画
			//テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

			dx = laser[i].pos.x;
			dy = laser[i].pos.y;

			switch (laser[i].direction)
			{
			case DOWN:
				dy += 10.0f - laser[i].h * 0.5f;
				dw = BALL_WIDTH;
				dh = BALL_HEIGHT;
				break;
			case LEFT:
				dx += laser[i].w * 0.5f;
				dw = BALL_HEIGHT;
				dh = BALL_WIDTH;
				break;
			case RIGHT:
				dx += -laser[i].w * 0.5f;
				dw = BALL_HEIGHT;
				dh = BALL_WIDTH;
				break;
			case UP:
				dy += -10.0f + laser[i].h * 0.5f;
				dw = BALL_WIDTH;
				dh = BALL_HEIGHT;
				break;
			}

			for (int j = 0; j < 5; j++)
			{
				float mag = (rand() % 20 + 90) * 0.01f;

				SetSpriteColor(g_VertexBuffer, dx, dy, dw * mag, dh * mag, 0.0f, 0.0f, 1.0f, 1.0f,
					D3DXCOLOR(0.3f, 0.3f, 1.0f, 1.0f), true);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}
	
	SetBlendState(BLEND_MODE_ALPHABLEND);
}

PARTICLE *GetLaser(void)	//レーザーの構造体へのアドレス
{
	return(&laser[0]);
}

void SetLaser(D3DXVECTOR3 pos,DIRECTION vect)	//レーザーのセット
{
	for (int i = 0; i < LASER_MAX; i++)
	{
		if (laser[i].use == false)
		{
			laser[i].use = true;
			laser[i].hit = false;
			laser[i].time = 60;
			laser[i].direction = vect;

			switch (vect)	//与えられた向きに応じて幅と位置を調整
			{
			case DOWN:
				laser[i].pos = pos + D3DXVECTOR3(0.0f, TEXTURE_LONG / 2.0f + 15.0f, 0.0f);
				laser[i].w = TEXTURE_SIZE;
				laser[i].h = TEXTURE_LONG - 50.0f;
				break;
			case LEFT:
				laser[i].pos = pos + D3DXVECTOR3(-TEXTURE_LONG / 2.0f - 8.0f, 0.0f, 0.0f);
				laser[i].w = TEXTURE_LONG - 50.0f;
				laser[i].h = TEXTURE_SIZE;
				break;
			case RIGHT:
				laser[i].pos = pos + D3DXVECTOR3(TEXTURE_LONG / 2.0f + 8.0f, 0.0f, 0.0f);
				laser[i].w = TEXTURE_LONG - 50.0f;
				laser[i].h = TEXTURE_SIZE;
				break;
			case UP:
				laser[i].pos = pos + D3DXVECTOR3(0.0f, -TEXTURE_LONG / 2.0f - 15.0f, 0.0f);
				laser[i].w = TEXTURE_SIZE;
				laser[i].h = TEXTURE_LONG - 50.0f;
				break;
			}

			return;
		}
	}
}






