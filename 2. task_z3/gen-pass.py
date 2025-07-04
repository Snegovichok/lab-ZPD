from z3 import * # pip install z3-solver

def generate_key():
    # Инициализируем решатель Z3
    s = Solver()
    
    # Создаем переменные для каждого символа ключа (20 символов)
    key = [BitVec(f'key_{i}', 8) for i in range(19)]
    
    # Условие 1: длина ключа 20 символов (уже задано размером массива)
    
    # Условие 2: каждый 5-й символ должен быть '-'
    for i in range(4, 19, 5):
        s.add(key[i] == ord('-'))
    
    # Условие 3: ограничения на остальные символы
    bad_chars = "\"'`;,.!:?()-{}[]<>_"
    for i in range(19):
        if i % 5 == 4:  # это дефисы, их уже проверили
            continue
            
        # Не управляющий символ и не DEL
        s.add(key[i] > 0x20)
        s.add(key[i] != 0x7f)
        
        # Не символ из запрещенного набора
        for c in bad_chars:
            s.add(key[i] != ord(c))
    
    # Собираем матрицу 4x4 из символов, пропуская дефисы
    matrix = []
    for i in range(20):
        if i % 5 != 4:
            matrix.append(key[i])
    
    # Создаем 4 группы по 4 байта (как в sub_400984)
    # Первый вариант группировки (var_38)
    var_38 = [Concat(*[matrix[j + i*4] for j in range(4)]) for i in range(4)]
    
    # Второй вариант группировки (var_28)
    var_28 = [Concat(*[matrix[i + j*4] for j in range(4)]) for i in range(4)]
    
    # Реализация функций проверки
    def sub_40093e(a1, a2, a3, a4):
        return (a2 ^ a4) & (a1 ^ a3)
    
    def sub_400960(a1, a2, a3, a4):
        return a4 + a1 + a2 + a3
    
    # Реальные значения из бинарного файла
    data_602070 = 0x3010602
    data_602074 = 0x70B7695
    data_602078 = 0x4004001
    data_60207C = 0x7D064C4E
    
    # Первая проверка (для var_38)
    s.add(sub_40093e(var_38[1], var_38[2], var_38[0], var_38[3]) == data_602070)
    s.add(sub_400960(var_38[1], var_38[3], var_38[0], var_38[2]) == data_602074)
    
    # Вторая проверка (для var_28)
    s.add(sub_40093e(var_28[1], var_28[2], var_28[0], var_28[3]) == data_602078)
    s.add(sub_400960(var_28[1], var_28[3], var_28[0], var_28[2]) == data_60207C)
    
    # Проверяем, есть ли решение
    if s.check() == sat:
        model = s.model()
        # Собираем ключ из модели
        result = []
        for i in range(19):
            if i % 5 == 4:
                result.append('-')
            else:
                result.append(chr(model[key[i]].as_long()))
        return ''.join(result)
    else:
        return "Решение не найдено"

# Генерируем и выводим ключ
# print("Действительный ключ:", generate_key())
for _ in range(10):
    print(generate_key())
