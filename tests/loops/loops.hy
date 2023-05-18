while (false) {
    print(1);
}

while (true) {
    print(2);
    break;
}

let n = 3;
while (n < 8) {
    print(n);
    n++;
    let w = n + 10;
    print(w);
}

for (let i = 0; i < 5; i++) {
    print(i);
    let w = i + 10;
    print(w);
}

for (let i = 10; i >= 0; --i) {
    print(i);
    let x = i--;
    print(x);
}

for (let x = 0; x < 20; x++) {
    print(x);
    if (x > 5) {
        break;
    }
}