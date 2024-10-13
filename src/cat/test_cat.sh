#!/bin/bash

SUCCESS=0
FAIL=0
DIFF_RES=""

declare -a tests=(
"VAR test_1_cat.txt"
"VAR test_2_cat.txt"
# "VAR test_case_cat.txt"
)

declare -a extra=(
"-s test_1_cat.txt"
"-s test_2_cat.txt"
"-s test_4_cat.txt"

"-e test_1_cat.txt"
"-e test_2_cat.txt"
"-e test_3_cat.txt"
"-e test_4_cat.txt"
"-e test_5_cat.txt"

"-t test_1_cat.txt"
"-t test_2_cat.txt"
"-t test_3_cat.txt"
"-t test_4_cat.txt"
"-t test_5_cat.txt"

"-n test_1_cat.txt"
"-n test_2_cat.txt"
"-n test_3_cat.txt"
"-n test_4_cat.txt"
"-n test_5_cat.txt"

"-b test_1_cat.txt"
"-b test_2_cat.txt"
"-b test_3_cat.txt"
"-b test_4_cat.txt"
"-b test_5_cat.txt"

"-v test_3_cat.txt"
"-v test_5_cat.txt"

"-n -b test_1_cat.txt"
"-s -n -e test_4_cat.txt"

)

testing()
{
    t=$(echo $@ | sed "s/VAR/$var/")
    ./s21_cat $t > test_s21_cat.log
    cat $t > test_sys_cat.log
    DIFF_RES="$(diff -s test_s21_cat.log test_sys_cat.log)"
    if [ "$DIFF_RES" == "Files test_s21_cat.log and test_sys_cat.log are identical" ]
    then
      (( SUCCESS++ ))
        echo "OK: cat $t"
    else
      (( FAIL++ ))
        echo "FAIL: cat $t"
    fi
    rm test_s21_cat.log test_sys_cat.log
}

# специфические тесты
for i in "${extra[@]}"
do
    var="-"
    testing $i
done

# 1 параметр
for var1 in b e n s t v
do
    for i in "${tests[@]}"
    do
        var="-$var1"
        testing $i
    done
done

# 2 параметра
for var1 in b e n s t v
do
    for var2 in n s t v
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1 -$var2"
                testing $i
            done
        fi
    done
done

echo " "
echo "SUCCESS: $SUCCESS"
echo "FAIL: $FAIL"