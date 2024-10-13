#!/ bin / bash

fails=0
success=0

pattern='ke'

file='test.txt'
non_file='nonfile.txt'
second_pattern='r'

echo "" > log_grep.txt

flags=(
  "c"
  "h"
  "i"
  "l"
  "n"
  "s"
  "v"
  "o"
)

echo "____TESTING_SINGLE_GREP_FLAGS____"

for flag in "${flags[@]}"
do
    grep "-$flag" $pattern $file > grep.txt
    ./s21_grep "-$flag" $pattern $file > s21_grep.txt
    diff -q grep.txt s21_grep.txt
    if [ $? -eq 0 ]
    then
        (( success++ ))
        echo "OK: grep -$flag $pattern $file"
    else
        echo "./s21_grep -$flag $pattern $file" >> log_grep.txt
        (( fails++ ))
        echo "FAIL: grep -$flag $pattern $file"
    fi
done

echo "____TESTING_GREP_2_PATTERNS(FLAG_E)____"

for flag in "${flags[@]}"
do
    grep "-$flag" $pattern $file > grep.txt
    ./s21_grep "-$flag" $pattern $file > s21_grep.txt
    diff -q grep.txt s21_grep.txt
    if [ $? -eq 0 ]
    then
        (( success++ ))
        echo "OK: grep -$flag $pattern $second_pattern $file"
    else
        echo "./s21_grep -$flag $pattern $second_pattern $file" >> log_grep.txt
        (( fails++ ))
        echo "FAIL: grep -$flag $pattern $second_pattern $file"
    fi
done

for flag in "${flags[@]}"
do
    grep "-$flag" $pattern $file > grep.txt
    ./s21_grep "-$flag" $pattern $file > s21_grep.txt
    diff -q grep.txt s21_grep.txt
    if [ $? -eq 0 ]
    then
        (( success++ ))
        echo "OK: grep -$flag -e $pattern -e $second_pattern $file"
    else
        echo "./s21_grep -$flag -e $pattern -e $second_pattern $file" >> log_grep.txt
        (( fails++ ))
        echo "FAIL: grep -$flag -e $pattern -e $second_pattern $file"
    fi
done

echo "____TESTING_GREP_FLAGS_PAIRS____"

for flag1 in "${flags[@]}"
do
    for flag2 in "${flags[@]}"
    do
        if [ $flag1 != $flag2 ]
        then
            grep "-$flag1$flag2" $pattern $file > grep.txt
            ./s21_grep "-$flag1$flag2" $pattern $file > s21_grep.txt
            diff -q grep.txt s21_grep.txt
            if [ $? -eq 0 ]
            then
                (( success++ ))
                echo "OK: grep -$flag1$flag2 $pattern $file"
            else
                echo "./s21_grep -$flag1$flag2 $pattern $file" >> log_grep.txt
                (( fails++ ))
                echo "FAIL: grep -$flag1$flag2 $pattern $file"
            fi
        fi
    done
done


echo " "
echo "SUCCESS: $success"
echo "FAIL $fails"



rm -f grep.txt s21_grep.txt

exit 0