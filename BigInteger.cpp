#include "pch.h"
#include "BigInteger.h"


std::string BigInteger::IntToString(int value)
{
	std::stringstream ss;
	ss << value;
	std::string str_value = ss.str();

	return str_value;
}

BigInteger::BigInteger()
{
	std::fill_n(value, MAX_DIGIT_SIZE, 0);
}

BigInteger::BigInteger(std::string value)
	: BigInteger()
{
	Init(value);
}

BigInteger::BigInteger(int value)
	: BigInteger()
{
	Init(BigInteger::IntToString(value));
}

BigInteger::BigInteger(int value[], unsigned int size)
	: BigInteger()
{
	int finalValue = 0;
	for (int i = 0; i < size; i++)
	{
		finalValue += value[i] * pow(10, i);
	}

	Init(BigInteger::IntToString(finalValue));
}

BigInteger::BigInteger(const BigInteger & bi)
	: BigInteger()
{
	std::copy(bi.value, bi.value + sizeof(unsigned char) * MAX_DIGIT_SIZE, value);
	size = bi.size;
	isNegative = bi.isNegative;
}

void BigInteger::Init(std::string value)
{
	const char* chr_value = value.c_str();
	unsigned char digitSize = strlen(chr_value);
	bool hasSignChar = false;

	// ù ��° ���� ��ȣ��ȣ üũ
	char first_chr = chr_value[0];

	if (first_chr == '+')
	{
		this->isNegative = false;
		hasSignChar = true;
	}
	else if (first_chr == '-')
	{
		this->isNegative = true;
		hasSignChar = true;
	}

	// ���� �ڸ��� ���� ä����
	for (int i = 0; i < digitSize; i++)
	{
		char chr = chr_value[digitSize - i - 1];
		if ('0' <= chr && chr <= '9')
		{
			this->value[i] = chr - '0';
		}
	}

	this->size = digitSize - (hasSignChar ? 1 : 0);
}

std::string BigInteger::ToString()
{
	std::string str;

	if (size == 0)
	{
		return "0";
	}

	if (isNegative)
	{
		str = "-";
	}

	for (int i = size - 1; i >= 0; i--)
	{
		str += std::string(1, (signed)(value[i] + '0'));
	}

	return str;
}

BigInteger BigInteger::operator-(BigInteger & bi)
{
	BigInteger negValue(bi);
	negValue.isNegative = !negValue.isNegative;

	return (*this + negValue);
}

BigInteger BigInteger::operator+(BigInteger & bi)
{
	BigInteger finalValue;

// ��� 1 : ��� �ڸ����� �����. �Ѿ�� ���� ���� �ڸ����� ����(�迭���� 1�� �ڸ��� ���� ����). �迭���� �ٸ� ��ȣ ����.
	int rawValues[MAX_DIGIT_SIZE];
	int overValue = 0;
	int prevComputeSize = 0;
	for (int i = 0; i < MAX_DIGIT_SIZE; i++)
	{
		int currOverValue = overValue % 10;
		int a = 0;
		int b = 0;
		bool a_exist = i < this->size;
		bool b_exist = i < bi.size;

		if (!a_exist && !b_exist && overValue == 0)
			break;

		if (a_exist)
			a = this->value[i] * (this->isNegative ? -1 : 1);

		if (b_exist)
			b = bi.value[i] * (bi.isNegative ? -1 : 1);

		int computed = a + b;

		int computedSize = abs(computed + currOverValue);
		int computedSign = (computed + currOverValue) < 0 ? -1 : 1;

		// ���� �ڸ����� ���� ��ġ���� Ȯ��
		// ��ĥ��� ���� �ڸ������� �����ֱ� ���� over value �� ����
		if (computedSize >= 10)
		{
			overValue += (computedSize - computedSize % 10) * computedSign;
			computedSize %= 10;
		}

		rawValues[i] = computedSize * computedSign;

		prevComputeSize++;
		overValue /= 10;
	}

// ��� 2 : ��ȣ ����. �ϳ��� ��ȣ�� ����.
	// ���� ���� �ڸ��� ��ȣ (��������?)
	bool standardSign = rawValues[prevComputeSize - 1] < 0;

	for (int i = prevComputeSize - 2; i >= 0; i--)
	{
		bool currDigitSign = rawValues[i] < 0;

		if (rawValues[i] == 0)
			continue;

		if (currDigitSign != standardSign)
		{
			// �� �ܰ� ���� �ڸ����� ���� ������
			int prevIndex = i + 1;
			rawValues[prevIndex] -= standardSign ? -1 : 1;

			// current value
			rawValues[i] += (standardSign ? -1 : 1) * 10; // ������ �� ����

		}
	}

// ���� : big integer�� ��ȯ
	int lastNotZeroPosition = 0; // ���������� 0�� �ƴ� ��ġ
	finalValue.isNegative = standardSign;
	for (int i = 0; i < prevComputeSize; i++)
	{
		if (rawValues[i] != 0)
		{
			lastNotZeroPosition = i + 1;
		}
		finalValue.value[i] = abs(rawValues[i]);
	}

	finalValue.size = lastNotZeroPosition;

	return finalValue;
}

