//
// Created by blomq on 2025-08-13.
//

#ifndef PSYENGINE_RAND_HPP
#define PSYENGINE_RAND_HPP

#include <algorithm>
#include <array>
#include <functional>
#include <random>
#include <type_traits>
#include <vector>

namespace psyengine::rand
{
    namespace detail
    {

        template <typename T, typename U = void>
        /**
         * @brief Specialization of the HasStateSize trait for types that do not provide a `state_size` member.
         *
         * This struct is the default implementation of the HasStateSize trait and inherits from `std::false_type`.
         * It represents the absence of a `state_size` member in the specified type.
         *
         * @tparam T The type being checked for a `state_size` member.
         * @tparam U A placeholder template parameter for SFINAE defaults to `void`.
         */
        struct HasStateSize : std::false_type
        {
        };

        template <typename T>
        /**
         * @brief Trait to detect whether a type provides a `state_size` member.
         *
         * Specialization for types that define a `state_size` member. When a type `T` has a
         * `state_size` member, this specialization inherits from `std::true_type`, effectively
         * evaluating to true for trait checks.
         *
         * @tparam T The type to be checked for the presence of a `state_size` member.
         */
        struct HasStateSize<T, std::void_t<decltype(T::state_size)>> : std::true_type
        {
        };

        template <typename T>
        inline constexpr bool HAS_STATE_SIZE_V = HasStateSize<T>::value;

        // Check engine can be constructed/seeded from a SeedSequence
        template <typename Engine>
        concept SeedSeqConstructibleOrSeedable = requires(std::seed_seq& s, Engine& e)
        {
            Engine{s}; // constructor from a seed sequence
            e.seed(s); // or ability to seed with a seed sequence
        };


        template <typename Engine>
        /**
         * @brief Computes and returns the number of 32-bit words to use for seeding a random number generator (RNG) engine.
         *
         * @tparam Engine The RNG engine type to consider. If the type has a `state_size` member,
         * its value is used. Otherwise, a default value of 16 (representing 512 bits of seed material) is returned.
         *
         * @return The number of 32-bit words required for seeding the specified Engine type. If `state_size` is
         * defined for the Engine, its value is used. In the absence of `state_size`, a default heuristic of 16 words
         * is applied to ensure sufficient diffusion of randomness while maintaining computational efficiency.
         */
        constexpr std::size_t SeedWordCount()
        {
            if constexpr (HAS_STATE_SIZE_V<Engine>)
            {
                return static_cast<std::size_t>(Engine::state_size);
            }
            else
            {
                // Heuristic: more than 1 to give good diffusion, but not huge.
                // 16 x 32-bit words = 512 bits of seed material.
                return 16;
            }
        }

        // Generate a seed_seq with N 32-bit words from std::random_device
        template <typename Engine>
        std::seed_seq MakeSeedSeq()
        {
            std::random_device rd;
            constexpr std::size_t n = SeedWordCount<Engine>();
            std::vector<std::seed_seq::result_type> seedData;
            seedData.reserve(n);
            std::generate_n(std::back_inserter(seedData), n, std::ref(rd));

            return {std::begin(seedData), std::end(seedData)};
        }
    } // namespace detail

    template <typename Engine>
        requires (std::uniform_random_bit_generator<Engine> &&
            detail::SeedSeqConstructibleOrSeedable<Engine>)
    /**
     * @brief Creates and returns a seeded random number generator (RNG) of the specified Engine type.
     *
     * @tparam Engine The type of the RNG engine to create. Must satisfy the requirements of
     * std::uniform_random_bit_generator and be either constructible from or seedable with a std::seed_seq.
     *
     * @return A seeded RNG of the specified Engine type. If the Engine can be directly constructed
     * using a std::seed_seq, the generator is initialized in this manner. Otherwise, the Engine is
     * default-constructed and seeded using its seed member function. The seeding process is based on
     * random seed material derived from std::random_device.
     *
     * @note This function is marked with [[nodiscard]] to prevent the created RNG from being
     * accidentally discarded, ensuring robust random number generation. The randomness quality
     * depends on the underlying implementation of std::random_device.
     */
    [[nodiscard]] Engine MakeSeededRng()
    {
        if constexpr (auto seq = detail::MakeSeedSeq<Engine>();
            requires { Engine{seq}; })
        {
            return Engine(seq);
        }
        else
        {
            Engine eng{};
            eng.seed(seq);
            return eng;
        }
    }

