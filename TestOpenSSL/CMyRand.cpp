#include "CMyRand.h"

#include <time.h>
#include <random>





CMyRand::CMyRand(TYPE_RANDOM type, TYPE_DIST dtype, unsigned long min, unsigned long max)
{
	m_type = type;
	m_dType = dtype;
	m_min = min;
	m_max = max;

	if (type == STDLIB)		srand(time(NULL));
	if (type == OPENSSL)	RAND_status();


	m_param1 = 0.f;
	m_param2 = 1.f;

	// default parameter
	switch (m_dType)
	{
		case BERNOULLI:				m_param1 = 0;						break;
		case BINOMIAL:				m_param1 = 1;	m_param2 = 0.5;		break;
		case N_BINOMIAL:			m_param1 = 1;	m_param2 = 0.5;		break;
		case GEOMETRIC:				m_param1 = 0.5;						break;
		case POISSON:				m_param1 = 1;						break;
		case EXPONENTIAL:			m_param1 = 1;						break;
		case GAMMA:					m_param1 = 1;	m_param2 = 1;		break;
		case WEIBULL:				m_param1 = 1;	m_param2 = 1;		break;
		case EXTREME_VLAUE:			m_param1 = 0;	m_param2 = 1;		break;
		case NORMAL:				m_param1 = 0;	m_param2 = 1;		break;
		case LOGNORAML:				m_param1 = 0;	m_param2 = 1;		break;
		case CHI_SQUARED:			m_param1 = 1;						break;
		case CAUCHY:				m_param1 = 0;	m_param2 = 1;		break;
		case FISHER_F:				m_param1 = 1;	m_param2 = 1;		break;
		case STUDENT_T:				m_param1 = 1;						break;
		case DISCRETE:				m_param1 = 0;						break;
		case PIECEWISE_CONSTANT:	m_param1 = 0;						break;
		case PIECEWISE_LINEAR:		m_param1 = 0;						break;
	}
}


CMyRand::~CMyRand()
{
}


void CMyRand::SetRange(unsigned long min, unsigned long max)
{
	m_min = min;
	m_max = max;

	if (min == max)		m_range = 0;
	else				m_range = max - min + 1;
}


void CMyRand::SetParam(double param1, double param2)
{
	m_param1 = param1;
	m_param2 = param2;
}


template <typename T>
unsigned long CMyRand::DoDistribution(T &engine)
{
	double	value = 0;


	switch (m_dType)
	{
		case UNIFORM_INT:
		{
			std::uniform_int_distribution<> dis(m_min, m_max);	// default MIN=0, MAX=2147483647
			value = dis(engine);
			break;
		}

		case UNIFORM_REAL:
		{
			std::uniform_real_distribution<> dis(m_param1, m_param2);	// deault MIN=0, MAX=1
			value = dis(engine);
			break;
		}

		case BERNOULLI:
		{
			std::bernoulli_distribution dis(m_param1);				// default P=0.5
			value = dis(engine);
			break;
		}

		case BINOMIAL:
		{
			std::binomial_distribution<> dis(m_param1, m_param2);	// default T=1, P=0.5
			value = dis(engine);
			break;
		}

		case N_BINOMIAL:
		{
			std::negative_binomial_distribution<> dis(m_param1, m_param2);	// default K=1, P=0.5
			value = dis(engine);
			break;
		}

		case GEOMETRIC:
		{
			std::geometric_distribution<> dis(m_param1);	// default P = 0.5
			value = dis(engine);
			break;
		}

		case POISSON:
		{
			std::poisson_distribution<> dis(m_param1);		// default Mean = 1.0
			value = dis(engine);
			break;
		}

		case EXPONENTIAL:
		{
			std::exponential_distribution<> dis(m_param1);	// no default Lamda. I will set to 1
			value = dis(engine);
			break;
		}

		case GAMMA:
		{
			std::gamma_distribution<> dis(m_param1, m_param2);	// default alpha=1, beta=1 
			value = dis(engine);
			break;
		}

		case WEIBULL:
		{
			std::weibull_distribution<> dis(m_param1, m_param2); // default A=1, B=1 
			value = dis(engine);
			break;
		}

		case EXTREME_VLAUE:
		{
			std::extreme_value_distribution<> dis(m_param1, m_param2);	// default A=0, B=1 
			value = dis(engine);
			break;
		}

		case NORMAL:
		{
			std::normal_distribution<> dis(m_param1, m_param2);	// default A=0, B=1 
			value = dis(engine);
			break;
		}

		case LOGNORAML:
		{
			std::lognormal_distribution<> dis(m_param1, m_param2);	// default M=0, S=1 
			value = dis(engine);
			break;
		}

		case CHI_SQUARED:
		{
			std::chi_squared_distribution<> dis(m_param1);			// default N=1 
			value = dis(engine);
			break;
		}

		case CAUCHY:
		{
			std::cauchy_distribution<> dis(m_param1, m_param2);	// default A=0, B=1 
			value = dis(engine);
			break;
		}

		case FISHER_F:
		{
			std::fisher_f_distribution<> dis(m_param1, m_param2);	// default M=1, N=1 
			value = dis(engine) * 10;
			break;
		}

		case STUDENT_T:
		{
			std::student_t_distribution<> dis(m_param1);	// default N=1 
			value = dis(engine);
			break;
		}

		case DISCRETE:
		{
			std::discrete_distribution<> dis({1, 100, 1, 100});		// default  
			value = dis(engine);
			break;
		}

		case PIECEWISE_CONSTANT:
		{
			std::vector<double> i{ 0, 5, 10, 15 };
			std::vector<double> w{ 0, 1,   1, 0 };
			std::piecewise_constant_distribution<> dis(i.begin(), i.end(), w.begin());	// default
			value = dis(engine);
			break;
		}

		case PIECEWISE_LINEAR:
		{
			std::vector<double> i{ 0, 5, 10, 15 };
			std::vector<double> w{ 0, 1,   1, 0 };
			std::piecewise_linear_distribution<> dis(i.begin(), i.end(), w.begin());		// default 
			value = dis(engine);
			break;
		}

		case RAW:
		default:
			break;
	}

	return	m_range ? m_min + (unsigned long)value % m_range : value;
	//return	value;
}


