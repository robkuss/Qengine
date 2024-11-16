#!/bin/bash

struct="Qstatus"
header="STATUS_H"
file="src/$struct.h"
hexformat="0x[0-9A-F]+"

# new status entry
entry() {
  tmp_file=$(mktemp) # for modified file content
  insertion_point=""
  done_flag=0

  while IFS= read -r line; do
    prsv_line="$line" # preserve unmodified line content

    if [[ "$line" =~ ^\s*#define\s+$header ]]; then
      continue  # skip header define
    elif [[ "$line" =~ ^\s*#define ]]; then
      while [[ "$line" =~ $hexformat ]]; do
        hex_num="${BASH_REMATCH[0]}"
        # remove the matched part from the line to avoid infinite loop
        line="${line#*"$hex_num"}"

        # find and mark insertion point for new entry
        if (( 0x${hex_num:2} > 0x${2:2} )); then
          insertion_point="$line"
          break
        fi
      done
    fi

    # create and insert new entry before the insertion point
    if [[ -n "$insertion_point" && $done_flag -eq 0 ]]; then
      new_entry=$(printf '#define %s %s(%s, %s)\n' "$1" "$struct" "$2" "$3")
      echo -e "$new_entry" >> "$tmp_file"
      done_flag=1
    fi

    # write the current line to the temporary file
    echo "$prsv_line" >> "$tmp_file"
  done < "$file"

  # move the temporary file back to the original file
  mv "$tmp_file" "$file"
}


# entry point
echo "change status codes"

# shellcheck disable=SC2001
# shellcheck disable=SC2181
while true; do
  read -r input
  case "$input" in
  "help")
    printf "chcodes commands:\n  help\n  new\n  chmsg\n  exit\n";;
  "new")
    read -rp "section: " section
    section=$(echo "$section" | sed 's/.\x08//g')  # remove backspaces

    # search for existing section
    match=$(grep -n "// $section" "$file" | cut -d':' -f1)

    if [ $? -eq 0 ]; then # TODO: this check doesn't work
      # section already defined
      echo "section \"$section\" is already defined in line $match"
      continue # TODO: new status entry at first free spot in section
    else
      # section not yet defined
      echo "creating new section \"$section\""
      read -rp "category [s/w/e/f]: " category
      case "$category" in
      "s")
        echo "adding section to category Success";;
      "w")
        echo "adding section to category Warning";;
      "e")
        echo "adding section to category Non-Fatal Error";;
      "f")
        echo "adding section to category Fatal Error";;
      *)
        echo "choose s (Success), w (Warning), e (Non-Fatal Error) or f (Fatal Error)"
        exit 1;;
      esac
    fi

    read -rp "new status entry [name msg]: " name msg

    # remove backspaces
    name=$(echo "$name" | sed 's/.\x08//g')
    msg=$(echo "$msg" | sed 's/.\x08//g')

    # check if name is already defined
    match=$(grep -nqF "#define $name $struct(" "$file" | cut -d':' -f1)
    if [ $? -eq 0 ]; then
      exis_code=$(grep -oP "(?<=${name}).*" "$file" | grep -oP "$hexformat" | head -n 1)
      echo "WARN: name $name already defined in line $match (code: $exis_code)"
      read -rp "overwrite? [y/n] " proceed
      if [[ "$proceed" == "n" ]]; then
        continue
      else
        sed -i "/${name}/d" "$file"
      fi
    fi

    # create status entry
    entry "$name" "$code" "$msg"
    echo "done"
    ;;
  "exit")
    break;;
  *)
    echo "unknown command, try \"help\"";;
  esac
done
