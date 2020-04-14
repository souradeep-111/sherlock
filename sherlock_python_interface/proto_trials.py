import addressbook_pb2

# Write a message in protobuf format
address_book = addressbook_pb2.AddressBook()
person = address_book.people.add()

# person = addressbook_pb2.Person()

person.id = 1234
person.name = "John Doe"
person.email = "jdoe@example.com"
phone = person.phones.add()
phone.number = "555-4321"
phone.type = addressbook_pb2.Person.HOME


f = open("Address_Book.sherlock_message", "wb")
f.write(address_book.SerializeToString())
f.close()


# Read that and display the results

f = open("Address_Book.sherlock_message", "rb")
address_book = addressbook_pb2.AddressBook()
address_book.ParseFromString(f.read())
f.close()

print("Address Book read")

for each_person in address_book.people :
    print("Name of the person : ", each_person.name)
    print("ID of the person : ", each_person.id)
    print("Email : ", each_person.email)
    for each_phone in each_person.phones:
        print("Phone : ", each_phone.number)
        if(each_phone.type == addressbook_pb2.Person.PhoneType.HOME):
            print("Type : HOME")
        else:
            print("NOT HOME")


# print(address_book)
