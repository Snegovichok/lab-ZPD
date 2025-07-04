#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdint.h>

/* ===== Простая реализация MD5 (public domain, на базе RFC1321) ===== */

typedef struct {
    uint32_t state[4];
    uint32_t count[2];
    unsigned char buffer[64];
} MD5_CTX;

static void MD5Transform(uint32_t state[4], const unsigned char block[64]);
static void Encode(unsigned char *output, const uint32_t *input, unsigned int len);
static void Decode(uint32_t *output, const unsigned char *input, unsigned int len);
static void MD5_memcpy(unsigned char* output, const unsigned char* input, unsigned int len);
static void MD5_memset(unsigned char* output, int value, unsigned int len);

void MD5Init(MD5_CTX *context) {
    context->count[0] = context->count[1] = 0;
    // Инициализация констант как в RFC1321
    context->state[0] = 0x67452301;
    context->state[1] = 0xefcdab89;
    context->state[2] = 0x98badcfe;
    context->state[3] = 0x10325476;
}

void MD5Update(MD5_CTX *context, const unsigned char *input, unsigned int inputLen) {
    unsigned int i, index, partLen;
    index = (unsigned int)((context->count[0] >> 3) & 0x3F);
    if ((context->count[0] += ((uint32_t)inputLen << 3)) < ((uint32_t)inputLen << 3))
        context->count[1]++;
    context->count[1] += ((uint32_t)inputLen >> 29);
    partLen = 64 - index;
    if (inputLen >= partLen) {
        MD5_memcpy(&context->buffer[index], input, partLen);
        MD5Transform(context->state, context->buffer);
        for (i = partLen; i + 63 < inputLen; i += 64)
            MD5Transform(context->state, &input[i]);
        index = 0;
    } else {
        i = 0;
    }
    MD5_memcpy(&context->buffer[index], &input[i], inputLen - i);
}

void MD5Final(unsigned char digest[16], MD5_CTX *context) {
    unsigned char bits[8];
    unsigned int index, padLen;
    // Счётчик бит
    Encode(bits, context->count, 8);
    index = (unsigned int)((context->count[0] >> 3) & 0x3f);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    // Заполнить до 56 байт: 0x80, затем нули
    static unsigned char PADDING[64] = { 0x80 };
    MD5Update(context, PADDING, padLen);
    // Добавить длину в битах
    MD5Update(context, bits, 8);
    // Выдать результат
    Encode(digest, context->state, 16);
    // Затирание
    MD5_memset((unsigned char*)context, 0, sizeof(*context));
}

/* Внутренние функции */

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

#define F(x,y,z) (((x) & (y)) | ((~x) & (z)))
#define G(x,y,z) (((x) & (z)) | ((y) & (~z)))
#define H(x,y,z) ((x) ^ (y) ^ (z))
#define I(x,y,z) ((y) ^ ((x) | (~z)))

#define ROTATE_LEFT(x,n) (((x) << (n)) | ((x) >> (32-(n))))

