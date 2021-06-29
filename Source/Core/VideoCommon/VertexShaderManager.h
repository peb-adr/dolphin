// Copyright 2013 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#pragma once

#include "VideoCommon/ConstantManager.h"
#include "VideoCommon/VertexShaderGen.h"

class PointerWrap;

void UpdateProjectionHack(int iParams[], std::string sParams[]);

// The non-API dependent parts.
class VertexShaderManager
{
public:
	static void Init();
	static void Dirty();
	static void Shutdown();
	static void DoState(PointerWrap &p);

	// constant management
	static void SetConstants();

	static void InvalidateXFRange(int start, int end);
	static void SetTexMatrixChangedA(u32 value);
	static void SetTexMatrixChangedB(u32 value);
	static void SetViewportChanged();
	static void SetProjectionChanged();
	static void SetMaterialColorChanged(int index, u32 color);

	static void TranslateView(float x, float y, float z = 0.0f);
	static void RotateView(float x, float y);
	static void ResetView();
	
	// THC98 - new functions
	static float GetTranslation(int i);
	static float GetRotation(int i);

	static VertexShaderConstants constants;
	static bool dirty;
};