BigInteger BigInteger::operator*(BigInteger & bi)
{
	BigInteger finalValue;

	std::vector<BigInteger*> multValues;
	multValues.reserve(bi.size);

	for (int i = 0; i < bi.size; i++)
	{
		BigInteger* biMultResult = new BigInteger(*this);
		multValues.push_back(biMultResult);

		int multiplyValue = bi.value[i];

		// operator+ ���� ������. +�������̶� �����
		int rawValues[MAX_DIGIT_SIZE];
		int overValue = 0;
		int prevComputeSize = 0;

		for (int j = 0; j < MAX_DIGIT_SIZE; j++)
		{
			int currOverValue = overValue % 10;

			bool a_exist = j < biMultResult->size;

			if (!a_exist && overValue == 0)
				break;

			int a = biMultResult->value[j];
			int b = multiplyValue;

			// ��ȣ�� ��� ���� Ȯ����. ������ ���밪���� ���.
			int computed = abs(a * b);
			int computedSize = computed + currOverValue;

			if (computedSize >= 10)
			{
				overValue += (computedSize - computedSize % 10);
				computedSize %= 10;
			}

			rawValues[j] = computedSize;

			prevComputeSize++;
			overValue /= 10;
		}

		int lastNotZeroPosition = 0; // ���������� 0�� �ƴ� ��ġ

		for (int j = 0; j < prevComputeSize; j++)
		{
			if (rawValues[j] != 0)
			{
				lastNotZeroPosition = j + 1;
			}

			biMultResult->value[j] = rawValues[j];
		}
		biMultResult->size = lastNotZeroPosition;
		//

		// ���� �ڸ����� ���� ����Ʈ : *(10^n)
		int rightShiftAmount = i;
		if (rightShiftAmount != 0)
		{
			for (int j = biMultResult->size - 1; j >= 0; j--)
			{
				if (j + rightShiftAmount > MAX_DIGIT_SIZE)
					continue;

				biMultResult->value[j + rightShiftAmount] = biMultResult->value[j];
			}

			for (int j = 0; j < rightShiftAmount; j++)
			{
				biMultResult->value[j] = 0;
			}

			biMultResult->size += rightShiftAmount;
		}
		
	}

	// ��� ���ϱ�
	while (multValues.size() > 0)
	{
		finalValue = finalValue + **multValues.begin();

		delete *multValues.begin();
		multValues.erase(multValues.begin());
	}

	finalValue.isNegative = this->isNegative != bi.isNegative;
	return finalValue;
}

const BigInteger BigInteger::Compute(std::string exp)
{
	BigInteger finalValue;
	std::vector<std::string> expSplitList;
	std::vector<BigInteger> numberList;

	// trim
	exp = std::regex_replace(exp, std::regex("\\s"), "");

	std::smatch m;
	auto str_it = exp.cbegin();
	while (std::regex_search(str_it, exp.cend(), m, std::regex("\\d+|[\\+\\-\\*]"))) // decimal or operator
	{
		std::string str = *m.begin();
		expSplitList.push_back(str);

		str_it = (*m.begin()).second;
	}

	std::string oper = "";
	bool isPrevOperator = true;
	bool nextValueIsNegative = false;
	for (auto it = expSplitList.cbegin(); it != expSplitList.cend(); it++)
	{
		if (std::regex_search(*it, m, std::regex("[\\+\\-\\*]")))
		{
			// operator
			if (isPrevOperator)
			{
				// ��ȣ ��ȣ
				if ((*it).compare("-") == 0)
				{
					nextValueIsNegative = true;
				}
				else
				{
					nextValueIsNegative = false;
				}
			}
			else
			{
				// ���� ���� ��ȣ
				oper = *it;
			}

			isPrevOperator = true;
		}
		else
		{
			// decimal
			BigInteger bi(*it);
			bi.isNegative = nextValueIsNegative;

			numberList.push_back(bi);

			isPrevOperator = false;
		}
	}

	if (numberList.size() >= 2)
	{
		if (oper.compare("+") == 0)
		{
			finalValue = numberList[0] + numberList[1];
		}
		else if (oper.compare("-") == 0)
		{
			finalValue = numberList[0] - numberList[1];
		}
		else if (oper.compare("*") == 0)
		{
			finalValue = numberList[0] * numberList[1];
		}
	}

	return finalValue;
}

std::ostream & operator<<(std::ostream & os, BigInteger & bi)
{
	return os << bi.ToString();
}