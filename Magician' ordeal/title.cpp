//=============================================================================
//
// タイトル処理[title.cpp]
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#include "title.h"
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "fade.h"
#include "sound.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(500)	// キャラサイズ
#define TEXTURE_HEIGHT				(300)	 
#define TEXTURE_MAX					(3)		// テクスチャの数

#define TEXTURE_DIVIDE_X	(120)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_DIVIDE_Y	(60)		// アニメパターンのテクスチャ内分割数（Y)

#define PRESSENTER_WIDTH			(360)
#define PRESSENTER_HEIGHT			(40)

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
	"data/TEXTURE/title_new.png",
	"data/TEXTURE/press_enter.png",
	"data/TEXTURE/white.png",
};


CURSORPOP title;	//タイトルの構造体

static int x_curve_frequency;						//横にカーブする間隔
static int y_curve_frequency;						//縦にカーブする間隔
static float x_curve_size;							//横のカーブの幅
static float y_curve_size;							//縦のカーブの幅
static float x_rad;
static float y_rad;
static float add_x_rad;
static float add_y_rad;
static float rad = pi / 180.0f;
static float g_mag;									//タイトルの表示倍率
static int g_time;									//経過時間
static float addcolor = 1.0f / 20.0f;				//色を足す度合い
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
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


	// タイトルの初期化
	title.use = true;
	title.w = TEXTURE_WIDTH;
	title.h = TEXTURE_HEIGHT;
	title.pos = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f);
	title.texNo = 0;
	title.pop = false;

	//カーブの度合い
	x_curve_frequency = 2;
	y_curve_frequency = 2;
	x_curve_size = 1.0f;
	y_curve_size = 4.0f;
	x_rad = 0.0f;
	y_rad = 0.0f;
	add_x_rad = 360.0f * rad * x_curve_frequency / TEXTURE_DIVIDE_Y;
	add_y_rad = 360.0f * rad * y_curve_frequency / TEXTURE_DIVIDE_X;

	//タイトルの倍率
	g_mag = 5.8f;

	g_time = 0;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
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
void UpdateTitle(void)
{
	g_time++;

	if (g_time > 480)	//演出が終了したらEnterを受け付ける
	{
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			SetFade(FADE_OUT, MODE_SELECT);
			PlaySound(SOUND_LABEL_SE_decision);
		}
	}
	else
	{
		//倍率を小さくしていく
		g_mag -= 0.015f;
		if (g_mag < 1.0f)
		{
			g_mag = 1.0f;
		}
	}

	if (g_time % 3 == 0)
	{
		//3フレームおきに揺らす
		x_rad += add_x_rad;
		y_rad += add_y_rad;
	}

//#ifdef _DEBUG	// デバッグ情報を表示する
//	char *str = GetDebugStr();
//	sprintf(&str[strlen(str)], " R%.2f G%.2f B%.2f are%.6f", uo.r, uo.g, uo.b,are);
//
//#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
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

	// スコアの位置やテクスチャー座標を反映
	float px = title.pos.x;		// タイトルの表示位置X
	float py = title.pos.y;		// タイトルの表示位置Y
	float pw = title.w / TEXTURE_DIVIDE_X * g_mag;			// タイトルの表示幅
	float ph = title.h / TEXTURE_DIVIDE_Y * g_mag;			// タイトルの表示高さ

	float tw = 1.0f / TEXTURE_DIVIDE_X;		// テクスチャの幅
	float th = 1.0f / TEXTURE_DIVIDE_Y;		// テクスチャの高さ
	float tx = 0.0f;		// テクスチャの左上X座標
	float ty = 0.0f;		// テクスチャの左上Y座標

	int cs;	//色を決定するための添字
	D3DXCOLOR color;

	float npx = title.pos.x - pw * (TEXTURE_DIVIDE_X / 2.0f - 0.5f);	//分割されたものの、左上の座標
	float npy = title.pos.y - ph * (TEXTURE_DIVIDE_Y / 2.0f - 0.5f);

	for (int i = 0; i < TEXTURE_DIVIDE_X; i++)	//縦の分割
	{
		cs = (g_time + i) % TEXTURE_DIVIDE_X;
		color = SetColor(cs);	//色をセット
		for (int j = 0; j < TEXTURE_DIVIDE_Y; j++)	//横の分割
		{
			// スコアの位置やテクスチャー座標を反映
			px = npx + pw * i - sinf(x_rad + add_x_rad * j) * x_curve_size;		// タイトルの表示位置X
			py = npy + ph * j - sinf(y_rad + add_y_rad * i) * y_curve_size;		// タイトルの表示位置Y
			tx = tw * i;		// テクスチャの左上X座標
			ty = th * j;		// テクスチャの左上Y座標
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, color, true);
			//SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	if (g_time > 420 && g_time <= 640)	//フラッシュ的な描写
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		float px = SCREEN_CENTER_X;			//表示位置X
		float py = SCREEN_CENTER_Y;			//表示位置Y
		float pw = SCREEN_WIDTH;			//表示幅
		float ph = SCREEN_HEIGHT;			//表示高さ

		float tw = 1.0f;		// テクスチャの幅
		float th = 1.0f;		// テクスチャの高さ
		float tx = 0.0f;		// テクスチャの左上X座標
		float ty = 0.0f;		// テクスチャの左上Y座標

		float alpha = g_time <= 480 ? (g_time - 420) / 60.0f : (640 - g_time) / 180.0f;	//不透明度をいじる
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, D3DXCOLOR(1.0f, 1.0f, 1.0f, alpha), true);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
	
	if (g_time > 480)	//演出後にプレスエンターを表示
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		float px = 480.0f;					// 表示位置X
		float py = 510.0f;					// 表示位置Y
		float pw = PRESSENTER_WIDTH;			//表示幅
		float ph = PRESSENTER_HEIGHT;			//表示高さ

		float tw = 1.0f;		// テクスチャの幅
		float th = 1.0f;		// テクスチャの高さ
		float tx = 0.0f;		// テクスチャの左上X座標
		float ty = 0.0f;		// テクスチャの左上Y座標

		float alpha = g_time % 20 < 10 ? 1.0f : 0.0f;
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, D3DXCOLOR(1.0f, 1.0f, 1.0f, alpha), true);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}
}

D3DXCOLOR SetColor(int start)	//色を120段階で設定　虹色に
{
	if (start < 20)
	{
		return D3DXCOLOR(1.0f, 0.0f, addcolor*(start - 120), 1.0f);
	}
	else if (start < 40)
	{
		return D3DXCOLOR(1.0f,addcolor*(start - 20), 0.0f, 1.0f);
	}
	else if (start < 60)
	{
		return D3DXCOLOR(1.0f - addcolor*(start - 40), 1.0f, 0.0f, 1.0f);
	}
	else if (start < 80)
	{
		return D3DXCOLOR(0.0f, 1.0f, addcolor*(start - 60), 1.0f);
	}
	else if (start < 100)
	{
		return D3DXCOLOR(0.0f, 1.0f - addcolor*(start - 80), 1.0f, 1.0f);
	}
	else
	{
		return D3DXCOLOR(addcolor*(start - 100), 0.0f, 1.0f, 1.0f);
	}
}