#define FF(a,b,c,d,x,s,ac) { \
 (a) += F ((b), (c), (d)) + (x) + (uint32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define GG(a,b,c,d,x,s,ac) { \
 (a) += G ((b), (c), (d)) + (x) + (uint32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define HH(a,b,c,d,x,s,ac) { \
 (a) += H ((b), (c), (d)) + (x) + (uint32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define II(a,b,c,d,x,s,ac) { \
 (a) += I ((b), (c), (d)) + (x) + (uint32_t)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

static void MD5Transform(uint32_t state[4], const unsigned char block[64]) {
    uint32_t a = state[0],
             b = state[1],
             c = state[2],
             d = state[3],
             x[16];
    Decode(x, block, 64);

    // Раунд 1
    FF (a, b, c, d, x[ 0], S11, 0xd76aa478);
    FF (d, a, b, c, x[ 1], S12, 0xe8c7b756);
    FF (c, d, a, b, x[ 2], S13, 0x242070db);
    FF (b, c, d, a, x[ 3], S14, 0xc1bdceee);
    FF (a, b, c, d, x[ 4], S11, 0xf57c0faf);
    FF (d, a, b, c, x[ 5], S12, 0x4787c62a);
    FF (c, d, a, b, x[ 6], S13, 0xa8304613);
    FF (b, c, d, a, x[ 7], S14, 0xfd469501);
    FF (a, b, c, d, x[ 8], S11, 0x698098d8);
    FF (d, a, b, c, x[ 9], S12, 0x8b44f7af);
    FF (c, d, a, b, x[10], S13, 0xffff5bb1);
    FF (b, c, d, a, x[11], S14, 0x895cd7be);
    FF (a, b, c, d, x[12], S11, 0x6b901122);
    FF (d, a, b, c, x[13], S12, 0xfd987193);
    FF (c, d, a, b, x[14], S13, 0xa679438e);
    FF (b, c, d, a, x[15], S14, 0x49b40821);
    // Раунд 2
    GG (a, b, c, d, x[ 1], S21, 0xf61e2562);
    GG (d, a, b, c, x[ 6], S22, 0xc040b340);
    GG (c, d, a, b, x[11], S23, 0x265e5a51);
    GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa);
    GG (a, b, c, d, x[ 5], S21, 0xd62f105d);
    GG (d, a, b, c, x[10], S22,  0x2441453);
    GG (c, d, a, b, x[15], S23, 0xd8a1e681);
    GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8);
    GG (a, b, c, d, x[ 9], S21, 0x21e1cde6);
    GG (d, a, b, c, x[14], S22, 0xc33707d6);
    GG (c, d, a, b, x[ 3], S23, 0xf4d50d87);
    GG (b, c, d, a, x[ 8], S24, 0x455a14ed);
    GG (a, b, c, d, x[13], S21, 0xa9e3e905);
    GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8);
    GG (c, d, a, b, x[ 7], S23, 0x676f02d9);
    GG (b, c, d, a, x[12], S24, 0x8d2a4c8a);
    // Раунд 3
    HH (a, b, c, d, x[ 5], S31, 0xfffa3942);
    HH (d, a, b, c, x[ 8], S32, 0x8771f681);
    HH (c, d, a, b, x[11], S33, 0x6d9d6122);
    HH (b, c, d, a, x[14], S34, 0xfde5380c);
    HH (a, b, c, d, x[ 1], S31, 0xa4beea44);
    HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9);
    HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60);
    HH (b, c, d, a, x[10], S34, 0xbebfbc70);
    HH (a, b, c, d, x[13], S31, 0x289b7ec6);
    HH (d, a, b, c, x[ 0], S32, 0xeaa127fa);
    HH (c, d, a, b, x[ 3], S33, 0xd4ef3085);
    HH (b, c, d, a, x[ 6], S34,  0x4881d05);
    HH (a, b, c, d, x[ 9], S31, 0xd9d4d039);
    HH (d, a, b, c, x[12], S32, 0xe6db99e5);
    HH (c, d, a, b, x[15], S33, 0x1fa27cf8);
    HH (b, c, d, a, x[ 2], S34, 0xc4ac5665);
    // Раунд 4
    II (a, b, c, d, x[ 0], S41, 0xf4292244);
    II (d, a, b, c, x[ 7], S42, 0x432aff97);
    II (c, d, a, b, x[14], S43, 0xab9423a7);
    II (b, c, d, a, x[ 5], S44, 0xfc93a039);
    II (a, b, c, d, x[12], S41, 0x655b59c3);
    II (d, a, b, c, x[ 3], S42, 0x8f0ccc92);
    II (c, d, a, b, x[10], S43, 0xffeff47d);
    II (b, c, d, a, x[ 1], S44, 0x85845dd1);
    II (a, b, c, d, x[ 8], S41, 0x6fa87e4f);
    II (d, a, b, c, x[15], S42, 0xfe2ce6e0);
    II (c, d, a, b, x[ 6], S43, 0xa3014314);
    II (b, c, d, a, x[13], S44, 0x4e0811a1);
    II (a, b, c, d, x[ 4], S41, 0xf7537e82);
    II (d, a, b, c, x[11], S42, 0xbd3af235);
    II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb);
    II (b, c, d, a, x[ 9], S44, 0xeb86d391);

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;

    // Затираем рабочие данные
    MD5_memset((unsigned char*)x, 0, sizeof(x));
}

