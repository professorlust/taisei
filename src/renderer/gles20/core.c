/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2018, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2018, Andrei Alexeyev <akari@alienslab.net>.
 */

#include "taisei.h"

#include "core.h"
#include "texture.h"
#include "../gl33/core.h"
#include "../glcommon/debug.h"
#include "../glcommon/vtable.h"

static void gles20_init_context(SDL_Window *w) {
	GLVT_OF(_r_backend_gl33).init_context(w);
	gles20_init_texformats_table();
	// glcommon_require_extension("GL_OES_depth_texture");
}

static void gles_init(RendererBackend *gles_backend, int major, int minor) {
	// omg inheritance
	RendererFuncs original_funcs;
	memcpy(&original_funcs, &gles_backend->funcs, sizeof(original_funcs));
	memcpy(&gles_backend->funcs, &_r_backend_gl33.funcs, sizeof(gles_backend->funcs));
	gles_backend->funcs.init = original_funcs.init;
	gles_backend->funcs.screenshot = original_funcs.screenshot;

	glcommon_setup_attributes(SDL_GL_CONTEXT_PROFILE_ES, major, minor, 0);
	glcommon_load_library();
}

static void gles20_init(void) {
	gles_init(&_r_backend_gles20, 2, 0);
}

static void gles30_init(void) {
	gles_init(&_r_backend_gles30, 3, 0);
}

static bool gles20_screenshot(Pixmap *out) {
	IntRect vp;
	r_framebuffer_viewport_current(NULL, &vp);
	out->width = vp.w;
	out->height = vp.h;
	out->format = PIXMAP_FORMAT_RGBA8;
	out->origin = PIXMAP_ORIGIN_BOTTOMLEFT;
	out->data.untyped = pixmap_alloc_buffer_for_copy(out);
	glReadPixels(vp.x, vp.y, vp.w, vp.h, GL_RGBA, GL_UNSIGNED_BYTE, out->data.untyped);
	return true;
}

RendererBackend _r_backend_gles20 = {
	.name = "gles20",
	.funcs = {
		.init = gles20_init,
		.screenshot = gles20_screenshot,
	},
	.custom = &(GLBackendData) {
		.vtable = {
			.texture_type_info = gles20_texture_type_info,
			.init_context = gles20_init_context,
		}
	},
};

RendererBackend _r_backend_gles30 = {
	.name = "gles30",
	.funcs = {
		.init = gles30_init,
		.screenshot = gles20_screenshot,
	},
	.custom = &(GLBackendData) {
		.vtable = {
			.texture_type_info = gles20_texture_type_info,
			.init_context = gles20_init_context,
		}
	},
};
