#pragma once

#include	<random>
#include	"openssl/rand.h"


typedef enum
{
	STDLIB, MSTD1988, MSTD1993, OPENSSL,
	MT19937, MT19937_64, LANLUX24B, LANLUX48B,
	LANLUX24, LANLUX48, KNUTH_B
}
TYPE_RANDOM;

typedef enum
{
	RAW, UNIFORM_INT, UNIFORM_REAL, BERNOULLI, BINOMIAL,
	N_BINOMIAL, GEOMETRIC, POISSON, EXPONENTIAL,
	GAMMA, WEIBULL, EXTREME_VLAUE, NORMAL, LOGNORAML,
	CHI_SQUARED, CAUCHY, FISHER_F, STUDENT_T,
	DISCRETE, PIECEWISE_CONSTANT, PIECEWISE_LINEAR

}
TYPE_DIST;

#define PSEUDO_RAND		0
#define DEVICE_RAND		1

class CMyRand
{
public:
	CMyRand(TYPE_RANDOM type, TYPE_DIST dtype, unsigned long min = 0, unsigned long max = 0);
	~CMyRand();

	TYPE_RANDOM	m_type;
	TYPE_DIST	m_dType;

	void SetRange(unsigned long min, unsigned long max);
	void SetParam(double param1, double param2);

	bool Rand(unsigned char *buf, int size);
	bool Rand(unsigned short *buf, int size);
	bool Rand(unsigned long *buf, int size);

private:
	unsigned long m_min;
	unsigned long m_max;
	double m_param1;
	double m_param2;
	unsigned long m_mid;
	unsigned long m_range;


	template <typename T>
	unsigned long DoDistribution(T &engine);

	unsigned long RandEngineOpenSSL(int size);
	unsigned long RandEngineRaw(int size);
};

