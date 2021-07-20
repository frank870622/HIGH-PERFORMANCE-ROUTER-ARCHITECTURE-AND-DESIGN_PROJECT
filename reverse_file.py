

f_rev = open("dmoz.txt", "w", encoding="utf-8")
# reverse
with open("temp.txt", "r", encoding="utf-8") as dmoz_file:

    for line in dmoz_file:
        split_list = line.split('.')
        
        #print(line)
        #print(split_list)
        print_string = ""
        #print("split_list: " + str(split_list))
        for i in range(len(split_list) - 1 , -1, -1):
            print_string += str(split_list[i])
            if i > 0:
                print_string += '/'
        print_string = print_string.replace("\n", "")
        #print("string : " + print_string)
        f_rev.writelines(print_string + '\n')
    

f_rev.close()