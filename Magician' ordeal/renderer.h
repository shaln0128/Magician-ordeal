//=============================================================================
//
// レンダリング処理 [renderer.h]
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#pragma once

#include "main.h"

//*********************************************************
// 構造体
//*********************************************************

enum BLEND_MODE
{
	BLEND_MODE_ALPHABLEND,		//ブレンド無し
	BLEND_MODE_ADD,			//加算ブレンド

	BLEDD_MODE_NUM
};

// 頂点構造体
struct VERTEX_3D
{
    D3DXVECTOR3 Position;
    D3DXVECTOR3 Normal;
    D3DXCOLOR  Diffuse;
    D3DXVECTOR2 TexCoord;
};


// マテリアル構造体
struct MATERIAL
{
	D3DXCOLOR	Ambient;
	D3DXCOLOR	Diffuse;
	D3DXCOLOR	Specular;
	D3DXCOLOR	Emission;
	float		Shininess;
	float		Dummy[3];//16bit境界用
};

struct VIEWPORT
{
	float	mag;
	D3DXVECTOR3 topleft;
};
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitRenderer(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void UninitRenderer(void);

void Clear(void);
void Present(void);

ID3D11Device *GetDevice( void );
ID3D11DeviceContext *GetDeviceContext( void );

void SetDepthEnable( bool Enable );

void SetWorldViewProjection2D( void );
void SetWorldMatrix( D3DXMATRIX *WorldMatrix );
void SetViewMatrix( D3DXMATRIX *ViewMatrix );
void SetProjectionMatrix( D3DXMATRIX *ProjectionMatrix );


void SetMaterial( MATERIAL Material );

void SetViewports();
VIEWPORT *GetViewports(void);
void ResetViewports(void);

void SetBlendState(BLEND_MODE bm);
