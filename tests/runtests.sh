echo "Running unit tests:"

if test -f tests.log
then
    rm tests.log
fi

FAILURE=0

for i in `find . -name *_test`
do
    if test -f $i
    then
        $VALGRIND ./$i 2>&1 | tee -a tests.log
        if test ${PIPESTATUS[0]} -ne 0
        then
            FAILURE=1
        fi
    fi
done

if test $FAILURE -ne 0
then
    echo "TESTS FAILED."
fi
exit $FAILURE
