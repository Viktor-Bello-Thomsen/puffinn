#pragma once

#include <stdint.h>

template <typename dataType>
struct LshDatatype_DECL{
    
    // virtual bool operator< (LshDatatype<dataType> const& other) const = 0;
    // virtual bool operator<= (LshDatatype<dataType> const& other) const = 0;
    // virtual bool prefix_eq(LshDatatype<dataType> const& other, uint32_t prefix_length) const = 0;
    
    
    virtual dataType operator>>(uint64_t shift_amount) const = 0;
    virtual dataType operator&(uint64_t bits) const = 0;
    virtual void concatenate_hashes(const std::vector<unsigned int>& indices, const uint64_t* hashes,const uint_fast8_t& bits_per_function) = 0;
    virtual void concatenate_hash(const uint64_t& hash,const uint_fast8_t& bits_per_function) = 0;
    virtual void operator<<= (int bits) = 0; 

    dataType value;    
public:
    virtual dataType getValue() const = 0;
};

//We should implement a concatenation function such that the hashsource has an easier time.


//It might be better to not have any pure virtual functions and just have LshDataType 
//Maintain basic functionality i.e. no more of operator<= (LshDatatype<dataType> const &other) type functions in subclass

template <typename dataType>
struct HammingType: public LshDatatype_DECL<dataType> //How should I set the scope? private/protected/public
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

    HammingType(){
        this->value = 0;
    }

    HammingType(uint32_t value){
        this->value = value;
    }

    // HammingType(HammingType<dataType> &other){
    //     this->value = other.getValue();
    // }

    void concatenate_hashes(
        const std::vector<unsigned int>& indices, 
        const uint64_t* hashes,
        const uint_fast8_t& bits_per_function
    ) override { 
        for (auto idx : indices) {
            this->value <<= bits_per_function;
            this->value |= hashes[idx];
        }
    }

    void concatenate_hash(
        const uint64_t& hash,
        const uint_fast8_t& bits_per_function
    ) override {
        this->value <<= bits_per_function;
        this->value |= hash; 
    }

    dataType operator^(HammingType<uint64_t> other) const {
        return this->value ^ other.getValue();
    }

    dataType operator>>(uint64_t shift_amount) const override {
        return this->value >> shift_amount;
    }

    void operator>>=(unsigned int shift_amount) {
        this->value >>= shift_amount;
    }

    void operator|=(HammingType<dataType> mask){
        this->value |= mask.getValue();
    }

    dataType operator&(uint64_t bits) const override {
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


    bool prefix_eq(LshDatatype_DECL<dataType> const &other, uint32_t prefix_length) const {
        return 0;
    }

    HammingType<dataType> operator|(HammingType<dataType> const &other) const{
        return this->value | other.getValue();
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

