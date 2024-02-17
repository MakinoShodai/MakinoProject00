/**
 * @file   UtilityForType.h
 * @brief  This file defines utility functions that are useful for type operations.
 * 
 * @author Shodai Makino
 * @date   2024/1/2
 */

#ifndef __UTILITY_FOR_TYPE_H__
#define __UTILITY_FOR_TYPE_H__

namespace Utl {
    namespace Type {
        /** @brief Type relationship checking */
        namespace Relat {
            /**
               @brief Are the two templates in an inheritance relationship?
               @tparam T First type
               @tparam U Second type
            */
            template<class T, class U>
            concept IsInheritance = std::is_base_of<T, U>::value || std::is_base_of<U, T>::value;
            /**
               @brief Are the two templates in an inheritance relationship? (remove extent)
               @tparam T First type
               @tparam U Second type
            */
            template<class T, class U>
            concept IsInheritanceRemoveExtent = IsInheritance<typename std::remove_extent<T>::type, typename std::remove_extent<U>::type>;


            /**
               @brief std::is_base_of and std::remove_extent and not the same type
               @tparam Base Base class type
               @tparam Derived Derived class type
            */
            template<class Base, class Derived>
            concept IsBaseOfRemoveExtentNotSame 
                = std::is_base_of<typename std::remove_extent<Base>::type, typename std::remove_extent<Derived>::type>::value
                && std::is_same_v<Base, Derived>;
        }

        /** @brief Type trait checking */
        namespace Traits {
            /**
               @brief Is it floating point?
               @tparam T Type to be checked
            */
            template<typename T>
            concept IsFloatingPoint = std::is_same_v<T, float> || std::is_same_v<T, double>;


            /**
               @brief Check if a type has a copy constructor 
               @tparam T Type to be checked
            */
            template <typename T, typename = std::void_t<>>
            struct HasCopyConstructor : std::false_type {};
            /**
               @brief Check if a type has a copy constructor
               @tparam T Type to be checked
            */
            template <typename T>
            struct HasCopyConstructor<T, std::void_t<decltype(T(std::declval<const T&>()))>> : std::true_type {};


            /**
               @brief Check if a type has a copy constructor
               @tparam T Type to be checked
            */
            template <typename T, typename = std::void_t<>>
            struct HasCopyOperator : std::false_type {};
            /**
               @brief Check if a type has a copy constructor
               @tparam T Type to be checked
            */
            template <typename T>
            struct HasCopyOperator<T, std::void_t<decltype(std::declval<T&>() = std::declval<const T&>())>> : std::true_type {};


            /**
               @brief Evaluation expression to check if T matches the internal type of std::variant
               @tparam T Type to be checked
               @tparam Variant std::variant to be compared
            */
            template<typename T, typename Variant>
            struct IsVariantMember;
            /**
               @brief Evaluation expression to check if T matches the internal type of std::variant
               @tparam T Type to be checked
               @tparam Variant std::variant to be compared
            */
            template<typename T, typename... Types>
            struct IsVariantMember<T, std::variant<Types...>> : std::bool_constant<(std::is_same_v<T, Types> || ...)> {};


            /** @brief Internal function of 'CheckVariantTypes'  */
            template <typename Variant, template <typename> class Evaluation, size_t... Is>
            constexpr bool InnerCheckVariantTypes(std::index_sequence<Is...>) {
                return (... && Evaluation<std::variant_alternative_t<Is, Variant>>::value);
            }
            /**
               @brief Check all internal types of std::variant with evaluation expressions
               @tparam Variant std::variant to be checked
               @tparam Evaluation Evalution expression (Must have static constexpr bool value)
               @return Result
            */
            template <typename Variant, template <typename> class Evaluation>
            constexpr bool CheckVariantTypes() {
                return InnerCheckVariantTypes<Variant, Evaluation>(std::make_index_sequence<std::variant_size_v<Variant>>{});
            }
        } // namespace Traits

        /** @brief Type conversion */
        namespace Conv {
            /**
               @brief Add const to type
               @tparam T Type to be added
            */
            template<typename T>
            struct AddConstToType {
                using type = const T;
            };
            /**
               @brief Add const to type for pointer type
               @tparam T Type to be added
            */
            template<typename T>
                requires std::is_pointer_v<T>
            struct AddConstToType<T> {
                using type = const std::remove_pointer_t<T>*;
            };


            /**
               @brief Helper structure to apply const to all types of std::variant
               @tparam Ts Internal type of std::variant
            */
            template<typename... Ts>
            struct ConstVariant;
            /**
               @brief Helper structure to apply const to all types of std::variant
               @tparam Ts Internal type of std::variant
               @details
               If std::variant is specified directly, the internal type is specified
            */
            template<typename... Ts>
            struct ConstVariant<std::variant<Ts...>> {
                using type = std::variant<typename AddConstToType<Ts>::type...>;
            };
        } // namespace Conv

    } // namespace Type
} // namespace Utl

#endif // !__UTILITY_FOR_TYPE_H__
