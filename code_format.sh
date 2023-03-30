for DIRECTORY in $@
do
    echo "Formatting code under $DIRECTORY/"
    find "$DIRECTORY" \(  -name '*.h' -or -name '*.hpp' -or -name '*.cpp' -or -name '*.mm' \) -not -path "*/3party/*" -print0 | xargs -0 clang-format -i
done
