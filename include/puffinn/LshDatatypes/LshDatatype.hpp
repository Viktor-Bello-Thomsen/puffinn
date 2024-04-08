#pragma once

#include <stdint.h>


template <typename dataType>
struct LshDatatype{
    
    // virtual bool operator<= (LshDatatype<dataType> const& other) const = 0;
    // virtual bool prefix_eq(LshDatatype<dataType> const& other, uint32_t prefix_length) const = 0;
    // virtual bool operator< (LshDatatype<dataType> const& other) const = 0;


    dataType value;    
public:
    virtual dataType getValue() const = 0;
};

//It might be better to not have any pure virtual functions and just have LshDataType 
//Maintain basic functionality i.e. no more of operator<= (LshDatatype<dataType> const &other) type functions in subclass

template <typename dataType>
struct EuclideanType: public LshDatatype<dataType> //How should I set the scope? private/protected/public
{
public:
    // bool operator<= (LshDatatype<dataType> const &other) const override {
    //     return this->value <= other.value;
    // }

    // bool prefix_eq(LshDatatype<dataType> const &other, uint32_t prefix_length) const override{
    //     return 0;
    // }

    // dataType operator&(LshDatatype<dataType> other) const {
    //     return this->getValue() & other.getValue();
    // }

    // bool operator< (LshDatatype<dataType> const& other) const override{
    //     return this->value < other.getValue();
    // }

    EuclideanType(){}

    EuclideanType(uint32_t value){
        this->value = value;
    }

    dataType operator>>(unsigned int shift_amount) const {
        return this->value >> shift_amount;
    }

    dataType operator&(unsigned int bits) const {
        return this->value & bits;
    }

    dataType operator&(EuclideanType<dataType> other) const {
        return this->value & other.getValue();
    }

    dataType operator%(EuclideanType<dataType> other) const{
        return this->value % other.getValue();
    }

    bool operator== (EuclideanType<dataType> other) const{
        return this->value == other.getValue();
    }

    void operator<<= (int bits) {
        this->value = this->value << bits;
    }

    bool operator< (EuclideanType<dataType> const& other) const{
        return this->value < other.getValue();
    }

    dataType getValue() const override{
        return this->value;
    }

};

