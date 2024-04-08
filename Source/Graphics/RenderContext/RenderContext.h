#pragma once

#include "ModelContext.h"
#include "PostProcessContext.h"

// レンダーコンテキスト
struct RenderContext
{
	// モデル描画情報
	WaterSurfaceData			waterSurface;

	// ポストプロセス情報
	PostProcessContext		postProcess;
};
