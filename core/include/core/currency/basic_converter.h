#pragma once

#ifndef __ATINA_SERVER_CORE_CURRENCY_BASIC_CONVERTER_H__
#define __ATINA_SERVER_CORE_CURRENCY_BASIC_CONVERTER_H__

#include<string>

namespace atina::server::core::currency {

    class basic_converter {

        public:
            basic_converter(){}
            virtual ~basic_converter(){}

        public:
            /**
             * All currency codes should follow ISO 4217 standard: three capital latin letters.
             * The reason why these codes are passed with string but not enum is to make supports
             * for multiple *supported-currency-set* easier, since not all currency convert api can
             * support all currencies defined in ISO 4217 standard.
             *
             * The steps for a safe currency conversion should be:
             * * Call `is_currency_supported()` to see if both base currency and target currency are
             *   supported.
             * * Call `convert()` in your use.
             */

            /**
             * Initialize the converter.
             * If the converter doesn't need a special Initialization, leave the implementation empty.
             */
            virtual void init() = 0;

            /**
             * Check currency support. Codes are defined in ISO 4217 standard.
             */
            virtual bool is_currency_supported(const std::string& __cr_ccode) = 0;

            /**
             * Convert the currency at the latest rate. Returns -1 on error.
             */
            virtual double convert(double __amount, const std::string& __cr_from, const std::string& __cr_to) = 0;

            /**
             * Convert the currency at the rate of the given time (timestamp in seconds). Returns -1 on error.
             * If the rate of exact this time is not recorded, a rate of the nearest record should be used.
             * The real time of the rate record being used should be passed by `__op_real_time_ts`.
             */
            virtual double convert(
                uint64_t __time_ts,
                double __amount, const std::string& __cr_from, const std::string& __cr_to,
                uint64_t* __op_real_time_ts
            ) = 0;

    }; // class basic_converter

} // namespace atina::server::core::currency

#endif // __ATINA_SERVER_CORE_CURRENCY_BASIC_CONVERTER_H__
