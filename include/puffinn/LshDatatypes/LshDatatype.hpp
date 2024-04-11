#pragma once

#include <stdint.h>


template <typename dataType>
struct LshDatatype{
    
    // virtual bool operator< (LshDatatype<dataType> const& other) const = 0;
    // virtual bool operator<= (LshDatatype<dataType> const& other) const = 0;
    // virtual bool prefix_eq(LshDatatype<dataType> const& other, uint32_t prefix_length) const = 0;
    
    virtual dataType operator>>(unsigned int shift_amount) const = 0;
    virtual dataType operator&(unsigned int bits) const = 0;
    virtual void operator<<= (int bits) = 0; 

    dataType value;    
public:
    virtual dataType getValue() const = 0;
};

//It might be better to not have any pure virtual functions and just have LshDataType 
//Maintain basic functionality i.e. no more of operator<= (LshDatatype<dataType> const &other) type functions in subclass

template <typename dataType>
struct HammingType: public LshDatatype<dataType> //How should I set the scope? private/protected/public
{
public:
    // bool operator<= (LshDatatype<dataType> const &other) const override {
    //     return this->value <= other.value;
    // }

   
    // dataType operator&(LshDatatype<dataType> other) const {
    //     return this->getValue() & other.getValue();
    // }

    // bool operator< (LshDatatype<dataType> const& other) const override{
    //     return this->value < other.getValue();
    // }

    // bool operator< (LshDatatype<dataType> const& other) const override{
    //     auto* otherEuc = dynamic_cast<const HammingType<dataType>*>(&other);
    //     if(!otherEuc){exit(1);}
    //     return this->getValue() < otherEuc->getValue();
    // }

    HammingType(){}

    HammingType(uint32_t value){
        this->value = value;
    }

    dataType operator>>(unsigned int shift_amount) const override {
        return this->value >> shift_amount;
    }

    dataType operator&(unsigned int bits) const override {
        return this->value & bits;
    }

    void operator<<= (int bits) override {
        this->value = this->value << bits;
    }

    dataType getValue() const override{
        return this->value;
    }

    bool operator< (HammingType<dataType> const& other) const {
        return this->value < other.getValue();
    }

    bool operator< (unsigned int val){
        return this->getValue() < val;
    }


    bool prefix_eq(LshDatatype<dataType> const &other, uint32_t prefix_length) const override{
        return 0;
    }


    dataType operator& (HammingType<dataType> const& other) const {
        return this->value & other.getValue();
    }

    dataType operator% (HammingType<dataType> const& other) const {
        return this->value % other.getValue();
    }

    bool operator== (HammingType<dataType> const& other) const{
        return this->value == other.getValue();
    }

};

