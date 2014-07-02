#!/bin/sh

# build then run this

TOTAL=0
PASSED=0
FAILED=0

C_RED="\e[31m"
C_GREEN="\e[32m"
C_END="\e[39m"

EMU_BIN="emu/emu"

hex2string () {
  I=0
  HRES=""
  while [ $I -lt ${#1} ];
  do
    echo -en "\x"${1:$I:2}
    let "I += 2"
  done
}

run_test() {
  echo -n " '$DESC' "
  FCODE="$(echo "$CODE"|perl -pe 's/([0-9a-f]{2})/chr hex $1/gie')"
  RES="$(emu/emu "$FCODE")"

  # IF stack random is checked in tests
  # then unrandomize the stacks in res+expect
  if [[ $RANDOM_STACK == 1 ]]; then
    RES="$(echo "${RES}" | sed 's/stack\[.*\]/stack\[RANDOM\]/g')"
    EXPECT="$(echo "${EXPECT}" | sed 's/stack\[.*\]/stack\[RANDOM\]/g')"
  fi
  # Remove verbose string "Function >>>> %s %.nx...etc"
  RES="$(echo "${RES}" | sed 's/.*>>>>.*//g')"
  EXPECT="$(echo "${EXPECT}" | sed 's/.*>>>>.*//g')"

  if [ "$RES" == "$EXPECT" ]; then
    echo -e "\t$C_GREEN PASSED $C_END"
    TOTAL=$((TOTAL+1))
    PASSED=$((PASSED+1))
  else
    echo -e "\t$C_RED FAILED $C_END"
    TOTAL=$((TOTAL+1))
    FAILED=$((FAILED+1))
  fi
}

if [ ! -f $EMU_BIN ]; then
  echo "$EMU_BIN not found compile!"
  exit 1
fi

for file in tests/*; do
  echo -n "$file "
  . ./$file
done

echo "==== Summary ===="
echo "Total  : $TOTAL"
echo "Passed : $PASSED"
echo "Failed : $FAILED"
PERCENTAGE="$(echo "scale=2; ($PASSED/$TOTAL*100.0)" | bc)"
echo "Success: %$PERCENTAGE"


