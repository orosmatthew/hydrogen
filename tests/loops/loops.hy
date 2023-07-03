while (false) {
    print(1);
}

while (true) {
    print(2);
    break;
}

var n = 3;
while (n < 8) {
    print(n);
    n++;
    var w = n + 10;
    print(w);
}

for (var i = 0; i < 5; i++) {
    print(i);
    var w = i + 10;
    print(w);
}

for (var i = 10; i >= 0; --i) {
    print(i);
    var x = i--;
    print(x);
}

for (var x = 0; x < 20; x++) {
    print(x);
    if (x > 5) {
        break;
    }
}