static void Encode(unsigned char *output, const uint32_t *input, unsigned int len) {
    for (unsigned int i = 0, j = 0; j < len; i++, j += 4) {
        output[j  ] = (unsigned char)( input[i]        & 0xff);
        output[j+1] = (unsigned char)((input[i] >> 8 ) & 0xff);
        output[j+2] = (unsigned char)((input[i] >> 16) & 0xff);
        output[j+3] = (unsigned char)((input[i] >> 24) & 0xff);
    }
}

static void Decode(uint32_t *output, const unsigned char *input, unsigned int len) {
    for (unsigned int i = 0, j = 0; j < len; i++, j += 4) {
        output[i] = ((uint32_t)input[j]) 
                  | ((uint32_t)input[j+1] << 8)
                  | ((uint32_t)input[j+2] << 16)
                  | ((uint32_t)input[j+3] << 24);
    }
}

static void MD5_memcpy(unsigned char* output, const unsigned char* input, unsigned int len) {
    memcpy(output, input, len);
}

static void MD5_memset(unsigned char* output, int value, unsigned int len) {
    memset(output, value, len);
}

/* ===== Функции валидации ===== */

int is_prime(int n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if ((n & 1) == 0) return 0;
    int lim = (int)(sqrt(n) + 0.5);
    for (int i = 3; i <= lim; i += 2)
        if (n % i == 0) return 0;
    return 1;
}

int Check1(const char *key) {
    // символы с 5 по 9 (индексация с 0)
    for (int i = 5; i < 10; i++)
        if (!isdigit((unsigned char)key[i])) return 0;
    int num = atoi(key + 5);
    return is_prime(num);
}

int Check2(const char *key) {
    int expected[5] = {90, 69, 69, 75, 82};
    for (int i = 0; i < 5; i++)
        if ((unsigned char)key[11 + i] != expected[i])
            return 0;
    return 1;
}

int Check3(const char *key) {
    // два строчных латинских и три цифры
    if (!islower((unsigned char)key[17]) ||
        !islower((unsigned char)key[18]))
        return 0;
    for (int i = 0; i < 3; i++)
        if (!isdigit((unsigned char)key[19 + i]))
            return 0;
    return 1;
}

int Check4(const char *key) {
    // "rgn" + две цифры, входящие в набор
    if (strncmp(key + 23, "rgn", 3) != 0) return 0;
    const char *reg = key + 26;
    // допустимые коды
    const char *valid[] = {"01","02","77","78","70","66","54","23"};
    for (size_t i = 0; i < sizeof(valid)/sizeof(valid[0]); i++)
        if (strncmp(reg, valid[i], 2) == 0)
            return 1;
    return 0;
}

int Check5(const char *key) {
    // собираем строку: key[17:22] + '`' + key[23:28]
    char buf[6 + 1 + 5 + 1];
    memcpy(buf, key + 17, 5);
    buf[5] = '`';
    memcpy(buf + 6, key + 23, 5);
    buf[11] = '\0';
    // MD5
    unsigned char digest[16];
    MD5_CTX ctx;
    MD5Init(&ctx);
    MD5Update(&ctx, (unsigned char*)buf, strlen(buf));
    MD5Final(digest, &ctx);
    // превращаем в hex
    char hex[33];
    for (int i = 0; i < 16; i++)
        sprintf(hex + i*2, "%02x", digest[i]);
    // сравниваем последние 5 символов hex и key[29:34]
    return strncmp(hex + 27, key + 29, 5) == 0;
}

int Check(const char *key) {
    if (strlen(key) != 35) return 0;
    // "code"
    for (int i = 0; i < 4; i++)
        if (tolower((unsigned char)key[i]) != "code"[i]) return 0;
    if (key[4] != '[' || key[34] != ']') return 0;
    // дефисы
    for (int pos = 10; pos <= 28; pos += 6)
        if (key[pos] != '-') return 0;
    // все проверки
    return Check1(key) &&
           Check2(key) &&
           Check3(key) &&
           Check4(key) &&
           Check5(key);
}

int main(void) {
    char key[64];
    printf("Enter the key: ");
    if (!fgets(key, sizeof(key), stdin)) {
        fprintf(stderr, "Ошибка ввода\n");
        return 1;
    }
    // убрать возможный '\n'
    key[strcspn(key, "\r\n")] = '\0';

    if (Check(key))
        printf("Congratulations, you guessed the key!\n");
    else
        printf("Oh, it looks like you've thought badly.\n");

    return 0;
}
