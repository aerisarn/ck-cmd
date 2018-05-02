/**
* Created by Bet on 10.03.2016.
* C++ converted by aerisarn
*/

#include <string>

class HkCRC {
public:
	int crc_order = 32;
	std::string crc_poly = "4C11DB7";
	std::string initial_value = "000000";
	std::string final_value = "000000";

	int reflectByte(int c);

	int* reflect(int* crc, int bitnum, int startLSB);

	std::string compute(std::string input);
};