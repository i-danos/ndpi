/*
 * Copyright (c) 2020, AT&T Intellectual Property.  All rights reserved.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdio.h>
#include <string.h>
#include "ndpi_api.h"


char *strlwr(const char *str)
{
	char *dup = strdup(str);

	for (char *p = dup; *p; ++p) *p = tolower(*p);

	return dup;
}


/*
 * Show the application names associated with the given application type.
 */
void show_type(char *type)
{
	struct ndpi_detection_module_struct *ndpi_info_mod = ndpi_init_detection_module(ndpi_no_prefs);
	if (ndpi_info_mod == NULL) return;

	int id = ndpi_get_category_id(ndpi_info_mod, type);
	if (id == -1) return;

	u_int num_protocols = ndpi_get_ndpi_num_supported_protocols(ndpi_info_mod);

	ndpi_proto_defaults_t *pd = ndpi_get_proto_defaults(ndpi_info_mod);

	printf("%s includes the following applications:\n", type);

	for (int i = NDPI_PROTOCOL_UNKNOWN; i < num_protocols; i++) {
		ndpi_protocol_category_t category = pd[i].protoCategory;

		if (category == id) {
			const char *proto_name = strlwr(ndpi_get_proto_name(ndpi_info_mod, i));
			if (proto_name == "") {
				continue;
			}

			printf("%s ", proto_name);
		}
	}

	printf("\n");
	ndpi_exit_detection_module(ndpi_info_mod);
}


/*
 * Show the application type associated with the given application name.
 */
void show_name(char *name)
{
	struct ndpi_detection_module_struct *ndpi_info_mod = ndpi_init_detection_module(ndpi_no_prefs);
	if (ndpi_info_mod == NULL) return;

	int id = ndpi_get_protocol_id(ndpi_info_mod, name);
	if (id == -1) return;

	ndpi_proto_defaults_t *pd = ndpi_get_proto_defaults(ndpi_info_mod);

	ndpi_protocol_category_t category = pd[id].protoCategory;
	const char *category_name = strlwr(ndpi_category_get_name(ndpi_info_mod, category));

	printf("%s is included in the following application types:\n%s\n",
		name, category_name);

	ndpi_exit_detection_module(ndpi_info_mod);
}


void usage(char *name)
{
	printf("Usage: %s [name | type] <application>\n", name);
}


int main(int argc, char **argv)
{
	if (argc != 3) {
		usage(argv[0]);
		return -1;
	}

	if (!strcmp(argv[1], "type")) {
		show_type(argv[2]);
	}
	else if (!strcmp(argv[1], "name")) {
		show_name(argv[2]);
	}
	else {
		usage(argv[0]);
		return -1;
	}
}
