/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#pragma once

/** \file
 * \ingroup bke
 */

#ifdef __cplusplus
extern "C" {
#endif

struct Object;

typedef struct GeometrySetC GeometrySetC;

void BKE_geometry_set_user_add(GeometrySetC *geometry_set_c);
void BKE_geometry_set_user_remove(GeometrySetC *geometry_set_c);

bool BKE_geometry_set_has_instances(const GeometrySetC *geometry_set_c);

int BKE_geometry_set_instances(const GeometrySetC *geometry_set_c,
                               float (**r_positions)[3],
                               struct Object ***r_objects);

#ifdef __cplusplus
}
#endif
