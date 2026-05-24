def simple_example_1():
    squares = {x: x ** 2 for x in range(1, 6)}
    print(squares)


simple_example_1()


def simple_example_2():
    words = ["apple", "banana", "cherry"]
    word_length = {word:len(word)  for word in words}
    print(word_length)


simple_example_2()


def with_filter_condition():
    squares = {x: x ** 2 for x in range(1, 11) if x % 2 == 0}
    print(squares)


with_filter_condition()


def nested_example():
    matrix = {(i, j):i * j   for i in range(1, 4)
                             for j in range(1, 4)}

    print(matrix)


nested_example()


def calculate(x):
    return (x ** 2 + 3) % 6


def using_extra_function():
    numbers = [1, 2, 3, 4, 5]
    my_dict = {x:calculate(x) for x in numbers}
    print(my_dict)


using_extra_function()