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


void show_categories(void)
{
	struct ndpi_detection_module_struct *ndpi_info_mod = ndpi_init_detection_module(ndpi_no_prefs);
	if (ndpi_info_mod == NULL) return;

	for (int i = NDPI_PROTOCOL_CATEGORY_UNSPECIFIED + 1;
	     i < NDPI_PROTOCOL_NUM_CATEGORIES;
	     i++) {
		const char *category_name = ndpi_category_get_name(ndpi_info_mod, i);
		if ( (i >= CUSTOM_CATEGORY_MINING &&
		      i <= CUSTOM_CATEGORY_ANTIMALWARE) ||
		     ndpi_is_custom_category(i) ||
		     (category_name == "") ) {
			continue;
		}

		printf("%s\n", strlwr(category_name));
	}

	ndpi_exit_detection_module(ndpi_info_mod);
}


void show_protocols(void)
{
	struct ndpi_detection_module_struct *ndpi_info_mod = ndpi_init_detection_module(ndpi_no_prefs);
	if (ndpi_info_mod == NULL) return;

	u_int num_protocols = ndpi_get_ndpi_num_supported_protocols(ndpi_info_mod);

	for (int i = NDPI_PROTOCOL_UNKNOWN;
	     i < num_protocols;
	     i++) {
		const char *proto_name = ndpi_get_proto_name(ndpi_info_mod, i);
		if (proto_name == "") {
			continue;
		}

		printf("%s\n", strlwr(proto_name));
	}

	ndpi_exit_detection_module(ndpi_info_mod);
}


void show_mapping(void)
{
	struct ndpi_detection_module_struct *ndpi_info_mod = ndpi_init_detection_module(ndpi_no_prefs);
	if (ndpi_info_mod == NULL) return;

	u_int num_protocols = ndpi_get_ndpi_num_supported_protocols(ndpi_info_mod);

	ndpi_proto_defaults_t *pd = ndpi_get_proto_defaults(ndpi_info_mod);

	for (int i = NDPI_PROTOCOL_UNKNOWN; i < num_protocols; i++) {
		const char *proto_name = ndpi_get_proto_name(ndpi_info_mod, i);
		if (proto_name == "") {
			continue;
		}

		ndpi_protocol_category_t category = pd[i].protoCategory;
		const char *category_name = ndpi_category_get_name(ndpi_info_mod, category);
		printf("%s %s\n", strlwr(proto_name), strlwr(category_name));
	}

	ndpi_exit_detection_module(ndpi_info_mod);
}


void dump_protocols(void)
{
	struct ndpi_detection_module_struct *ndpi_info_mod = ndpi_init_detection_module(ndpi_no_prefs);
	if (ndpi_info_mod == NULL) return;

	ndpi_dump_protocols(ndpi_info_mod);

	ndpi_exit_detection_module(ndpi_info_mod);
}


void usage(char *name)
{
	printf("Usage: %s [--names | --types | --mapping | --dump | --revision]\n", name);
}


int main(int argc, char **argv)
{
	if (argc < 2) {
		usage(argv[0]);
		return -1;
	}

	if (!strcmp(argv[1], "--revision"))
		printf("Revision: %s\n", ndpi_revision());

	else if (!strcmp(argv[1], "--types"))
		show_categories();

	else if (!strcmp(argv[1], "--names"))
		show_protocols();

	else if (!strcmp(argv[1], "--mapping"))
		show_mapping();

	else if (!strcmp(argv[1], "--dump"))
		dump_protocols();

	else {
		usage(argv[0]);
		return -1;
	}
}
