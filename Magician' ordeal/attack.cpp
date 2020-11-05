//=============================================================================
//
// 攻撃処理 [attack.cpp]
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#include "attack.h"
#include "sprite.h"
#include "enemy.h"
#include "ghost.h"
//*****************************************************************************
// マクロ定義
//****************************************************************************
#define TEXTURE_SIZE				(20)	// サイズ
#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値

#define pi							float(3.14159265)

#define EMITTER_ROTATION			(4) //エミッター（先端）の回転速度
#define STAR_ROTATION				(8)//パーティクルの回転速度
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/star.png",
};

static PARTICLE attack[ATTACK_MAX];						// エミッターの構造体
static PARTICLE star[STAR_MAX];							// パーティクルの構造体

static float rad = pi / 180.0f; //　ラジアン変換用
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitAttack(void)
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


	// エミッター構造体の初期化
	for (int i = 0; i < ATTACK_MAX; i++)
	{
		attack[i].use = false;	// 未使用（発射されていない弾）
	
		attack[i].size = TEXTURE_SIZE;
		attack[i].pos = D3DXVECTOR3(0, 0.0f, 0.0f);
		attack[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		attack[i].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 移動量を初期化
		attack[i].color = D3DXCOLOR(1.0f, 0.8f, 0.1f,1.0f);	//　黄色
		attack[i].time = 0;
	}

	for (int i = 0; i < STAR_MAX; i++)
	{
		star[i].use = false;	// 未使用（発射されていない弾）

		star[i].size = TEXTURE_SIZE;
		star[i].pos = D3DXVECTOR3(0, 0.0f, 0.0f);
		star[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		star[i].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 移動量を初期化
		star[i].color = D3DXCOLOR(1.0f, 0.8f, 0.1f, 1.0f);	//黄色
		star[i].time = 0;
	}	

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitAttack(void)
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
void UpdateAttack(void)
{
	ENEMY *enemy = GetEnemy();
	ENEMY *ghost = GetGhost();

	for (int i = 0; i < ATTACK_MAX; i++)	//エミッターの更新処理
	{
		if (attack[i].use == true)	// このエミッターが使われている？
		{	
			//　一番近い敵をソート
			float dif;
			D3DXVECTOR3 b_dif;
			float min_dif = 1000000;
			D3DXVECTOR3 min_b_dif;

			for (int j = 0; j < GHOST_MAX; j++)
			{
				if (ghost[j].use == true && ghost[j].invincible_time == 0)
				{
					b_dif = ghost[j].pos - attack[i].pos;

					dif = b_dif.x * b_dif.x + b_dif.y * b_dif.y;

					if (dif < min_dif)	//比較して距離が短かったら入れ替え
					{
						min_dif = dif;
						min_b_dif = b_dif;
					}
				}
			}

			if (min_dif == 1000000)	//もしゴーストが一体もいなければメイジへと飛ばす
			{
				min_b_dif = enemy[0].pos - attack[i].pos;
			}
			float rot = atan2f(min_b_dif.y, min_b_dif.x);

			attack[i].move.x = cosf(rot) * 3.0f;		//移動量を保存
			attack[i].move.y = sinf(rot) * 3.0f;

			
			attack[i].pos += attack[i].move;			// エミッターの移動処理

			attack[i].rot.z += EMITTER_ROTATION * rad;	//回転処理

			attack[i].time++;							//時間の更新

			if (attack[i].time % 3 == 0)				//3Fに1回、パーティクルを生成
			{
				for (int j = 0; j < STAR_MAX; j++)
				{
					if (star[j].use == true) continue;
					else
					{
						star[j].use = true;
						star[j].pos = attack[i].pos + D3DXVECTOR3(float(rand() % 10 - 5), float(rand() % 10 - 5), 0.0f);	//エミッターの位置から乱数でずらす
						star[j].move = -attack[i].move * 0.2f;	//エミッターの反対側に0.2倍の速さで移動させる
						star[j].size = TEXTURE_SIZE;
						star[j].time = 60;
						break;
					}
				}
			}

		}

	}


	for (int j = 0; j < STAR_MAX; j++)	//パーティクルの更新処理
	{
		if (star[j].use == false) continue;
		
		star[j].pos += star[j].move;
		star[j].size *= 0.95f;					//大きさを5％縮小
		star[j].rot.z += STAR_ROTATION * rad;	//回転
		star[j].time--;							//生存時間

		if (star[j].time <= 0)					//0になったら消す
		{
			star[j].use = false;
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawAttack(void)
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

	for (int i = 0; i < ATTACK_MAX; i++)
	{
		if (attack[i].use == true)			// このエミッターが使われている？
		{									// Yes
			//エミッターの位置やテクスチャー座標を反映
			float px = attack[i].pos.x;		// エミッターの表示位置X
			float py = attack[i].pos.y;		// エミッターの表示位置Y
			float pw = attack[i].size;		// エミッターの表示幅
			float ph = attack[i].size;		// エミッターの表示高さ

			D3DXCOLOR color = attack[i].color;
			float pb_rad = attack[i].rot.z;

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer,
				px, py, pw, ph,
				0.0f, 0.0f, 1.0f, 1.0f,
				color,pb_rad);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	for (int i = 0; i < STAR_MAX; i++)
	{
		if (star[i].use == true)			// このパーティクルが使われている？
		{									// Yes
			//パーティクルの位置やテクスチャー座標を反映
			float px = star[i].pos.x;		// パーティクルの表示位置X
			float py = star[i].pos.y;		// パーティクルの表示位置Y
			float pw = star[i].size;		// パーティクルの表示幅
			float ph = star[i].size;		// パーティクルの表示高さ

			D3DXCOLOR color = star[i].color;
			float pb_rad = star[i].rot.z;

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer,
				px, py, pw, ph,
				0.0f, 0.0f, 1.0f, 1.0f,
				color, pb_rad);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	SetBlendState(BLEND_MODE_ALPHABLEND);

}


//=============================================================================
// エミッター構造体の先頭アドレスを取得
//=============================================================================
PARTICLE *GetAttack(void)
{
	return &attack[0];
}

//=============================================================================
// エミッターの発射設定
//=============================================================================
void SetAttack(D3DXVECTOR3 pos,DIRECTION vect)
{
	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < ATTACK_MAX; i++)
	{
		if (attack[i].use == false)		// 未使用状態のエミッターを見つける
		{
			attack[i].use = true;			// 使用状態へ変更する
			attack[i].pos = pos;			// 座標をセット
			attack[i].time = 0;

			switch (vect)
			{
			case DOWN:
				attack[i].pos += D3DXVECTOR3(0.0f, 30.0f, 0.0f);
				break;
			case LEFT:
				attack[i].pos += D3DXVECTOR3(-20.0f, 0.0f, 0.0f);
				break;
			case RIGHT:
				attack[i].pos += D3DXVECTOR3(20.0f, 0.0f, 0.0f);
				break;
			case UP:
				attack[i].pos += D3DXVECTOR3(0.0f, -30.0f, 0.0f);
				break;
			}

			return;							// 1発セットしたので終了する
		}
	}

	return;
}


