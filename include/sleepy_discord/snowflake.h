#pragma once
#include <string>
#include <chrono>
#include <algorithm>
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS)
#include <stdexcept>
#endif
#include "nonstd/string_view.hpp"
#include "json_wrapper.h"

namespace SleepyDiscord {
	//Stops you from mixing up different types of ids, like using a message_id as a user_id
	template <typename DiscordObject>
	struct Snowflake {
		Snowflake(                                  ) : raw(""                            ) {}
		Snowflake(const std::string         & snow  ) : raw( snow                         ) {}
		Snowflake(const std::string         * snow  ) : raw(*snow                         ) {}
		Snowflake(const char                * snow  ) : raw( snow                         ) {}
		Snowflake(const nonstd::string_view & snow  ) : raw( snow.to_string(             )) {}
		Snowflake(const DiscordObject       & object) : Snowflake(object. ID              ) {}
		Snowflake(const DiscordObject       * object) : Snowflake(object->ID              ) {}
		Snowflake(const int64_t               number) : Snowflake(std::to_string(number  )) {}
		Snowflake(const json::Value         & value ) : Snowflake(json::toStdString(value)) {}

		inline bool operator==(const Snowflake& right) const {
			return raw == right.raw;
		}

		inline bool operator!=(const Snowflake& right) const {
			return raw != right.raw;
		}

		inline bool operator==(const char* right) const {
			return raw == right;
		}

		inline bool operator!=(const char* right) const {
			return raw != right;
		}

		inline operator const std::string&() const { return raw; }

		inline const std::string& string() const { return operator const std::string&(); }
		inline const int64_t& number() const { return std::stoll(raw); }

		std::chrono::time_point<std::chrono::steady_clock> timestamp() const {
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS)
			if (raw == "") throw std::invalid_argument("invalid snow in Snowflake");
#endif
			return std::chrono::time_point<std::chrono::steady_clock>(std::chrono::milliseconds((std::stoll(raw) >> 22) + discordEpoch));
		}

		template<class iterator>
		inline iterator findObject(iterator begin, iterator end) const {
			return std::find_if(begin, end, [&](const DiscordObject& object) {
				return this->operator==(static_cast<DiscordObject>(object));
			});
		}

/*

		template<class Container, class = int>
		struct HasFindFuncation : std::false_type {};

		template<class Container>
		struct HasFindFunction<Container, decltype((void) Container::find)> : std::true_type {};

		template<class Container>
		inline typename std::enable_if<HasFindFunction<Container>::value == true, decltype(Container::find())>
		findObject(const Container& objects) const {
			return objects.find(*this);
		}

*/
		template<class Container>
		//inline typename std::enable_if<HasFindFuncation<Container>::value == false, decltype(Container::begin())>
		auto findObject(Container& objects) const -> decltype(objects.begin()) {
			return findObject(objects.begin(), objects.end());
		}

	private:
		std::string raw;
		static const time_t discordEpoch = 1420070400000;	//the first second of 2015 since epoch
	};

	template <typename DiscordOject>
	inline std::string operator+(const char * left, Snowflake<DiscordOject>& right) {
		return left + right.operator const std::string&();
	}

	template <typename DiscordOject>
	inline bool operator==(const char * left, Snowflake<DiscordOject>& right) {
		return left == right.operator const std::string&().c_str();
	}

	template <typename DiscordOject>
	inline bool operator!=(const char * left, Snowflake<DiscordOject>& right) {
		return left != right.operator const std::string&().c_str();
	}
}