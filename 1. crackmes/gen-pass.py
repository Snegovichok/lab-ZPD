import random
import itertools

def generate_key():
    # Часть 1: "EXAM" в любом регистре
    part1 = ''.join(random.choice([c.upper(), c.lower()]) for c in "exam")
    
    # Часть 2: перестановка символов "3l1t" (уникальные)
    part2_chars = list("3l1t")
    random.shuffle(part2_chars)
    part2 = ''.join(part2_chars)
    
    # Часть 3: палиндром из 4 символов (например, "abba")
    part3 = ''.join([random.choice('abcdefghijklmnopqrstuvwxyz') for _ in range(2)])
    part3 += part3[::-1]  # зеркалируем первые два символа
    
    # Часть 4: HEX-число из списка [0x1337, 0x4242, 0xdead, 0xbeef, 0xcafe, 0xbabe]
    hex_values = [0x1337, 0x4242, 0xdead, 0xbeef, 0xcafe, 0xbabe]
    chosen_hex = random.choice(hex_values)
    part4 = f"{chosen_hex:04x}".upper()
    
    # Часть 5: согласные на чётных позициях, гласные на нечётных
    vowels = 'AEIOU'
    consonants = 'BCDFGHJKLMNPQRSTVWXYZ'
    part5 = []
    for i in range(4):
        if i % 2 == 0:
            part5.append(random.choice(consonants))
        else:
            part5.append(random.choice(vowels))
    part5 = ''.join(part5)
    
    # Часть 6: символы, которые после XOR с 7,8,9,10 дают "UGJA"
    target = "UGJA"
    part6 = []
    for i, c in enumerate(target):
        xor_val = 7 + i
        part6.append(chr(ord(c) ^ xor_val))
    part6 = ''.join(part6)
    
    # Собираем ключ
    key = (
        f"{part1}{{"
        f"{part2}_"
        f"{part3}_"
        f"{part4}_"
        f"{part5}_"
        f"{part6}}}"
    )
    
    # Заменяем разделители на '_' в нужных позициях
    key = list(key)
    separators = [9, 14, 19, 24]
    for pos in separators:
        if pos < len(key):
            key[pos] = '_'
    key = ''.join(key)
    
    return key

# Генерируем и выводим ключ
# print("Действительный ключ:", generate_key())
for _ in range(10):
    print(generate_key())
