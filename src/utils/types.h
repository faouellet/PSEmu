#ifndef UTILS_H
#define UTILS_H

#include <type_traits>

namespace Utils
{

    // TODO: Change custom integer types for (u)int*_t??
namespace details
{
/**
* \class Error
* \brief Dummy class
*/
class Error;

/**
* \struct UInt_
* \brief Defines an unsigned type that will at most contain the specified 
*        number of bytes
*/
template<int N>
struct UInt_
{
    using type = 
        std::conditional_t<sizeof(char) == N, unsigned char, 
            std::conditional_t<sizeof(short) == N, unsigned short,
                std::conditional_t<sizeof(int) == N, unsigned int,
                    std::conditional_t<sizeof(long) == N, unsigned long,
                        std::conditional_t<sizeof(long long) == N, unsigned long long, Error>>>>>;
};
/**
* \struct Int_
* \brief Defines a signed type that will at most contain the specified 
*        number of bytes
*/
template<int N>
struct Int_
{
    using type = 
        std::conditional_t<sizeof(char) == N, char, 
            std::conditional_t<sizeof(short) == N, short,
                std::conditional_t<sizeof(int) == N, int,
                    std::conditional_t<sizeof(long) == N, long,
                        std::conditional_t<sizeof(long long) == N, long long, Error>>>>>;
};

} // end namespace details

	using UInt8  = details::UInt_<1>::type;	/**< Unsigned 8 bits integer type */
	using UInt16 = details::UInt_<2>::type;	/**< Unsigned 16 bits integer type */
	using UInt32 = details::UInt_<4>::type;	/**< Unsigned 32 bits integer type */
	using UInt64 = details::UInt_<8>::type;	/**< Unsigned 64 bits integer type */

    using Int8  = details::Int_<1>::type;	/**< Signed 8 bits integer type */
    using Int16 = details::Int_<2>::type;	/**< Signed 16 bits integer type */
    using Int32 = details::Int_<4>::type;	/**< Signed 32 bits integer type */
    using Int64 = details::Int_<8>::type;	/**< Signed 64 bits integer type */

    // It's not an error to use a signed char as a byte.
    // The C and C++ standards requires that all char types have the same binary layout.
    // Consequently, conversions between char types are safe since they preserve
    // bit patterns.
    using Byte = details::Int_<1>::type;    /**< Byte type */
    
}   // end namespace Utils

#endif // UTILS_H