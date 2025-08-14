#!/bin/bash

# This script finds and removes duplicate library files in a specified directory.
# It uses the md5sum utility to generate a checksum for each file and then compares them.

# Check if a directory path is provided as an argument.
if [ -z "$1" ]; then
    echo "Usage: $0 <path_to_lib_directory>"
    echo "Example: $0 AppDir/usr/lib"
    exit 1
fi

LIB_DIR="$1"

# Check if the directory exists.
if [ ! -d "$LIB_DIR" ]; then
    echo "Error: Directory '$LIB_DIR' not found."
    exit 1
fi

# Define a temporary file to store checksums and filenames.
TEMP_FILE=$(mktemp)

# Iterate through all files in the directory and its subdirectories.
# This loop generates an MD5 checksum for each file and stores it with the filename.
find "$LIB_DIR" -type f -print0 | while IFS= read -r -d $'\0' file; do
    # Skip symbolic links and special files.
    if [ ! -f "$file" ]; then
        continue
    fi
    # Generate the checksum and print it along with the file path to the temp file.
    md5sum "$file" >> "$TEMP_FILE"
done

# Now, we process the temporary file to find and remove duplicates.
# We'll use awk to find unique checksums and list the files associated with them.
# The `a[$1]++` part creates an associative array where the key is the checksum.
# If a checksum is seen more than once, its value in the array will be > 0.
# The `a[$1]++ > 0` condition ensures that only duplicate lines are printed.
awk 'a[$1]++ > 0 {print $2}' "$TEMP_FILE" | while read duplicate_file; do
    echo "Removing duplicate: $duplicate_file"
    rm "$duplicate_file"
done

# Clean up the temporary file.
rm "$TEMP_FILE"

echo "Cleanup complete."
