//=============================================================================
//
// フェード処理 [fade.cpp]
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "bg.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(1)				// テクスチャの数

#define	FADE_RATE					(0.02f)			// フェード係数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/black.png",
};


static bool						g_Use;						// true:使っている  false:未使用
static float					g_w, g_h;					// 幅と高さ
static D3DXVECTOR3				g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

FADE							g_Fade = FADE_IN;			// フェードの状態
MODE							g_ModeNext;					// 次のモード
D3DXCOLOR						g_Color;					// フェードのカラー（α値）


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitFade(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
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
	g_Use = true;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_TexNo = 0;

	g_Fade = FADE_IN;
	g_Color = D3DXCOLOR(1.0, 1.0, 1.0, 1.0);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitFade(void)
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
void UpdateFade(void)
{

	if (g_Fade != FADE_NONE)
	{// フェード処理中
		if (g_Fade == FADE_OUT)
		{// フェードアウト処理
			g_Color.a += FADE_RATE;		// α値を加算して画面を消していく
			if (g_Color.a >= 1.0f)
			{
				// 鳴っている曲を全部止める
				StopSound();

				// フェードイン処理に切り替え
				g_Color.a = 1.0f;
				SetFade(FADE_IN, g_ModeNext);

				// モードを設定
				SetMode(g_ModeNext);
				SetBg(g_ModeNext);
			}

		}
		else if (g_Fade == FADE_IN)
		{// フェードイン処理
			g_Color.a -= FADE_RATE;		// α値を減算して画面を浮き上がらせる
			if (g_Color.a <= 0.0f)
			{
				// フェード処理終了
				g_Color.a = 0.0f;
				SetFade(FADE_NONE, g_ModeNext);
			}

		}
	}


#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawFade(void)
{
	if (g_Fade == FADE_NONE) return;	// フェードしないのなら描画しない

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

	// タイトルの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f,g_Color,true);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}


//=============================================================================
// 頂点データ設定
//=============================================================================
//static void SetVertex(float X, float Y, float Width, float Height, float U, float V, float UW, float VH)
//{
//	D3D11_MAPPED_SUBRESOURCE msr;
//	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
//
//	VERTEX_3D *vertex = (VERTEX_3D*)msr.pData;
//
//	vertex[0].Position = D3DXVECTOR3(X, Y, 0.0f);
//	vertex[0].Diffuse  = g_Color;
//	vertex[0].TexCoord = D3DXVECTOR2(U, V);
//
//	vertex[1].Position = D3DXVECTOR3(X + Width, Y, 0.0f);
//	vertex[1].Diffuse  = g_Color;
//	vertex[1].TexCoord = D3DXVECTOR2(U + UW, V);
//
//	vertex[2].Position = D3DXVECTOR3(X, Y + Height, 0.0f);
//	vertex[2].Diffuse  = g_Color;
//	vertex[2].TexCoord = D3DXVECTOR2(U, V + VH);
//
//	vertex[3].Position = D3DXVECTOR3(X + Width, Y + Height, 0.0f);
//	vertex[3].Diffuse  = g_Color;
//	vertex[3].TexCoord = D3DXVECTOR2(U + UW, V + VH);
//
//	GetDeviceContext()->Unmap(g_VertexBuffer, 0);
//
//}


//=============================================================================
// フェードの状態設定
//=============================================================================
void SetFade(FADE fade, MODE modeNext)
{
	g_Fade = fade;
	g_ModeNext = modeNext;
}

//=============================================================================
// フェードの状態取得
//=============================================================================
FADE GetFade(void)
{
	return g_Fade;
}



