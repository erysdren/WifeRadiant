/*
   Copyright (C) 2025, Baguettery.
   Copyright (C) 2026, erysdren (it/its).

   This Source Code Form is subject to the terms of the
   Mozilla Public License, v. 2.0. If a copy of the MPL
   was not distributed with this file, You can obtain one
   at https://mozilla.org/MPL/2.0/.
*/

#pragma once

#include <cstdint>
#include <cfloat>
#include <vector>
#include "math/vector.h"

class HotSpotDef {
public:
	/// If any other rects are within this threshold,
	/// they should be used to randomize the result.
	const float aspectErrorMargin = 0.02f;

	// (1.0 = allow 2x bigger or 2x smaller textures)
	const float scaleErrorMargin = 0.20f;

	struct Vec2 {
		uint16_t x, y;
	};

	struct Rect {
		enum Flags : uint8_t {
			EnableRotation = 1 << 0,
			EnableReflection = 1 << 1,
			EnableTiling = 1 << 2,
			AltGroup = 1 << 3
		};
		Vec2 mins, maxs;
		uint8_t flags;

		bool CanRotate() const {
			return this->flags & static_cast<uint8_t>(Flags::EnableRotation);
		}
		bool CanReflect() const {
			return this->flags & static_cast<uint8_t>(Flags::EnableReflection);
		}
		bool CanTile() const {
			return this->flags & static_cast<uint8_t>(Flags::EnableTiling);
		}
		bool IsAltGroup() const {
			return this->flags & static_cast<uint8_t>(Flags::AltGroup);
		}
	};

	Vec2 size;
	std::vector<Rect> rects;

	int MatchRandomBestRect(float targetAspect, float targetScale, bool altGroup, bool* out_isRotated, float* out_aspectErr, float* out_scalingErr) const {
		if (this->rects.empty()) return -1;
		float logTargetScale = std::log2f(targetScale);

		std::size_t rectCount = this->rects.size();
		std::vector<bool> rectsRotated(rectCount);
		std::vector<float> aspectErrors(rectCount);
		std::vector<float> scaleErrors(rectCount);
		float bestAspectError = INFINITY;

		// Calculate the aspect ratio and scaling errors of each rect
		for (std::size_t i = 0; i < rectCount; i++) {
			Rect rect = this->rects[i];
			if (rect.IsAltGroup() != altGroup) continue;
			float width = static_cast<float>(rect.maxs.x - rect.mins.x);
			float height = static_cast<float>(rect.maxs.y - rect.mins.y);
			float maxDim = (width > height ? width : height);
			if (height < FLT_EPSILON || width < FLT_EPSILON) continue;

			float aspect = width / height;
			rectsRotated[i] = rect.CanRotate() && ((aspect > 1) != (targetAspect > 1));
			if (rectsRotated[i]) aspect = 1.0f / aspect;

			float aspectError = fabs(aspect - targetAspect);
			aspectErrors[i] = aspectError;
			scaleErrors[i] = fabs(std::log2f(maxDim) - logTargetScale);
			if (aspectError < bestAspectError) bestAspectError = aspectError;
		}

		std::vector<int> aspectMatches;
		float bestScaleError = INFINITY;

		// Pick only the best aspect matches within an error margin
		for (std::size_t i = 0; i < rectCount; i++) {
			if (this->rects[i].IsAltGroup() != altGroup) continue;
			if (aspectErrors[i] > bestAspectError + aspectErrorMargin) continue;
			if (scaleErrors[i] < bestScaleError) bestScaleError = scaleErrors[i];
			aspectMatches.push_back(i);
		}

		std::vector<int> finalMatches;

		// Pick only the best scaling matches within an error margin
		for (std::size_t i = 0; i < aspectMatches.size(); i++) {
			if (scaleErrors[aspectMatches[i]] > bestScaleError + scaleErrorMargin) continue;
			finalMatches.push_back(aspectMatches[i]);
		}

		if (finalMatches.size() == 0) return -1;
		int result = finalMatches[std::rand() % finalMatches.size()];

		if (out_isRotated != NULL) *out_isRotated = rectsRotated[result];
		if (out_aspectErr != NULL) *out_aspectErr = aspectErrors[result];
		if (out_scalingErr != NULL) *out_scalingErr = scaleErrors[result];
		return result;
	}

	void GetUvMinMax(int i, std::size_t width, std::size_t height, Vector2* vMins, Vector2* vMaxs) const {
		const Rect* rect = &this->rects[i];
		vMins->x() = static_cast<float>(rect->mins.x) / static_cast<float>(width);
		vMins->y() = static_cast<float>(rect->mins.y) / static_cast<float>(height);
		vMaxs->x() = static_cast<float>(rect->maxs.x) / static_cast<float>(width);
		vMaxs->y() = static_cast<float>(rect->maxs.y) / static_cast<float>(height);
		return;
	}
};
