/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2004-2009  Marcel Holtmann <marcel@holtmann.org>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
extern struct bluetooth_plugin_desc __bluetooth_builtin_hciops;
extern struct bluetooth_plugin_desc __bluetooth_builtin_mgmtops;
extern struct bluetooth_plugin_desc __bluetooth_builtin_service;
static struct bluetooth_plugin_desc *__bluetooth_builtin[] = {
	&__bluetooth_builtin_mgmtops,
	//&__bluetooth_builtin_hciops,
	&__bluetooth_builtin_service,
	NULL };