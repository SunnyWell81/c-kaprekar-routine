#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { MAX_STEPS = 1000, MIN_DIGITS = 3, MAX_DIGITS = 10 };

int isValidNumber(long input) {
  long min_num = 1;
  for (int i = 0; i < MIN_DIGITS - 1; i++) {
    min_num *= 10;
  }
  long max_num = 1;
  for (int i = 0; i < MAX_DIGITS; i++) {
    max_num *= 10;
  }
  if (input < min_num || input >= max_num)
    return 0;
  long num = input;
  int firstnum = num % 10;
  num /= 10;
  while (num > 0) {
    if (firstnum != num % 10) {
      return 1;
    }
    num /= 10;
  }
  return 0;
}

long inputProcess() {
  printf("%d桁以上%d桁以下のゾロ目ではない整数を入力してください\n> ",
         MIN_DIGITS, MAX_DIGITS);
  long number;
  while (1) {
    char input[255];
    char *endptr;
    int ch;
    if (fgets(input, sizeof input, stdin) == NULL)
      return -1;
    if (strchr(input, '\n') == NULL) {
      while ((ch = getchar()) != '\n' && ch != EOF)
        ;
      printf("入力が長すぎる可能性があります\n> ");
      continue;
    }
    errno = 0;
    number = strtol(input, &endptr, 10);
    if (errno == ERANGE) {
      printf("数値が大きすぎるか小さすぎます\n> ");
      continue;
    }
    if (*endptr == '\n' && endptr != input && isValidNumber(number)) {
      break;
    }
    printf("値が無効です\n> ");
  }
  return number;
}

long sortDigits(long input, int mode, size_t length) {
  if ((mode != 1 && mode != -1) || length == 0 || length > MAX_DIGITS) {
    return -1;
  }
  long num = input;
  int numbers[MAX_DIGITS];
  for (size_t i = 0; i < length; i++) {
    numbers[i] = num % 10;
    num /= 10;
  }
  for (size_t i = 0; i < length; i++) {
    size_t selected = i;
    for (size_t j = i + 1; j < length; j++) {
      if ((mode == 1 && numbers[selected] <= numbers[j]) ||
          (mode == -1 && numbers[selected] >= numbers[j])) {
        selected = j;
      }
    }
    int temp = numbers[i];
    numbers[i] = numbers[selected];
    numbers[selected] = temp;
  }

  long result = numbers[0];
  for (size_t i = 1; i < length; i++) {
    result *= 10;
    result += numbers[i];
  }
  return result;
}

int main(void) {
  long input = inputProcess();
  if (input == -1)
    return 1;
  long num = input;
  size_t length = 0;
  while (num > 0) {
    num /= 10;
    length++;
  }
  size_t count = 0;
  size_t cycleStartIndex;
  int status = 0;
  long answer;
  long history[MAX_STEPS + 1];
  history[0] = input;
  while (count < MAX_STEPS) {
    count++;
    long descendingOrderNum = sortDigits(input, 1, length);
    long ascendingOrderNum = sortDigits(input, -1, length);
    answer = descendingOrderNum - ascendingOrderNum;
    if (answer == input) {
      status = 1;
    } else {
      for (size_t i = 0; i < count; i++) {
        if (history[i] == answer) {
          status = 2;
          cycleStartIndex = i;
          break;
        }
      }
    }
    history[count] = answer;
    if (status != 0) {
      break;
    }
    input = answer;
  }
  for (size_t i = 0; i <= count; i++) {
    if (i > 0) {
      if (i % 10 == 0) {
        printf("\n");
      } else {
        printf(" -> ");
      }
    }
    printf("%ld", history[i]);
  }
  printf("\n");
  if (status == 1) {
    printf("== 固定点に到達しました ==\n");
    printf("固定点: %ld\n", answer);
  } else if (status == 2) {
    printf("== 周期に到達しました ==\n");
    printf("終了時の周期値: %ld\n", answer);
    printf("周期:\n");
    for (size_t i = cycleStartIndex; i < count; i++) {
      if (i > cycleStartIndex) {
        printf(" -> ");
      }
      printf("%ld", history[i]);
    }
    printf("\n");
  } else {
    printf("** 計算回数が上限の%d回に達しました **\n", MAX_STEPS);
    return 1;
  }
  return 0;
}
