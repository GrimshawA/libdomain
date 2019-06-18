#ifndef LIBDOMAIN_DOM_SCHEMA_HPP
#define LIBDOMAIN_DOM_SCHEMA_HPP

#include <iostream>
#include <functional>
#include <chrono>
#include <thread>
#include <vector>
#include <functional>

namespace dom {

	class schema {
	public:

		struct FieldDef {
			std::string name;
		};


		explicit schema(const std::string &name) {
			typeName = name;
		}

		template<typename Field_t>
		schema &field(const std::string &name, Field_t addr) {
			fields.push_back({name});
			return *this;
		}

		void debug() {
			for (auto& f : fields)
			{
				std::cout << "Field: " << f.name << std::endl;
			}
		}


		std::vector<FieldDef> fields;
		std::string typeName;
	};

	template<typename T>
	schema& get_schema_for() {
		static schema broken {"NullType"};
		return broken;
	}
}

#endif //LIBDOMAIN_DOM_SCHEMA_HPP
