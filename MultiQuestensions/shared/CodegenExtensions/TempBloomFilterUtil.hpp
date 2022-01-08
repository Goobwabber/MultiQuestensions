#pragma once
namespace GlobalNamespace {
    class BloomFilterUtil : public ::Il2CppObject {
    public:
        template<class T>
        static T ToBloomFilter(::Il2CppString* value, int hashCount, int hashBits) {
            //static_assert(std::is_base_of_v<GlobalNamespace::IBitMask_1<T>, std::remove_pointer_t<T>>);
            static auto ___internal__logger = ::Logger::get().WithContext("GlobalNamespace::BloomFilterUtil::ToBloomFilter");
            static auto* ___internal__method = THROW_UNLESS((::il2cpp_utils::FindMethod("", "BloomFilterUtil", "ToBloomFilter", std::vector<Il2CppClass*>{::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<T>::get()}, ::std::vector<const Il2CppType*>{::il2cpp_utils::ExtractType(value), ::il2cpp_utils::ExtractType(hashCount), ::il2cpp_utils::ExtractType(hashBits)})));
            static auto* ___generic__method = THROW_UNLESS(::il2cpp_utils::MakeGenericMethod(___internal__method, std::vector<Il2CppClass*>{::il2cpp_utils::il2cpp_type_check::il2cpp_no_arg_class<T>::get()}));
            return ::il2cpp_utils::RunMethodRethrow<T, false>(static_cast<Il2CppObject*>(nullptr), ___generic__method, value, hashCount, hashBits);
        }
    };
}
