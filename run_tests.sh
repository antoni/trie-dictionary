#!/bin/bash


MEMLIMIT=263000
TIMELIMIT=3600


function ctrl_c {
    echo "** Aborting..."
    rm result.*
    exit 42
}

function table_delim {
    echo "+--------------------+-------------+-------+-------+"
}


function cmp_files {
    if ( diff -bB $1 $2 2>/dev/null > /dev/null )
    then
        echo -ne "\e[32m   OK  \e[0m|"
    else
        echo -ne "\e[91m   WA  \e[0m|"
    fi
}

# $1 - test name, $2 - test cmd, $3 - test only
function run_test {
    testname=$1
    testcmd=$2
    testonly=$3

    (ulimit -v $MEMLIMIT -t $TIMELIMIT; /usr/bin/time --quiet -f "%Es" -o result.tm $testcmd < tests/$testname.in > result.out 2> result.err)
    errorcode=$?
    elapsedtime=`cat result.tm | tail -n 1`

    printf "| %-18s |" $1

    if [ "$errorcode" -eq "0" ] && [ "$testonly" -eq "0" ]
    then
        # poszlo OK
        echo -ne "\e[32m"
        printf " %-11s " $elapsedtime
        echo -ne "\e[0m|"
        
        cmp_files result.out tests/$testname.out
        cmp_files result.err tests/$testname.err
        echo ""

    elif [ "$errorcode" -eq "0" ]
    then
        echo -ne "\e[32m"
        printf " %-11s " $elapsedtime
        echo -ne "\e[0m|"
        echo -e "      \e[32mOK\e[0m       |"
    else
        echo -ne "\e[34m"
        printf " %-11s " $elapsedtime
        echo -ne "\e[0m|"
        ecode=" \e[34mRE (code "`printf "%-3s" $errorcode`")\e[0m "

        if [ "$errorcode" -eq "130" ]
        then
            ecode=" \e[34m    SIGINT   \e[0m "
        elif [ "$errorcode" -eq "134" ]
        then
            ecode=" \e[34m   SIGABRT   \e[0m "
        elif [ "$errorcode" -eq "136" ]
        then
            ecode=" \e[34m    SIGFPE   \e[0m "
        elif [ "$errorcode" -eq "137" ]
        then
            ecode=" \e[93m     TLE     \e[0m "
        elif [ "$errorcode" -eq "139" ]
        then
            ecode=" \e[34m   SIGSEGV   \e[0m "
        fi

        echo -e "$ecode|"
    fi
}


# $1 - test cmd, $2 - test only
function run_tests {
    arg=("$@")

    table_delim
    if [ "$2" -eq "0" ]
    then
        echo "|      TEST NAME     |     TIME    |.out ok|.err ok|"
    else
        echo "|      TEST NAME     |     TIME    |   is run ok?  |"
    fi

    table_delim
    for t in `cat tests/TEST_LIST`
    do
        run_test $t "$@";
    done

    table_delim
}


trap ctrl_c INT
echo "ZWYKLE TESTY"
echo ""
run_tests "./dictionary -v" 0

VALGRIND_ERRCODE=196
VALGRIND_OPT="--leak-check=full --errors-for-leak-kinds=all --error-exitcode=$VALGRIND_ERRCODE"
echo "


VALGRIND

Program jest testowany na tych samych zestawach, co poprzednio, jednak
teraz pod Valgrindem. Valgrind pozwala m.in. na sledzenie tzw. wyciekow pamieci
(zgubiona/niezwolniona/itd. pamiec).

Uwagi:
1) Do dzialania potrzebny jest pakiet valgrind (zwykle jest w domyslnych
     repozytoriach Linuksa). Kod bledu 127 zazwyczaj oznacza, ze takiego pakietu
     nie ma.

2) Programy pod valgrindem moga dzialac nawet do 10-30x wolniej. Nalezy wiec uzbroic
     sie w cierpliwosc.

3) Skrypt ustawia opcje valgrinda tak, by po stwierdzeniu wycieku/niezwolnienia pamieci
     zwracal kod bledu $VALGRIND_ERRCODE.

"

run_tests "valgrind $VALGRIND_OPT ./dictionary" 1

rm result.out result.err result.tm
