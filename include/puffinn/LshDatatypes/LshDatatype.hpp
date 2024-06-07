#pragma once

#include <stdint.h>

struct LshDatatype_DECL{
    
    virtual void concatenate_hashes(const std::vector<unsigned int>& indices, const uint64_t* hashes,const uint_fast8_t& bits_per_function) = 0;
    virtual void concatenate_hash(const uint64_t& hash,const uint_fast8_t& bits_per_function) = 0;
    virtual void operator<<= (int bits) = 0;    
    virtual void pop_hash() = 0; 

};


template <typename dataType>
struct HammingType: public LshDatatype_DECL //How should I set the scope? private/protected/public
{
public:

    dataType value;
    
    HammingType(){
        this->value = 0;
    }

    HammingType(dataType value){
        this->value = value;
    }

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

    HammingType<dataType> intersperse_zero() const {
        dataType mask = 1;
        dataType shift = 0;
        dataType res = 0;
        for (unsigned i=0; i < sizeof(dataType)*8/2; i++) {
            res |= (this->value & mask) << shift;
            mask <<= 1;
            shift++;
        }
        return res;
    }
    //This is a hacky solution for now, this works because the function is only ever called for a prefix_mask which starts as all 1's
    void pop_hash() {
        this->value = this->value << 1;  
    } 
    

    bool prefix_eq(HammingType<dataType> other, HammingType<dataType> mask) const{
        return this->value == (other & mask); 
    }

    HammingType<dataType> interleave(const HammingType<dataType>& other) const {
        return this->value | other.getValue();
    }

    dataType operator^(HammingType<dataType> other) const {
        return this->value ^ other.getValue();
    }

    dataType operator>>(unsigned int shift_amount) const {
        return this->value >> shift_amount;
    }

    void operator>>=(unsigned int shift_amount) {
        this->value >>= shift_amount;
    }

    void operator|=(HammingType<dataType> mask){
        this->value |= mask.getValue();
    }

    void operator<<= (int bits) override {
        this->value = this->value << bits;
    }

    dataType getValue() const{
        return this->value;
    }

    bool operator< (HammingType<dataType> const& other) const {
        return this->value < other.getValue();
    }


    // bool prefix_eq(LshDatatype_DECL<dataType> const &other, uint32_t prefix_length) const {
    //     return 0;
    // }

    dataType operator& (HammingType<dataType> const& other) const {
        return this->value & other.getValue();
    }

};

