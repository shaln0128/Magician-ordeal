//=============================================================================
//
// 召喚処理【summon.cpp】
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "summon.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(30)	// キャラサイズ
#define TEXTURE_HEIGHT				(60)	// キャラサイズ
#define TEXTURE_MAX					(3)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(10)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)

#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X)	// アニメーションパターン数
#define ANIM_WAIT					(6)		// アニメーションの切り替わるWait値
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/summon.png",
	"data/TEXTURE/summon_red.png",
	"data/TEXTURE/summon_blue.png",
};

static EFFECT summon[SUMMON_MAX];	//召喚陣の構造体
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSummon(void)
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

	// 召喚陣の初期化
	for (int i = 0; i < SUMMON_MAX; i++)
	{
		summon[i].use = false;
		summon[i].w = TEXTURE_WIDTH;
		summon[i].h = TEXTURE_HEIGHT;
		summon[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		summon[i].countAnim = 0;
		summon[i].patternAnim = 0;
		summon[i].texNo = 0;
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSummon(void)
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
void UpdateSummon(void)
{
	for (int i = 0; i < SUMMON_MAX; i++)
	{
		if (summon[i].use == true)		// 召喚陣が使われている？
		{						// Yes
			// アニメーション  
			summon[i].countAnim++;
			if ((summon[i].countAnim % ANIM_WAIT) == 0)
			{
				// パターンの切り替え
				summon[i].patternAnim++;

				if (summon[i].patternAnim == ANIM_PATTERN_NUM)	//最後まで表示したら消す
				{
					summon[i].use = false;
				}
			}
		}
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	/*char *str = GetDebugStr();
	sprintf(&str[strlen(str)], " fire%d", fire_count);*/
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSummon(void)
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

	for (int i = 0; i < SUMMON_MAX; i++)
	{
		if (summon[i].use == true)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[summon[i].texNo]);
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			float dx = summon[i].pos.x;
			float dy = summon[i].pos.y;
			float dw = summon[i].w;
			float dh = summon[i].h;

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = tw * summon[i].patternAnim;// テクスチャの左上X座標
			float ty = 0.0f;// テクスチャの左上Y座標

			SetSpriteColor(g_VertexBuffer, dx, dy, dw, dh, tx, ty, tw, th, D3DXCOLOR(0.5f,0.5f,0.5f,1.0f), true);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	SetBlendState(BLEND_MODE_ALPHABLEND);
}

EFFECT *GetSummon(void)	//召喚陣の構造体へのアドレス
{
	return(&summon[0]);
}

void SetSummon(D3DXVECTOR3 pos, int type)	//召喚陣のセット
{
	for (int i = 0; i < SUMMON_MAX; i++)
	{
		if (summon[i].use == false)
		{
			summon[i].use = true;
			summon[i].pos = pos + D3DXVECTOR3(0.0f, -10.0f,0.0f);	//少し下に表示
			summon[i].countAnim = 0;
			summon[i].patternAnim = 0;
			summon[i].texNo = type;
			return;
		}
	}

	return;
}






