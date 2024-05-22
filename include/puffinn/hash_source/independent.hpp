#pragma once

#include "puffinn/dataset.hpp"
#include "puffinn/hash_source/hash_source.hpp"

namespace puffinn {
    // A source of completely independent hash functions.
    template <typename T, typename hashType>
    class IndependentHashSource : public HashSource<T, hashType> {
        T hash_family;
        std::vector<typename T::Function> hash_functions;
        unsigned int num_hashers;
        unsigned int functions_per_hasher;
        uint_fast8_t bits_per_function;
        unsigned int next_function = 0;
        unsigned int bits_to_cut;
    
    public:
        IndependentHashSource(
            DatasetDescription<typename T::Sim::Format> desc,
            typename T::Args args,
            // Number of hashers to create.
            unsigned int num_hashers,
            // Number of bits per hasher.
            unsigned int num_bits
        ) 
          : hash_family(desc, args), num_hashers(num_hashers)
        {
            bits_per_function = hash_family.bits_per_function();
            functions_per_hasher =
                (num_bits+bits_per_function-1)/bits_per_function;
            auto num_functions = functions_per_hasher*num_hashers;
            bits_to_cut = bits_per_function*functions_per_hasher-num_bits;
            hash_functions.reserve(num_functions);
            for (unsigned int i=0; i < num_functions; i++) {
                hash_functions.push_back(hash_family.sample());
            }
        }

        IndependentHashSource(std::istream& in)
          : hash_family(in)
        {
            size_t funcs_len;
            in.read(reinterpret_cast<char*>(&funcs_len), sizeof(size_t));
            hash_functions.reserve(funcs_len);
            for (size_t i=0; i < funcs_len; i++) {
                hash_functions.push_back(typename T::Function(in));
            }
            in.read(reinterpret_cast<char*>(&num_hashers), sizeof(unsigned int));
            in.read(reinterpret_cast<char*>(&functions_per_hasher), sizeof(unsigned int));
            in.read(reinterpret_cast<char*>(&bits_per_function), sizeof(uint_fast8_t));
            in.read(reinterpret_cast<char*>(&next_function), sizeof(unsigned int));
            in.read(reinterpret_cast<char*>(&bits_to_cut), sizeof(unsigned int));
        }

        void serialize(std::ostream& out) const {
            hash_family.serialize(out);
            size_t funcs_len = hash_functions.size();
            out.write(reinterpret_cast<char*>(&funcs_len), sizeof(size_t));
            for (auto& h : hash_functions) {
                h.serialize(out);
            }
            out.write(reinterpret_cast<const char*>(&num_hashers), sizeof(unsigned int));
            out.write(reinterpret_cast<const char*>(&functions_per_hasher), sizeof(unsigned int));
            out.write(reinterpret_cast<const char*>(&bits_per_function), sizeof(uint_fast8_t));
            out.write(reinterpret_cast<const char*>(&next_function), sizeof(unsigned int));
            out.write(reinterpret_cast<const char*>(&bits_to_cut), sizeof(unsigned int));
        }
        

        void hash_repetitions(
            const typename T::Sim::Format::Type * const input,
            //std::vector<uint64_t> & output
            std::vector<hashType> & output 
        ) const {
            output.resize(num_hashers);
            // Iterate through all the functions, accumulating bits
            for (size_t rep = 0; rep < num_hashers; rep++) {
                size_t offset = rep * functions_per_hasher;
                LshDatatype res = 0;
                for (unsigned int i=0; i < functions_per_hasher; i++) {
                    res <<= bits_per_function;
                    res |= (hash_functions[offset+i](input)); //Need to consider using LshDataType here to maintain bit length consistency between data types- this is just a quick fix
                }
                res >>= bits_to_cut;
                output[rep] = res.getValue();
            }
        }

        // Retrieve the number of functions this source can create.
        size_t get_size() const {
            return hash_functions.size()/functions_per_hasher;
        }

        uint_fast8_t get_bits_per_function() const {
            return bits_per_function;
        }

        float collision_probability(
            float similarity,
            uint_fast8_t num_bits
        ) const {
            return hash_family.collision_probability(similarity, num_bits);
        }

        float icollision_probability(float p) const {
            return hash_family.icollision_probability(p);
        }

        float failure_probability(
            uint_fast8_t hash_length,
            uint_fast32_t tables,
            uint_fast32_t max_tables,
            float kth_similarity
        ) const {
            float col_prob =
                this->concatenated_collision_probability(hash_length, kth_similarity);
            float last_prob =
                this->concatenated_collision_probability(hash_length+1, kth_similarity);
            return std::pow(1.0-col_prob, tables)*std::pow(1-last_prob, max_tables-tables);
        }
    };

    /// Describes a hash source where all hash functions are sampled independently.
    template <typename T, typename hashType>
    struct IndependentHashArgs : public HashSourceArgs<T, hashType> {
        /// Arguments for the hash family.
        typename T::Args args;

        IndependentHashArgs() = default;

        IndependentHashArgs(std::istream& in)
          : args(in)
        {
        }

        void serialize(std::ostream& out) const {
            HashSourceType type = HashSourceType::Independent;
            out.write(reinterpret_cast<char*>(&type), sizeof(HashSourceType));
            args.serialize(out);
        }

        std::unique_ptr<HashSource<T, hashType>> build(
            DatasetDescription<typename T::Sim::Format> desc,
            unsigned int num_tables,
            unsigned int num_bits
        ) const {
            return std::make_unique<IndependentHashSource<T, hashType>> (
                desc,
                args,
                num_tables,
                num_bits
            );
        }

        std::unique_ptr<HashSourceArgs<T, hashType>> copy() const {
            return std::make_unique<IndependentHashArgs<T, hashType>>(*this);
        }

        uint64_t memory_usage(
            DatasetDescription<typename T::Sim::Format> dataset,
            unsigned int num_tables,
            unsigned int num_bits
        ) const {
            typename T::Args args_copy(args);
            // Ensure that no expensive preprocessing is done,
            // as this method should be fast.
            args_copy.set_no_preprocessing();
            auto bits = T(dataset, args_copy).bits_per_function();
            auto funcs_per_hash = (num_bits+bits-1)/bits;
            return sizeof(IndependentHashSource<T, hashType>)
                + funcs_per_hash*num_tables*args.memory_usage(dataset);
        }

        uint64_t function_memory_usage(
            DatasetDescription<typename T::Sim::Format>,
            unsigned int /*num_bits*/
        ) const {
            return 0; // We no longer use hash functions sampled from pools
        }

        std::unique_ptr<HashSource<T, hashType>> deserialize_source(std::istream& in) const {
            return std::make_unique<IndependentHashSource<T, hashType>>(in);
        }
    };
}
