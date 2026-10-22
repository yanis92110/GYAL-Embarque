def read_file(file_path):
    with open(file_path,"r", encoding="utf-8") as file:
        while True:
            caractere = file.read(1)
            if not caractere:
                break
            print(caractere,end="")

read_file("zero.txt")
