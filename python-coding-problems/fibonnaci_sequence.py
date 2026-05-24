def fib_seq(no1, no2, repeat_times):
    sequence = [no1, no2]
    for i in range(repeat_times):
        sequence.append(sequence[-1] + sequence[-2])

    return sequence


def fib_seq_limit(no1, no2):
    global sequence # always accessible in code
    sequence = [no1, no2]
    sequence.append(sequence[-1] + sequence[-2])
