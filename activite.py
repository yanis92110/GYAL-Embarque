from pathlib import Path
def read_pattern(path):
    lines=Path(path).read_text().splitlines()
    if len(lines)!=9 or any(len(row)!=6 or set(row)-set('.*') for row in lines[:8]):
        raise ValueError('Expected 8 rows of 6 pixels, then one label')
    target=int(lines[8])
    if target not in range(10):raise ValueError('Invalid digit')
    return [int(p=='*') for row in lines[:8] for p in row],target

print(read_pattern("zero.txt"))
