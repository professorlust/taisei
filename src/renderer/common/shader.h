/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2018, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2018, Andrei Alexeyev <akari@alienslab.net>.
 */

#pragma once
#include "taisei.h"

#include "shader_defs.h"
#include "shader_glsl.h"
#include "shader_spirv.h"

struct ShaderLangInfo {
	ShaderLanguage lang;

	union {
		struct {
			GLSLVersion version;
		} glsl;

		struct {
			SPIRVTarget target;
		} spirv;
	};
};

struct ShaderSource {
	char *content;
	size_t content_size;
	ShaderLangInfo lang;
	ShaderStage stage;
};
