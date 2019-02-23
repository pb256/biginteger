#pragma once

#define MAX_DIGIT_SIZE 200

class BigInteger
{
private:
	unsigned char value[MAX_DIGIT_SIZE];
	unsigned char size = 0;
	bool isNegative = false;

	static std::string IntToString(int value);
public:
	BigInteger();
	BigInteger(std::string value);
	BigInteger(int value);
	BigInteger(int value[], unsigned int size);
	BigInteger(const BigInteger& bi);
	~BigInteger() {}

	void Init(std::string value);
	std::string ToString();

	BigInteger operator- (BigInteger& bi);
	BigInteger operator+ (BigInteger& bi);
	BigInteger operator* (BigInteger& bi);

	static const BigInteger Compute(std::string exp);
};

std::ostream& operator<< (std::ostream& os, BigInteger& bi);