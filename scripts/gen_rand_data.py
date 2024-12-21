import sys
import random

INT_RANGE = 100

def generate_blocks(num_of_inserts, num_of_queries):
    blocks = []
    
    # Generate insert blocks
    for _ in range(num_of_inserts):
        k_value = random.randint(-INT_RANGE, INT_RANGE)  # Random integer for insert
        blocks.append(f"k {k_value}")
    
    # Generate query blocks
    for _ in range(num_of_queries):
        a = random.randint(-INT_RANGE, INT_RANGE)
        b = random.randint(-INT_RANGE, INT_RANGE)
        while b == a:
            b = random.randint(-INT_RANGE, INT_RANGE)
        blocks.append(f"q {min(a,b)} {max(a,b)}")
    
    random.shuffle(blocks)  # Shuffle the blocks
    return blocks

def write_to_file(filename, blocks):
    with open(filename, 'w') as f:
        for block in blocks:
            f.write(block + ' ')

def main():
    if len(sys.argv) != 4:
        print("Usage: python script.py <num_of_inserts> <num_of_queries> <out_filename>")
        sys.exit(1)

    num_of_inserts = int(sys.argv[1])
    num_of_queries = int(sys.argv[2])
    out_filename = sys.argv[3]

    blocks = generate_blocks(num_of_inserts, num_of_queries)
    write_to_file(out_filename, blocks)
    print(f"Generated {num_of_inserts} inserts and {num_of_queries} queries in '{out_filename}'.")

if __name__ == "__main__":
    main()
