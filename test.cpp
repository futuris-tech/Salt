#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
using namespace std;

struct hacked_t {
	char hash[32];
	int64_t phone;
	bool operator<(const hacked_t& b) const {
		return memcmp(hash, b.hash, 32) < 0;
	}
	bool operator!=(const hacked_t& b) const {
		return memcmp(hash, b.hash, 32);
	}
};

constexpr int64_t real_phone[] = {
	89159518539,
	89637872649,
	89698570777,
	89038263485,
	89626643998
};

template<typename It, typename T>
constexpr inline It find_value(It begin, It end, const T& val) {
	auto found = lower_bound(begin, end, val);
	if (found == end || *found != val) return 0;
	return found;
}

int main() {
	const auto hacked = new hacked_t[50000];
	auto hacked_end = hacked;
	ifstream input("../hacked.txt", ios::binary);
	while (!input.eof()) {
		input.read(hacked_end->hash, 32);
		if (input.eof()) break;
		input.ignore();
		input >> hacked_end->phone;
		input.ignore();
		hacked_end++;
	}
	input.close();
	sort(hacked, hacked_end, [](const hacked_t& a, const hacked_t& b) {
		return a.phone < b.phone;
	});
	auto hacked_size = hacked_end - hacked;

	int64_t* possible_salts[5];
	for (int i = 0; i < 5; i++) {
		auto& salts = possible_salts[i];
		salts = new int64_t[hacked_size];
		for (int j = 0; j < hacked_size; j++)
			salts[j] = hacked[j].phone - real_phone[i];
	}

	int64_t real_salt = 0;
	for (int i = 0, j; i < hacked_size; i++) {
		auto salt = possible_salts[0][i];
		for (j = 1; j < 5; j++) {
			auto begin = possible_salts[j];
			auto end = begin + hacked_size;
			if (!find_value(begin, end, salt))
				break;
		}
		if (j == 5) {
			real_salt = salt;
			break;
		}
	}
	if (!real_salt) {
		cout << "salt was not found\n";
		return -1;
	}
	cout << "The salt is " << real_salt << '\n';

	sort(hacked, hacked_end);
	input.open("../input.txt", ios::binary);
	ofstream output("../result.csv", ios::binary);
	output << "md5(phone + salt),phone,salt\n";
	for (int i = 0; i < 50000; i++) {
		hacked_t value;
		input.read(value.hash, 32);
		input.ignore();
		auto found = find_value(hacked, hacked_end, value);
		if (found) {
			output.write(found->hash, 32);
			output << ',' << found->phone - real_salt
				<< ',' << real_salt << '\n';
		} else {
			cout << "not found hash ";
			cout.write(value.hash, 32);
			cout << '\n';
		}
	}

	delete[] hacked;
	for (auto salt_ptr : possible_salts)
		delete[] salt_ptr;
	return 0;
}
