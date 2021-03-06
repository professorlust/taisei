/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2018, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2018, Andrei Alexeyev <akari@alienslab.net>.
 */

#pragma once
#include "taisei.h"

#include "../api.h"
#include "opengl.h"
#include "resource/texture.h"
#include "../common/backend.h"
#include "common_buffer.h"

typedef struct TextureUnit TextureUnit;

typedef enum BufferBindingIndex {
	GL33_BUFFER_BINDING_ARRAY,
	GL33_BUFFER_BINDING_COPY_WRITE,
	GL33_BUFFER_BINDING_PIXEL_UNPACK,

	GL33_NUM_BUFFER_BINDINGS
} BufferBindingIndex;

// Internal helper functions

GLenum gl33_prim_to_gl_prim(Primitive prim);
void gl33_begin_draw(VertexArray *varr, void **state);
void gl33_end_draw(void *state);

uint gl33_bind_texture(Texture *texture, bool for_rendering);

void gl33_bind_vao(GLuint vao);
void gl33_sync_vao(void);
GLuint gl33_vao_current(void);

void gl33_bind_buffer(BufferBindingIndex bindidx, GLuint gl_handle);
void gl33_sync_buffer(BufferBindingIndex bindidx);
GLuint gl33_buffer_current(BufferBindingIndex bindidx);
GLenum gl33_bindidx_to_glenum(BufferBindingIndex bindidx);

void gl33_set_clear_color(const Color *color);
void gl33_set_clear_depth(float depth);

void gl33_sync_shader(void);
void gl33_sync_texunit(TextureUnit *unit, bool prepare_rendering, bool ensure_active) attr_nonnull(1);
void gl33_sync_texunits(bool prepare_rendering);
void gl33_sync_framebuffer(void);
void gl33_sync_blend_mode(void);
void gl33_sync_cull_face_mode(void);
void gl33_sync_depth_test_func(void);
void gl33_sync_capabilities(void);

void gl33_buffer_deleted(CommonBuffer *cbuf);
void gl33_vertex_buffer_deleted(VertexBuffer *vbuf);
void gl33_vertex_array_deleted(VertexArray *varr);
void gl33_texture_deleted(Texture *tex);
void gl33_framebuffer_deleted(Framebuffer *fb);
void gl33_shader_deleted(ShaderProgram *prog);

extern RendererBackend _r_backend_gl33;
