/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include <iostream>
#include "monster_generated.h"  
#include <flatbuffers/flatbuffers.h>  
  
int main() {
    // Encode test  
    // Create a FlatBufferBuilder instance to construct serialized data  
    flatbuffers::FlatBufferBuilder builder;  
    // Serialize the string "Dianjixz"  
    auto name = builder.CreateString("Dianjixz");  
    // Serialize a Person object  
    auto person = MyGame::Example::CreatePerson(builder, name, 10);  
    // Finish building and get the pointer to the serialized data  
    builder.Finish(person);  
    // Get the pointer and size of the serialized data  
    uint8_t *buf = builder.GetBufferPointer();  
    int size = builder.GetSize();  
    
    // Decode test
    // Verify the FlatBuffer data  
    flatbuffers::Verifier verifier(buf, size);  
    if (!MyGame::Example::VerifyPersonBuffer(verifier)) {  
        fprintf(stderr, "flatbuffers: data is not a valid MyGame.Example.Person\n");  
        return -1;  
    }  
    // Get the deserialized Person object  
    auto person_ptr = MyGame::Example::GetPerson(buf);    
    // Access the data from the Person object  
    std::cout << "Name: " << person_ptr->name()->c_str() << ", Age: " << person_ptr->age() << std::endl;  
  
    return 0;  
}
