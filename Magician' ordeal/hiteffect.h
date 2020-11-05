//=============================================================================
//
// �q�b�g�G�t�F�N�g�̏����yhiteffect.cpp�z
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "summon.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define HITEFFECT_MAX					(5)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitHitEffect(void);
void UninitHitEffect(void);
void UpdateHitEffect(void);
void DrawHitEffect(void);

EFFECT *GetHitEffect(void);
void SetHitEffect(D3DXVECTOR3 pos);


