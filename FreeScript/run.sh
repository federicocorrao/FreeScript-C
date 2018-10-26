cd frontend
echo FLEX
flex -o lexer.c lexer.l
echo BISON
bison -o parser.c parser.y -v
cd ..
echo GCC
g++ -o bin/freescript main.c -lm
# -O1 # -foptimize-sibling-calls -foptimize-strlen -finline-functions-called-once -fsplit-wide-types -fcrossjumping -fif-conversion -fif-conversion2  -fcaller-saves -fipa-cp -ftree-tail-merge -ftree-loop-distribution -fpredictive-commoning
#-lm serve per linkare math.h
./bin/freescript