bool CMyRand::Rand(unsigned char *buf, int size)
{
	register int	i;
	std::random_device r;
	std::seed_seq seed{ r(), r(), r(), r(), r(), r(), r(), r() };


	switch (m_type)
	{
	case STDLIB:
		for (i = 0; i < size; i++)
			buf[i] = (unsigned char)RandEngineRaw(1);
		break;

	case OPENSSL:
		for (i = 0; i < size; i++)
			buf[i] = (unsigned char)RandEngineRaw(1);
		break;

	case MSTD1988:
	{
		std::minstd_rand0 engine(seed);
		for (i = 0; i < size; i++)
			buf[i] = (unsigned char)DoDistribution(*&engine);
		break;
	}

	case MSTD1993:
	{
		std::minstd_rand engine(seed);
		for (i = 0; i < size; i++)
			buf[i] = (unsigned char)DoDistribution(*&engine);
		break;
	}

	case MT19937:
	{
		std::mt19937 engine(seed);
		for (i = 0; i < size; i++)
			buf[i] = (unsigned char)DoDistribution(*&engine);
		break;
	}

	case MT19937_64:
	{
		std::mt19937_64 engine(seed);
		for (i = 0; i < size; i++)
			buf[i] = (unsigned char)DoDistribution(*&engine);
		break;
	}

	case LANLUX24B:
	{
		std::ranlux24_base engine(seed);
		for (i = 0; i < size; i++)
			buf[i] = (unsigned char)DoDistribution(*&engine);
		break;
	}

		case LANLUX48B:
		{
			std::ranlux48_base engine(seed);
			for (i = 0; i < size; i++)
				buf[i] = (unsigned char)DoDistribution(*&engine);
			break;
		}

		case LANLUX24:
		{
			std::ranlux24 engine(seed);
			for (i = 0; i < size; i++)
				buf[i] = (unsigned char)DoDistribution(*&engine);
			break;
		}

		case LANLUX48:
		{
			std::ranlux48 engine(seed);
			for (i = 0; i < size; i++)
				buf[i] = (unsigned char)DoDistribution(*&engine);
			break;
		}

		case KNUTH_B:
		{
			std::knuth_b engine(seed);
			for (i = 0; i < size; i++)
				buf[i] = (unsigned char)DoDistribution(*&engine);
			break;
		}
		break;
	}

	return	true;
}


bool CMyRand::Rand(unsigned short *buf, int size)
{
	Rand((unsigned char *)buf, size * 2);

	return	true;
}


bool CMyRand::Rand(unsigned long *buf, int size)
{
	Rand((unsigned char *)buf, size * 4);

	return	true;
}


unsigned long CMyRand::RandEngineOpenSSL(int size)
{
	unsigned long value;


	switch (size)
	{
		case 1:
			RAND_bytes((unsigned char *)&value, 1);
			break;
		case 2:
			RAND_bytes((unsigned char *)&value, 2);
			break;
		case 4:
		default:
			RAND_bytes((unsigned char *)&value, 4);
			break;
	}

	return	m_range ? m_min + value % m_range : value;
}


unsigned long CMyRand::RandEngineRaw(int size)
{
	unsigned long value;


	switch (size)
	{
		case 1:
			value = rand();
			break;
		case 2:
			value = rand();
			break;
		case 4:
		default:
			value = (unsigned long)rand() | rand();
			break;
	}

	return	m_range ? m_min + value % m_range : value;
}