    template <typename Engine>
        requires (std::uniform_random_bit_generator<Engine> &&
            detail::SeedSeqConstructibleOrSeedable<Engine>)
    /**
     * @brief Creates and returns a seeded random number generator (RNG) of the specified Engine type.
     *
     * @tparam Engine The type of the RNG engine to create. Must satisfy the requirements of
     * std::uniform_random_bit_generator and be either constructible from or seedable with a std::seed_seq.
     *
     * @param seedSize The number of 32-bit words to use as seed material. The random seed material
     * is generated using std::random_device.
     *
     * @return A seeded RNG of the specified Engine type. The generator is seeded using a seed sequence
     * constructed from the specified number of 32-bit seed words. If the Engine supports construction
     * from a std::seed_seq, it is constructed directly. Otherwise, it is default-constructed and seeded
     * via the seed member function.
     *
     * @note This function is marked with [[nodiscard]] to ensure the created RNG is not inadvertently
     * discarded. The quality and randomness of the RNG depend on the underlying std::random_device
     * implementation.
     */
    [[nodiscard]] Engine MakeSeededRngWithWords(const std::size_t seedSize)
    {
        std::random_device rd;
        std::vector<std::seed_seq::result_type> seedData;
        seedData.reserve(seedSize);
        std::generate_n(std::back_inserter(seedData), seedSize, std::ref(rd));

        if constexpr (std::seed_seq seq(std::begin(seedData), std::end(seedData));
            requires { Engine{seq}; })
        {
            return Engine(seq);
        }
        else
        {
            Engine eng{};
            eng.seed(seq);
            return eng;
        }
    }


    template <typename Engine>
        requires (std::uniform_random_bit_generator<Engine> &&
            detail::SeedSeqConstructibleOrSeedable<Engine>)
    /**
     * @brief Creates and returns a custom-seeded random number generator engine.
     *
     * This function generates a random number generator engine of the specified type `Engine` that is
     * seeded with a user-provided 64-bit integer seed. It internally uses a SplitMix64-like algorithm
     * to expand the given 64-bit seed into the number of 32-bit words required by the engine's seed sequence.
     *
     * If the `Engine` type supports direct construction with an ` std::seed_seq ` instance, it is used to construct
     * and initialize the engine. Otherwise, the engine is default-constructed and seeded explicitly.
     *
     * @tparam Engine The type of the random number generator being created. Must be compatible with `std::seed_seq`.
     * @param userSeed A 64-bit seed value provided by the user for initializing the random number generator.
     * @return A constructed and seeded instance of the `Engine` type.
     *
     * @details
     * The splitmix64-like generator function `next32` generates 32-bit words by transforming the input `userSeed`
     * through a series of bitwise and arithmetic operations. The resulting words are used to populate a
     * `std::vector` of the required size for the seed sequence.
     *
     * The number of 32-bit words required is determined by `detail::SeedWordCount<Engine>()`. These words
     * are used to initialize a `std::seed_seq`, which seeds the engine. The function selects the appropriate
     * seeding method based on the engine's construction requirements.
     */
    [[nodiscard]] Engine MakeCustomSeededRng(const std::uint64_t userSeed)
    {
        // Expand a single 64-bit seed into many 32-bit words with splitmix64
        std::uint64_t sm = userSeed;
        auto next32 = [&sm]() -> std::uint32_t
        {
            sm += 0x9E3779B97F4A7C15ull; // advance state by golden ratio
            std::uint64_t z = sm;
            z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull; // mix
            z = (z ^ (z >> 27)) * 0x94D049BB133111EBull; // mix
            z ^= (z >> 31);
            return static_cast<std::uint32_t>(z);
        };

        constexpr std::size_t n = detail::SeedWordCount<Engine>();
        std::vector<std::seed_seq::result_type> seedData;
        seedData.reserve(n);
        std::generate_n(std::back_inserter(seedData), n, next32);

        std::seed_seq seq(std::begin(seedData), std::end(seedData));
        if constexpr (requires { Engine{seq}; })
        {
            return Engine(seq);
        }
        else
        {
            Engine eng{};
            eng.seed(seq);
            return eng;
        }
    }


    using Mersenne32 = std::mt19937;
    using Mersenne64 = std::mt19937_64;

    /**
     * @brief Creates and returns a seeded Mersenne Twister random number generator with a 32-bit state size.
     *
     * This function uses the MakeSeededRng function to initialize a Mersenne Twister engine
     * (std::mt19937) with a random seed value. The engine is seeded using random material generated
     * from a high-quality source, such as std::random_device.
     *
     * @return A seeded Mersenne Twister random number generator (std::mt19937). The generator is
     * initialized to provide robust and high-quality random number generation.
     *
     * @note Proper seeding ensures that the RNG produces a random enough and non-repeating
     * sequence of numbers.
     */
    inline auto MakeMersenne32() { return MakeSeededRng<Mersenne32>(); }

    /**
     * @brief Creates and returns a seeded Mersenne Twister random number generator with a 64-bit state size.
     *
     * This function uses the MakeSeededRng function to initialize a Mersenne Twister engine
     * (std::mt19937_64) with a random seed value. The engine is seeded using random material generated
     * from a high-quality source, such as std::random_device.
     *
     * @return A seeded Mersenne Twister random number generator (std::mt19937_64). The generator is
     * initialized to provide robust and high-quality random number generation.
     *
     * @note Proper seeding ensures that the RNG produces a random enough and non-repeating
     * sequence of numbers.
     */
    inline auto MakeMersenne64() { return MakeSeededRng<Mersenne64>(); }

    inline auto MakeMersenne32CustomSeed(const std::uint64_t seed) { return MakeCustomSeededRng<Mersenne32>(seed); }
    inline auto MakeMersenne64CustomSeed(const std::uint64_t seed) { return MakeCustomSeededRng<Mersenne64>(seed); }
} // namespace utils::rand
#endif //PSYENGINE_RAND_HPP
