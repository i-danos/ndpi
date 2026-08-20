# Generation scripts

Scripts to generate yang files and show-command scripts.

## Usage

The scripts are `application_names_yang.sh`, `application_types_yang.sh`.

These accept the `OLD_YANG_FILE` environment variable to use an old yang
file's "header" information (description, copyright, revisions, etc) rather
than starting from fresh.

These will prompt the user via stderr for revision notes, which are
pulled from /dev/stdin, so can be piped in.

## Examples

Generate a new nDPI application names yang file, using `/path/base.yang` as a base,
"Revision notes" as the notes for the revision, and writing it to
`new_applications.yang`.
`application_types_yang.sh` is invoked similarly.

```
echo "Revision Notes" | OLD_YANG_FILE=/path/base.yang ./application_names_yang.sh > new_applications.yang
```
