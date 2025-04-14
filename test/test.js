// 중첩된 반복문으로 복잡성을 테스트하는 함수
function testComplexity() {
    let i = 0;
    for (i = 0; i < 1000; i++) {
        console.log("i = " + i);
        for (let j = 0; j < 1000; j++) {
            console.log("j = " + j);
            for (let k = 0; k < 1000; k++) {
                console.log("k = " + k);
            }
        }
    }
}

// 메인 함수
function main() {
    console.log("Hello, World!");
    testComplexity();
    return 0;
}

// 프로그램 실행
main();
