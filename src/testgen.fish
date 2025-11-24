#!/usr/bin/fish

make bin/generator
mkdir -p tests
for i in (seq 1 5)
    ./bin/generator 100 >tests/test_100_$i.in
end
for i in (seq 1 10)
    ./bin/generator 10000 >tests/test_10000_$i.in
end
for i in (seq 1 30)
    ./bin/generator 200000 >tests/test_200000_$i.in
end

echo OK
