#pragma once

#ifndef __ATINA_SERVER_CORE_CURRENCY_CONVERTER_FRANKFURTER_H__
#define __ATINA_SERVER_CORE_CURRENCY_CONVERTER_FRANKFURTER_H__

#include<cstdint>
#include<memory>
#include<string>
#include<vector>

#include"core/currency/basic_converter.h"

namespace atina::server::core::currency {

    class converter_frankfurter final : public basic_converter {

        public:
            converter_frankfurter();
            virtual ~converter_frankfurter();

        public:
            struct currency_data {
                std::string code; // currency code
            }; // struct currency_data

            struct rate_data {
                int id;
                uint64_t time_ts;
                std::string target;
                double rate;
                std::string extra;
            }; // struct rate_data

        public:
            virtual void init() override;
            virtual bool is_currency_supported(const std::string& __cr_ccode) override;
            virtual double convert(double __amount, const std::string& __cr_from, const std::string& __cr_to) override;
            virtual double convert(
                uint64_t __time_ts,
                double __amount, const std::string& __cr_from, const std::string& __cr_to,
                uint64_t* __op_real_time_ts
            ) override;

        private:
            struct _impl;
            std::unique_ptr<_impl> _p_impl;

            int _refresh_latest_rate_data();

            inline static const std::string _base_currency = "EUR";
            inline static const std::vector<std::string> _supported_currencies = {
                "AUD", // Australian dollar
                "BGN", // Bulgarian lev
                "BRL", // Brazilian real
                "CAD", // Canadian dollar
                "CHF", // Swiss franc
                "CNY", // Chinese yuan
                "CZK", // Czech koruna
                "DKK", // Danish krone
                "EUR", // Euro
                "GBP", // British pound
                "HKD", // Hong Kong dollar
                "HUF", // Hungarian forint
                "IDR", // Indonesian rupiah
                "ILS", // Israeli new shekel
                "INR", // Indian rupee
                "ISK", // Icelandic króna
                "JPY", // Japanese yen
                "KRW", // South Korean won
                "MXN", // Mexican peso
                "MYR", // Malaysian ringgit
                "NOK", // Norwegian krone
                "NZD", // New Zealand dollar
                "PHP", // Philippine peso
                "PLN", // Polish złoty
                "RON", // Romanian leu
                "SEK", // Swedish krona
                "SGD", // Singapore dollar
                "THB", // Thai baht
                "TRY", // Turkish lira
                "USD", // US dollar
                "ZAR"  // South African rand
            }; // can be fetched from API

    }; // class converter_frankfurter

} // namespace atina::server::core::currency

#endif // __ATINA_SERVER_CORE_CURRENCY_CONVERTER_FRANKFURTER_H__
