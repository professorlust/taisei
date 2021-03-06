/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2018, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2018, Andrei Alexeyev <akari@alienslab.net>.
 */

#pragma once
#include "taisei.h"

enum {
	VFS_SYSPATH_MOUNT_READONLY = (1 << 0),
	VFS_SYSPATH_MOUNT_MKDIR = (1 << 1),
};

bool vfs_mount_syspath(const char *mountpoint, const char *fspath, uint flags)
	attr_nonnull(1, 2) attr_nodiscard;
