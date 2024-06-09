// #1
// #include <iostream>

// int main() {
//     long a, b;
//     std::cin >> a >> b;
//     std::cout << a + b;
//     return 0;
// }

// # 2
// #include <iostream>

// int main() {
//     long long shift;
//     int length;
//     std::cin >> length;
//     std::cin >> shift;

//     int a[1000];
//     for (int i = 0; i < length; i++) {
//         std::cin >> a[i];
//     }
//     if (shift == 0) {
//         for (int g = 0; g < length; g++) {
//             std::cout << a[g] << " ";
//         }
//     } else if (shift > 0) { //Сдвиг вправо
//         shift = abs(shift) % length; //Уменьшение времени
//         for (int j = length - shift; j < length; j++) {
//             std::cout << a[j] << " ";
//         }
//         for (int h = 0; h < length - shift; h++) {
//             std::cout << a[h] << " ";
//         }
//     } else { //Сдвиг влево
//         shift = abs(shift) % length;
//         for (int j = shift; j < length; j++) {
//             std::cout << a[j] << " ";
//         }
//         for (int h = 0; h < shift; h++) {
//             std::cout << a[h] << " ";
//         }
//     }
//     return 0;
// }

// #3 Алгоритм Евклида
// #include <iostream>

// int main() {
//     long long sweets, nuts;
//     std::cin >> sweets >> nuts;

//     while (sweets != 0 && nuts != 0) {
//         if (sweets > nuts) {
//             sweets = sweets % nuts;
//         } else {
//             nuts = nuts % sweets;
//         }
//     }
    
//     std::cout << sweets + nuts;
//     return 0;
// }

// #4
// #include <iostream>

// int main () {
//     long n, m, q, season, episode, i;
//     std::cin >> n >> m >> q;

//     long epi_cnt = 0;
//     long j = 1;
//     long i = 1;
//     int a[n][m];

//     for (int h = 1; h < q; h++) {
//         std::cin >> episode >> season;

//         if (season < n && episode <= m) {
//             while (i <= season) {
//                 if (i != season || (i == season && )) {
//                     while (j < m) {
//                         a[i][j] = j;
//                         j++;
//                         epi_cnt++;
//                     }
//                     j = 1;

//                 } else {
//                     while (j < episode) {
//                         a[i][j] = j;
//                         j++;
//                         epi_cnt++;
//                     }
//                 }
//                 i++;
//             }

//         } else {
//             for (int i = save_i; i <= n; i++) {
//                 for (int j = save_j; j < m; j++) {
//                     a[i][j] = j;
//                     epi_cnt++;
//                 }
//                 save_j = save_j + j;
            
//             }
//         }
//     }

//     return 0;
// }

// #include <iostream>

// int main () {

//     std::ios::sync_with_stdio(0);
//     std::cin.tie(0);
//     std::cout.tie(0);
//     using namespace std;

//     long long  n, m, q, episode, season;
//     cin >> n >> m >> q;
//     long long cnt = m*n;
//     long long f = 0;
//     long long g = 0;

//     long long a[100][100];

//     for (int b = 0; b < n; b++) {
//         for (int c = 0; c < m; c++) {
//             a[b][c] = 0;
//         }
//     }
//     for (long long h = 0; h < q; h++) {
//         cin >> episode >> season;
//         if (a[season-1][episode-1] == 0) {
//             a[season-1][episode-1] = 1;
//             cnt--;
//         }
//     }
//     cout << cnt << "\n";
//     f = 0;
//     while (f < n) {
//         g = 0;
//         while (g < m) {
//             if (a[f][g] == 0) {
//                 cout << g+1 << " " << f+1 << "\n";
//             }
//             g++;
//         }
//         f++;
//     }
//     return 0;
// }


// #5
// #include <iostream>

// int main() {
//     long n;

//     std::cin >> n;
//     char word[5];
//     for (int i = 0; i < n; i++) {
//         std::cin >> word[i];
//     }

//     int flag = 0;
//     long left = 0;
//     int cnt = 0;
//     long right = n - 1;

//     while (left < n / 2) {
//         if (word[left] != word[right]) {
//             cnt++;
//             // Восстанавливаем симметрию палиндрома:
//             if (word[left+1] == word[right]) {
//                 left++;
//             } else if (word[left] == word[right-1]) {
//                 right--;
//             }
//         }
//         left++;
//         right--;
//     }
//     if (cnt < 2) {
//         flag = 1;
//     }

//     if (flag > 0) {
//         std::cout << "YES";
//     } else {
//         std::cout << "NO";
//     }
// }

// #6
// #include <iostream>

// int main() {
//     std::ios::sync_with_stdio(0);
//     std::cin.tie(0);
//     std::cout.tie(0);

//     long sum = 0;
//     long n;
//     long result = -1;
//     std::cin >> n;
//     long a[100][2];

//     std::cin >> a[0][0];
//     a[0][1] = 0;
//     sum = sum + a[0][0];

//     for (long i = 1; i < n; i++) {
//         std::cin >> a[i][0];
//         a[i][1] = a[i-1][1] + a[i-1][0]; //Сумма до элемента (предыдущая сумма + предыдущий элемент)
//         sum = sum + a[i][0]; //Сумма всех чисел
//     }
//     for (long j = 0; j < n; j++) {
//         // std::cout << a[j][0] << " => " << a[j][1] << " => " << sum - (a[j][0] + a[j][1]) << "\n";
//         if (a[j][1] == (sum - (a[j][0] + a[j][1]))) { //Если сумма до = (общей сумме - элемент - сумма до) => выводи индекс
//             result = j;
//             break;
//         }
//     } 
//     std::cout << result;
//     return 0;
// }

// #7
// #include <iostream>

// int main() {
//     int result, num, t, people;
//     long long b_index = 0;
//     std::string simbol;
//     std::cin >> t;
//     int a[16];
//     int b[100];

//     for (int i = 0; i < t; i++) { //Кол-во направлений
//         std::cin >> people;

//         for (int k = 0; k < 16; k++) { //Обновление массива элементов 15-30
//             a[k] = 15 + k;
//         }

//         for (int j = 0; j < people; j++) { //Кол-во заявок экспертов
//             std::cin >> simbol >> num;

//             if (simbol == ">=") { //Если введён символ >=
//                 for (int n = 0; n < 16; n++) {
//                     if (a[n] < num) {
//                         a[n] = 0;
//                     } else {
//                         break;
//                     }
//                 }
//             } else { //Если введён символ <=
//                 for (int m = 0; m < 16; m++) {
//                     if (a[15 - m] > num) {
//                         a[15 - m] = 0;
//                     }
//                 }
//             }

//         result = -1;
//         for (int t = 0; t < 16; t++) { //Нахождение минимального подходящего числа
//             if (a[t] != 0) {
//                 result = a[t];
//                 break;
//             }
//         }
//         b[b_index] = result;
//         b_index++;
//         }
//     }
//     for (long long r = 0; r < b_index; r++) { //Вывод финального массива
//         std::cout << b[r] << "\n";
//     }
//     return 0;
// }

// #include <iostream>

// int main() {
//     long n;
//     std::cin >> n;
//     long long a[n];
//     for (long i = 0; i < n; i++) {
//         std::cin >> a[i];
//     }
    
// }