import sys

sys.stdin = open("badmilk.in")
sys.stdout = open("badmilk.out", "w")

highest_int_people, milk_types, drink_events_count, sick_people_count = map(int, input().split())

# (person, milk, time)
events = []
for _ in range(drink_events_count):
    person_int, milk, time = map(int, input().split())
    events.append((person_int, milk, time))

# person -> sick_time
sick_info = {}
for _ in range(sick_people_count):
    person_int, time = map(int, input().split())
    sick_info[person_int] = time


def find_possible_milks():
    milk_sets = []

    for person in sick_info:
        sick_time = sick_info[person]
        drank = set()

        for p, milk, t in events:
            if p == person and t < sick_time:
                drank.add(milk)

        milk_sets.append(drank)

    # intersect all sets
    common = milk_sets[0]
    for s in milk_sets[1:]:
        common &= s

    return common


def count_max_people(possible_milks):
    max_people = 0

    for milk_id in possible_milks:
        people_set = set()

        for p, milk, t in events:
            if milk == milk_id:
                people_set.add(p)

        max_people = max(max_people, len(people_set))

    return max_people


def final_filter():
    possible_milks = find_possible_milks()
    answer = count_max_people(possible_milks)
    print(answer)


final_filter()