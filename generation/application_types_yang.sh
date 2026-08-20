#!/bin/bash

# Copyright (c) 2020, AT&T Intellectual Property. All rights reserved.

# Create a new/updated vyatta-ndpi-applications-types.yang file,
# writing to stdout.
# ./application_list must exist.
#
# If OLD_YANG_FILE is set, the file will be read in, have the existing types
# removed and used instead of a fresh file.
#
# Revision notes will be taken from stdin and added to the resulting yang with
# today's date as a new revision.

# If OLD_YANG_FILE set, read it, cut out the pre-existing types and use the
# remainder as the header.
# Otherwise, use the heredoc.
heading=
if [ -z "${OLD_YANG_FILE}" ]; then
	read -r -d '' heading << EndOfHeader
module vyatta-ndpi-application-types-v1 {
	namespace "urn:vyatta.com:mgmt:vyatta-ndpi-application-types:1";
	prefix vyatta-ndpi-application-types-v1;

	organization "AT&T Inc.";
	contact	"AT&T
		 Postal: 208 S. Akard Street
		         Dallas, TX 75202, USA
		 Web: www.att.com";

	description
		"Copyright (c) 2020, AT&T Intellectual Property
		 All rights reserved.

		 Redistribution and use in source and binary forms,
		 with or without modification, are permitted provided
		 that the following conditions are met:

		 1. Redistributions of source code must retain the
		    above copyright notice, this list of conditions and
		    the following disclaimer.
		 2. Redistributions in binary form must reproduce
		    the above copyright notice, this list of conditions
		    and the following disclaimer in the documentation
		    and/or other materials provided with the distribution.
		 3. Neither the name of the copyright holder nor the
		    names of its contributors may be used to endorse or
		    promote products derived from this software without
		    specific prior written permission.

		 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS
		 AND CONTRIBUTORS 'AS IS' AND ANY EXPRESS OR IMPLIED
		 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
		 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
		 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
		 THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
		 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
		 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
		 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
		 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
		 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
		 IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
		 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
		 USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
		 OF SUCH DAMAGE.

		 SPDX-License-Identifier: BSD-3-Clause

		 Application types associated with the nDPI engine.

		 This file was auto-generated with
		 generation/application_types_yang.sh";
EndOfHeader
else
	heading=$(sed '1,/typedef application-types /!d' ${OLD_YANG_FILE} | head -n -1)
fi

# Get the all types from applications_list
new_list=$(./applications_list --types | sort -u)

# Get the types from the old yang file, which may be empty
if [ -n "$OLD_YANG_FILE" ]; then
  old_list=$(sed '1,/typedef application-types/d' ${OLD_YANG_FILE} | grep -o 'enum ".*"' | cut -d '"' -f 2)

  # Extract the new types and append them to the old list
  TMP=/tmp/app_type_yang.tmp
  echo "$old_list" > $TMP
  apps=$(echo "${old_list}" && comm -13 $TMP - <<< $new_list | sed '/^[[:blank:]]*$/d')
  rm -f $TMP
else
  apps=$new_list
fi

# Convert each type into an application_types enum variant with correct indentation.
yang="$(awk -F, '{print "\t\t\t" "enum \"" $1 "\";"}' <<< $apps)"

# Prompt the user for revision notes.
>&2 echo "Enter revision notes:"
changes=$(</dev/stdin)

# Glue everything together and dump it to stdout.
echo -e "$heading\n"
echo -en "\trevision $(date -I) {\n\t\tdescription \""
echo "$changes" | { sed '2,$ s/^/\t\t\t/g' | head -c -1; echo "\";"; }
echo -e "\t}\n"
echo -e "\ttypedef application-types {"
echo -e "\t\t/* New enums MUST only be added to the end of this list."
echo -e "\t\t * Old enums MAY be deprecated or obsoleted, but must not be removed."
echo -e "\t\t * It's not important to preserve alphabetical order."
echo -e "\t\t */"
echo -e "\t\ttype enumeration {"
echo "$yang"
echo -e "\t\t}\n\t}\n}"
