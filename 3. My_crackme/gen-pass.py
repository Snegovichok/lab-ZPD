import hashlib
import random
import re
from string import ascii_lowercase, digits # для получения строк с символами: ascii_lowercase = 'abcdefghijklmnopqrstuvwxyz', digits = '0123456789'

def _is_prime(n):
    """Проверка на простое число"""
    if n < 2:
        return False
    if n == 2:
        return True
    if n % 2 == 0:
        return False
    for i in range(3, int(n**0.5) + 1, 2):
        if n % i == 0:
            return False
    return True

def generate_prime():
    """Генерация 5-значного простого числа"""
    while True:
        num = random.randint(10000, 99999)
        if _is_prime(num):
            return num

def generate_special_part():
    """Генерация специальной части по ASCII-кодам (90,69,69,75,82)"""
    ascii_codes = [90, 69, 69, 75, 82]  # Z, E, E, K, R
    return ''.join(chr(code) for code in ascii_codes)

def generate_checksum(part3, part4):
    """Генерация MD5 контрольной суммы"""
    md5_input = part3 + '`' + part4
    return hashlib.md5(md5_input.encode()).hexdigest()[-5:]

def generate_key():
    # Часть 1: "code" в любом регистре
    part1 = ''.join(random.choice([c.upper(), c.lower()]) for c in "code")

    # Часть 2: 5-значное простое число
    prime_part = str(generate_prime())

    # Часть 3: "ZEEKR" по ASCII-кодам
    special_part = generate_special_part()

    # Часть 4: 2 строчные буквы + 3 цифры
    part3 = (random.choice(ascii_lowercase) + 
             random.choice(ascii_lowercase) + 
             ''.join(random.choices(digits, k=3)))

    # Часть 5: "rgn" с кодом региона
    rf_regions = ['01', '02', '77', '78', '70', '66', '54', '23']
    region_code = random.choice(rf_regions)
    part4 = "rgn" + region_code

    # Часть 6: MD5 контрольная сумма
    checksum = generate_checksum(part3, part4)

    # Собираем ключ
    key = (
        f"{part1}["
        f"{prime_part}-"
        f"{special_part}-"
        f"{part3}-"
        f"{part4}-"
        f"{checksum}]"
    )

    # Добавляем разделители '-'
    key = list(key)
    separators = [10, 16, 22, 28]
    for pos in separators:
        if pos < len(key):
            key[pos] = '-'
    key = ''.join(key)
    
    return key

# Генерируем и выводим ключ
# print("Действительный ключ:", generate_key())
for _ in range(10):
    print(generate_key())
