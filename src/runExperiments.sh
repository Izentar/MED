./src/prefixspan.out -f data/mushroom.dat -ms 2000 -mp 5 -o out/mushroom/SingleThr/2000_5 -r 10
./src/prefixspan.out -f data/mushroom.dat -ms 3000 -mp 10 -o out/mushroom/SingleThr/3000_10 -r 10
./src/prefixspan.out -f data/mushroom.dat -ms 4000 -mp 10 -o out/mushroom/SingleThr/4000_10 -r 10
./src/prefixspan.out -f data/mushroom.dat -ms 5000 -mp 10 -o out/mushroom/SingleThr/5000_10 -r 10

./src/prefixspan.out -f data/mushroom.dat -ms 2000 -mp 5 -o out/mushroom/MultiThr/2000_5 -r 10 -t -thr 4
./src/prefixspan.out -f data/mushroom.dat -ms 3000 -mp 10 -o out/mushroom/MultiThr/3000_10 -r 10 -t -thr 4
./src/prefixspan.out -f data/mushroom.dat -ms 4000 -mp 10 -o out/mushroom/MultiThr/4000_10 -r 10 -t -thr 4
./src/prefixspan.out -f data/mushroom.dat -ms 5000 -mp 10 -o out/mushroom/MultiThr/5000_10 -r 10 -t -thr 4




./src/prefixspan.out -f data/pumsb_star.dat -ms 25000 -mp 10 -o out/pumsb_star/SingleThr/25000_10 -r 4
./src/prefixspan.out -f data/pumsb_star.dat -ms 30000 -mp 10 -o out/pumsb_star/SingleThr/30000_10 -r 4
./src/prefixspan.out -f data/pumsb_star.dat -ms 30000 -mp 20 -o out/pumsb_star/SingleThr/30000_20 -r 4

./src/prefixspan.out -f data/pumsb_star.dat -ms 25000 -mp 10 -o out/pumsb_star/MultiThr/25000_10 -r 4 -t -thr 4
./src/prefixspan.out -f data/pumsb_star.dat -ms 30000 -mp 10 -o out/pumsb_star/MultiThr/30000_10 -r 4 -t -thr 4
./src/prefixspan.out -f data/pumsb_star.dat -ms 30000 -mp 20 -o out/pumsb_star/MultiThr/30000_20 -r 4 -t -thr 4

./src/prefixspan.out -f data/accidents.dat -ms 200000 -mp 5 -o out/accidents/MultiThr/200000_5 -r 1 -t -thr 4
./src/prefixspan.out -f data/accidents.dat -ms 200000 -mp 5 -o out/accidents/SingleThr/200000_5 -r 1